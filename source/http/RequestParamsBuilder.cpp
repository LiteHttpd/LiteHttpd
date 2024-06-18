#include "RequestParamsBuilder.h"
#include "Utils.h"
#include "log/Logger.h"

#include <memory>
#include <sstream>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>

void RequestParamsBuilder::build(
	RequestParams& params, evhttp_request* request) {
	if (!request) { return; }

	/** Get Connection */
	auto connection = evhttp_request_get_connection(request);

	/** Peer */
	{
		const char* peerAddr = nullptr;
		uint16_t peerPort = 0;
		evhttp_connection_get_peer(connection, &peerAddr, &peerPort);

		params.peerAddr = peerAddr;
		params.peerPort = peerPort;
	}

	/** Method */
	auto method = evhttp_request_get_command(request);
	switch (method) {
	case EVHTTP_REQ_GET:
		params.method = RequestParams::MethodType::GET;
		break;
	case EVHTTP_REQ_POST:
		params.method = RequestParams::MethodType::POST;
		break;
	case EVHTTP_REQ_HEAD:
		params.method = RequestParams::MethodType::HEAD;
		break;
	case EVHTTP_REQ_PUT:
		params.method = RequestParams::MethodType::PUT;
		break;
	case EVHTTP_REQ_DELETE:
		params.method = RequestParams::MethodType::DELETE_;
		break;
	case EVHTTP_REQ_OPTIONS:
		params.method = RequestParams::MethodType::OPTIONS;
		break;
	case EVHTTP_REQ_TRACE:
		params.method = RequestParams::MethodType::TRACE;
		break;
	case EVHTTP_REQ_CONNECT:
		params.method = RequestParams::MethodType::CONNECT;
		break;
	case EVHTTP_REQ_PATCH:
		params.method = RequestParams::MethodType::PATCH;
		break;
	case EVHTTP_REQ_PROPFIND:
		params.method = RequestParams::MethodType::PROPFIND;
		break;
	case EVHTTP_REQ_PROPPATCH:
		params.method = RequestParams::MethodType::PROPPATCH;
		break;
	case EVHTTP_REQ_MKCOL:
		params.method = RequestParams::MethodType::MKCOL;
		break;
	case EVHTTP_REQ_LOCK:
		params.method = RequestParams::MethodType::LOCK;
		break;
	case EVHTTP_REQ_UNLOCK:
		params.method = RequestParams::MethodType::UNLOCK;
		break;
	case EVHTTP_REQ_COPY:
		params.method = RequestParams::MethodType::COPY;
		break;
	case EVHTTP_REQ_MOVE:
		params.method = RequestParams::MethodType::MOVE;
		break;
	}

	/** Decode URI */
	const char* uriSource = evhttp_request_get_uri(request);
	auto uri = std::unique_ptr<evhttp_uri, void(*)(evhttp_uri*)>(evhttp_uri_parse(uriSource), evhttp_uri_free);

	/** Path */
	{
		const char* path = evhttp_uri_get_path(uri.get());
		params.path = path;
	}

	/** Query */
	{
		const char* query = evhttp_uri_get_query(uri.get());

		auto queryKV = std::unique_ptr<evkeyvalq, void(*)(evkeyvalq*)>(new evkeyvalq, evhttp_clear_headers);
		evhttp_parse_query_str(query, queryKV.get());

		for (auto i = queryKV->tqh_first; i != nullptr; i = i->next.tqe_next) {
			params.params.insert(std::make_pair(std::string{ i->key }, std::string{ i->value }));
		}
	}

	/** Headers */
	if (auto headers = evhttp_request_get_input_headers(request)) {
		for (auto i = headers->tqh_first; i != nullptr; i = i->next.tqe_next) {
			params.headers.insert(std::make_pair(std::string{ i->key }, std::string{ i->value }));
		}
	}

	/** Protocol */
	{
		auto it = params.headers.find("X-Forwarded-Proto");
		if (it != params.headers.end()) {
			params.protocol = (it->second == "https")
				? RequestParams::ProtocolType::HTTPS
				: RequestParams::ProtocolType::HTTP;
		}
	}

	/** Host */
	{
		const char* host = evhttp_request_get_host(request);
		std::tie(params.addr, params.port) = RequestParamsBuilder::parseHostStr(
			host, params.protocol == RequestParams::ProtocolType::HTTPS);
	}

	/** Data */
	{
		auto buffer = evhttp_request_get_input_buffer(request);
		params.data.resize(evbuffer_get_length(buffer));
		evbuffer_copyout(buffer, params.data.data(), params.data.size());
	}

	/** Content */
	params.content = reinterpret_cast<void*>(request);

	/** Callback */
	params.addHeaderFunc = [](void* content, const std::string& key, const std::string& value) {
		if (auto request = reinterpret_cast<evhttp_request*>(content)) {
			if (auto header = evhttp_request_get_output_headers(request)) {
				evhttp_add_header(header, key.c_str(), value.c_str());
			}
		}
		};
	params.replyFunc = [](void* content, int responseCode, const std::vector<char>& data) {
		if (auto request = reinterpret_cast<evhttp_request*>(content)) {
			auto buffer = std::unique_ptr<evbuffer, void(*)(evbuffer*)>(evbuffer_new(), evbuffer_free);
			evbuffer_add(buffer.get(), data.data(), data.size());

			evhttp_send_reply(request, responseCode, utils::getResponseReason(responseCode), buffer.get());
		}
		};
	params.replyStartFunc = [](void* content, int responseCode) {
		if (auto request = reinterpret_cast<evhttp_request*>(content)) {
			evhttp_send_reply_start(request, responseCode, utils::getResponseReason(responseCode));
		}
		};
	params.replyDataFunc = [](void* content, const std::vector<char>& data) {
		if (auto request = reinterpret_cast<evhttp_request*>(content)) {
			auto buffer = std::unique_ptr<evbuffer, void(*)(evbuffer*)>(evbuffer_new(), evbuffer_free);
			evbuffer_add(buffer.get(), data.data(), data.size());

			evhttp_send_reply_chunk(request, buffer.get());
		}
		};
	params.replyEndFunc = [](void* content) {
		if (auto request = reinterpret_cast<evhttp_request*>(content)) {
			evhttp_send_reply_end(request);
		}
		};
	params.logFunc = [addr = params.addr](RequestParams::LogLevel level, const std::string& data) {
		switch (level) {
		case RequestParams::LogLevel::DEBUG:
			Logger::debug("[" + addr + "] " + data);
			break;
		case RequestParams::LogLevel::INFO:
			Logger::info("[" + addr + "] " + data);
			break;
		case RequestParams::LogLevel::WARNING:
			Logger::warning("[" + addr + "] " + data);
			break;
		case RequestParams::LogLevel::ERROR_:
			Logger::error("[" + addr + "] " + data);
			break;
		case RequestParams::LogLevel::CRITICAL:
			Logger::fatal("[" + addr + "] " + data);
			break;
		}
		};
}

std::tuple<std::string, uint16_t> RequestParamsBuilder::parseHostStr(const std::string& hostStr, bool https) {
	std::string host;
	uint16_t port = https ? 443 : 80;

	size_t colIndex = hostStr.find(':');
	if (colIndex >= 0 && colIndex < hostStr.length()) {
		host = hostStr.substr(0, colIndex);
		auto portStr = hostStr.substr(colIndex + 1, hostStr.length() - colIndex - 1);
		std::istringstream ss(portStr);
		ss >> port;
	}
	else {
		host = hostStr;
	}

	return { host, port };
}
