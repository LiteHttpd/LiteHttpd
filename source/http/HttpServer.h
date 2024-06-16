#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <event2/http.h>
#include <openssl/ssl.h>

class HttpServer final {
public:
	enum class ProtocolType {
		HTTP, HTTPS
	};
	using FindCtxFunc = std::function<SSL_CTX* (const std::string&)>;
	using DefaultCtxFunc = std::function<SSL_CTX* (void)>;
	HttpServer() = delete;
	HttpServer(ProtocolType protocol, const std::string& address, uint16_t port,
		const FindCtxFunc& findCtx, const DefaultCtxFunc& defaultCtx);
	~HttpServer();

	void printBindStatus() const;

	void setDefaultPage(const std::vector<char>& pageData);

private:
	static bufferevent* connectionCallback(event_base* base, void* arg);
	static void requestCallback(evhttp_request* request, void* arg);

private:
	const ProtocolType protocol = ProtocolType::HTTP;
	const FindCtxFunc findCtx;
	const DefaultCtxFunc defaultCtx;

	std::vector<char> defaultPage;
	std::mutex defaultPageLock;

	evhttp* http = nullptr;
	evhttp_bound_socket* handle = nullptr;
};
