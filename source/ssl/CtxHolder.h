#pragma once

#include <string>
#include <functional>
#include <openssl/ssl.h>

class CtxHolder final {
public:
	using FindCtxFunc = std::function<SSL_CTX* (const std::string&)>;
	CtxHolder() = delete;
	CtxHolder(const std::string& keyPath, const std::string& cerPath,
		const FindCtxFunc& findCtxFunc);
	~CtxHolder();

	bool checkCer() const;
	void loadCertificateChain(const std::string& cerPath);
	void loadPrivateKey(const std::string& keyPath);

	void setFindCtxFunc(const FindCtxFunc& findCtxFunc);

	SSL_CTX* getCtx() const;

private:
	static int sniCallbackEntry(SSL* ssl, int* ad, void* arg);

private:
	SSL_CTX* sslCtx = nullptr;
	FindCtxFunc findCtx;
};
