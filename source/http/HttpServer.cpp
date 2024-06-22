#include "HttpServer.h"
#include "EventBase.h"
#include "RequestParamsBuilder.h"
#include "Utils.h"
#include "log/Logger.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#endif

#ifdef _WIN32
#define stat _stat
#define fstat _fstat
#define open _open
#define close _close
#define O_RDONLY _O_RDONLY
#endif

HttpServer::HttpServer(ProtocolType protocol, 
	const std::string& address, uint16_t port,
	const FindCtxFunc& findCtx, const DefaultCtxFunc& defaultCtx,
	const FindModuleFunc& findModule)
	: protocol(protocol),
	findCtx(findCtx), defaultCtx(defaultCtx),
	findModule(findModule) {
	Logger::info("Create http server: " + address + ":" + std::to_string(port));
	Logger::info("Protocol: " + std::string{ (protocol == ProtocolType::HTTPS) ? "https" : "http" });

	/** Init Socket */
#ifdef _WIN32
	WSADATA WSAData;
	if (WSAStartup(0x101, &WSAData) != 0) {
		Logger::error("Can't init windows socket!");
	}
#endif

	/** Init Event Http Connection */
	this->http = evhttp_new(EventBase::getInstance()->getBase());

	/** Set Connection Callback */
	evhttp_set_bevcb(this->http,
		HttpServer::connectionCallback, reinterpret_cast<void*>(this));

	/** Set Request Callback */
	evhttp_set_gencb(this->http,
		HttpServer::requestCallback, reinterpret_cast<void*>(this));

	/** Bind Socket */
	this->handle = evhttp_bind_socket_with_handle(this->http, address.c_str(), port);
	if (!this->handle) {
		Logger::error("Can't bind to socket!");
	}
}

HttpServer::~HttpServer() {
	evhttp_free(this->http);
}

void HttpServer::printBindStatus() const {
	/** Get fd */
	int fd = evhttp_bound_socket_get_fd(handle);

	/** Init Socket Hop */
	union SockHop {
		sockaddr_storage ss;
		sockaddr sa;
		sockaddr_in in;
		sockaddr_in6 i6;
	};
	SockHop ss{};
	ev_socklen_t socklen = sizeof(ss);
	memset(&ss, 0, sizeof(ss));

	/** Get Socket Name */
	if (getsockname(fd, &ss.sa, &socklen)) {
		Logger::error("getsockname() failed!");
		return;
	}

	/** Get Address Family */
	void* inaddr = nullptr;
	int gotPort = -1;
	if (ss.ss.ss_family == AF_INET) {
		gotPort = ntohs(ss.in.sin_port);
		inaddr = &ss.in.sin_addr;
	}
	else if (ss.ss.ss_family == AF_INET6) {
		gotPort = ntohs(ss.i6.sin6_port);
		inaddr = &ss.i6.sin6_addr;
	}
	else {
		Logger::info("Weird address family " + std::to_string(ss.ss.ss_family));
		return;
	}

	/** Get Address */
	char addrbuf[128] = { 0 };
	const char* addr = evutil_inet_ntop(ss.ss.ss_family, inaddr, addrbuf, sizeof(addrbuf));
	if (addr) {
		Logger::info("Listening on " + std::string{ addr } + ":" + std::to_string(gotPort));
	}
	else {
		Logger::error("evutil_inet_ntop failed!");
		return;
	}
}

void HttpServer::setDefaultPage(const std::vector<char>& pageData) {
	std::lock_guard locker(this->defaultPageLock);
	this->defaultPage = pageData;
}

bufferevent* HttpServer::connectionCallback(event_base* base, void* arg) {
	if (auto server = reinterpret_cast<HttpServer*>(arg)) {
		if (server->protocol == ProtocolType::HTTP) {
			Logger::info("Create buffer event socket for http connection.");
			return bufferevent_socket_new(base, -1,
				BEV_OPT_CLOSE_ON_FREE);
		}
		else if (server->protocol == ProtocolType::HTTPS) {
			if (auto defaultCtx = server->defaultCtx()) {
				Logger::info("Create buffer event socket for https connection.");
				return bufferevent_openssl_socket_new(base, -1,
					SSL_new(defaultCtx), BUFFEREVENT_SSL_ACCEPTING,
					BEV_OPT_CLOSE_ON_FREE);
			}
		}
	}
	Logger::error("Can't find server object for connection!");
	return nullptr;
}

void HttpServer::requestCallback(evhttp_request* request, void* arg) {
	if (auto server = reinterpret_cast<HttpServer*>(arg)) {
		/** Params */
		RequestParams params;
		RequestParamsBuilder::build(params, request, server->protocol == HttpServer::ProtocolType::HTTPS);
		Logger::info("Accept http(s) request: " + params.addr + ":" + std::to_string(params.port));
		Logger::info("Peer address: " + params.peerAddr + ":" + std::to_string(params.peerPort));
		Logger::info("Protocol: " + std::string{ (params.protocol == RequestParams::ProtocolType::HTTPS) ? "https" : "http" });
		Logger::info("Path: " + params.path);

		/** Find Module */
		if (auto module = server->findModule(params.addr)) {
			Logger::info("Find module for address: " + params.addr);
			module->processRequest(params);

			/** No Response */
			if (params.getResponseCode() <= 0) {
				Logger::warning("Not reply in moudule: " + params.addr + ", send 503.");
				evhttp_send_reply(request, 503, utils::getResponseReason(503), nullptr);
			}
			return;
		}

		/** Default */
		Logger::info("Can't find module for address: " + params.addr + ", using default page.");
		auto buffer = std::unique_ptr<evbuffer, void(*)(evbuffer*)>(evbuffer_new(), evbuffer_free);
		{
			std::lock_guard locker(server->defaultPageLock);
			evbuffer_add(buffer.get(), server->defaultPage.data(), server->defaultPage.size());
		}
		evhttp_send_reply(request, 200, utils::getResponseReason(200), buffer.get());
		return;
	}
	
	/** Error */
	Logger::error("Can't get server object, send 500.");
	evhttp_send_reply(request, 500, utils::getResponseReason(500), nullptr);
	return;
}
