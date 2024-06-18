#include "CtxHolder.h"
#include "log/Logger.h"

#include <openssl/err.h>

CtxHolder::CtxHolder(const std::string& keyPath, const std::string& cerPath,
	const FindCtxFunc& findCtxFunc) {
	Logger::info("Load certificate: " + cerPath);
	Logger::info("Load private key: " + keyPath);

	/** Create SSL Context */
	this->sslCtx = SSL_CTX_new(TLS_server_method());

	/** Set ECDH */
	SSL_CTX_set_options(this->sslCtx,
		SSL_OP_SINGLE_DH_USE |
		SSL_OP_SINGLE_ECDH_USE);
	if (EC_KEY* ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)) {
		if (SSL_CTX_set_tmp_ecdh(this->sslCtx, ecdh) != 1) {
			Logger::error("Can't set EC KEY for SSL CTX!");
		}
	}
	else {
		Logger::error("Can't create EC KEY!");
	}
	
	/** Load Certificate Chain And Key */
	if (!cerPath.empty()) {
		this->loadCertificateChain(cerPath);
	}
	if (!keyPath.empty()) {
		this->loadPrivateKey(keyPath);
	}

	/** Set SNI Callback */
	SSL_CTX_set_tlsext_servername_callback(this->sslCtx, CtxHolder::sniCallbackEntry);
	SSL_CTX_set_tlsext_servername_arg(this->sslCtx, reinterpret_cast<void*>(this));
	this->setFindCtxFunc(findCtxFunc);
}

CtxHolder::~CtxHolder() {
	if (this->sslCtx) {
		SSL_CTX_free(this->sslCtx);
	}
}

bool CtxHolder::checkCer() const {
	if (this->sslCtx) {
		return SSL_CTX_check_private_key(this->sslCtx) > 0;
	}
	return false;
}

void CtxHolder::loadCertificateChain(const std::string& cerPath) {
	if (this->sslCtx) {
		if (SSL_CTX_use_certificate_chain_file(this->sslCtx, cerPath.c_str()) != 1) {
			Logger::error("Can't set certificate: " + cerPath);
		}
	}
}

void CtxHolder::loadPrivateKey(const std::string& keyPath) {
	if (this->sslCtx) {
		if (SSL_CTX_use_PrivateKey_file(this->sslCtx, keyPath.c_str(), SSL_FILETYPE_PEM) != 1) {
			Logger::error("Can't set private key: " + keyPath);
		}
	}
}

void CtxHolder::setFindCtxFunc(const FindCtxFunc& findCtxFunc) {
	this->findCtx = findCtxFunc;
}

SSL_CTX* CtxHolder::getCtx() const {
	return this->sslCtx;
}

int CtxHolder::sniCallbackEntry(SSL* ssl, int* /*ad*/, void* arg) {
	/** Get Holder */
	if (auto holder = reinterpret_cast<CtxHolder*>(arg)) {
		/** Get Server Name */
		if (auto serverName = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name)) {
			if (auto ctx = holder->findCtx(serverName)) {
				/** Log */
				Logger::info("Find SSL context for server name: " + std::string{ serverName });

				/** Set Context */
				SSL_set_SSL_CTX(ssl, ctx);

				/** OK */
				return SSL_TLSEXT_ERR_OK;
			}
			else {
				Logger::error("Can't find SSL context for server name: " + std::string{ serverName });
			}
		}
		else {
			Logger::error("Can't get SSL server name.");
		}
	}

	/** Failed */
	return SSL_TLSEXT_ERR_ALERT_FATAL;
}
