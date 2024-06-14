#include "CtxHolder.h"

#include <openssl/err.h>

CtxHolder::CtxHolder(const std::string& keyPath, const std::string& cerPath,
	const FindCtxFunc& findCtxFunc) {
	/** Create SSL Context */
	this->sslCtx = SSL_CTX_new(TLS_server_method());

	/** Set ECDH */
	SSL_CTX_set_options(this->sslCtx,
		SSL_OP_SINGLE_DH_USE |
		SSL_OP_SINGLE_ECDH_USE);
	if (EC_KEY* ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)) {
		SSL_CTX_set_tmp_ecdh(this->sslCtx, ecdh);
	}
	
	/** Load Certificate Chain And Key */
	this->loadCertificateChain(cerPath);
	this->loadPrivateKey(keyPath);

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
		SSL_CTX_use_certificate_chain_file(this->sslCtx, cerPath.c_str());
	}
}

void CtxHolder::loadPrivateKey(const std::string& keyPath) {
	if (this->sslCtx) {
		SSL_CTX_use_PrivateKey_file(this->sslCtx, keyPath.c_str(), SSL_FILETYPE_PEM);
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
				/** Set Context */
				SSL_set_SSL_CTX(ssl, ctx);

				/** OK */
				return SSL_TLSEXT_ERR_OK;
			}
		}
	}

	/** Failed */
	return SSL_TLSEXT_ERR_ALERT_FATAL;
}
