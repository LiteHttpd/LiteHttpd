#include "HttpServer.h"
#include "EventBase.h"

#include <sys/types.h>
#include <sys/stat.h>

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
	const FindCtxFunc& findCtx, const DefaultCtxFunc& defaultCtx)
	: protocol(protocol), findCtx(findCtx), defaultCtx(defaultCtx) {
	/** Init Socket */
#ifdef _WIN32
	WSADATA WSAData;
	int socketStatus = WSAStartup(0x101, &WSAData);
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
		printf("getsockname() failed");
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
		printf("Weird address family %d\n", ss.ss.ss_family);
		return;
	}

	/** Get Address */
	char addrbuf[128] = { 0 };
	const char* addr = evutil_inet_ntop(ss.ss.ss_family, inaddr, addrbuf, sizeof(addrbuf));
	if (addr) {
		printf("Listening on %s:%d\n", addr, gotPort);
	}
	else {
		printf("evutil_inet_ntop failed\n");
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
			return bufferevent_socket_new(base, -1,
				BEV_OPT_CLOSE_ON_FREE);
		}
		else if (server->protocol == ProtocolType::HTTPS) {
			if (auto defaultCtx = server->defaultCtx()) {
				return bufferevent_openssl_socket_new(base, -1,
					SSL_new(defaultCtx), BUFFEREVENT_SSL_ACCEPTING,
					BEV_OPT_CLOSE_ON_FREE);
			}
		}
	}
	return nullptr;
}

void HttpServer::requestCallback(evhttp_request* request, void* arg) {
	/** TODO */
	evbuffer* buf = evbuffer_new();
	if (auto server = reinterpret_cast<HttpServer*>(arg)) {
		std::lock_guard locker(server->defaultPageLock);
		evbuffer_add(buf, server->defaultPage.data(), server->defaultPage.size());
	}
	evhttp_send_reply(request, 200, "OK", buf);
	evbuffer_free(buf);
}
