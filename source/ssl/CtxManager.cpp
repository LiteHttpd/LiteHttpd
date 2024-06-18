#include "CtxManager.h"
#include "log/Logger.h"

static void* cryptoAlloc(size_t num, const char* file, int line) {
	return malloc(num);
}

static void* cryptoRealloc(void* addr, size_t num,
	const char* file, int line) {
	return realloc(addr, num);
}

static void cryptoFree(void* addr, const char* file, int line) {
	free(addr);
}

CtxManager::CtxManager() {
	/** Init OpenSSL */
	CRYPTO_set_mem_functions(cryptoAlloc, cryptoRealloc, cryptoFree);
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	/** Default Ctx */
	{
		std::lock_guard locker(this->lock);
		auto getCtxFunc = [](const std::string& host) {
			return CtxManager::getInstance()->get(host);
			};
		this->defaultCtx = std::make_shared<CtxHolder>(
			std::string{}, std::string{}, getCtxFunc);
	}
}

CtxManager::~CtxManager() {
	EVP_cleanup();
}

void CtxManager::load(const std::string& host,
	const std::string& keyPath, const std::string& cerPath) {
	std::lock_guard locker(this->lock);

	auto getCtxFunc = [](const std::string& host) {
		return CtxManager::getInstance()->get(host);
		};

	auto ctxHolder = std::make_unique<CtxHolder>(keyPath, cerPath, getCtxFunc);
	if (ctxHolder->checkCer()) {
		Logger::info("Storage SSL CTX for: " + host);
		this->contexts.insert({ host,
			std::shared_ptr<CtxHolder>{ ctxHolder.release() } });
	}
}

SSL_CTX* CtxManager::get(const std::string& host) const {
	std::lock_guard locker(this->lock);

	auto it = this->contexts.find(host);
	if (it != this->contexts.end()) {
		return it->second->getCtx();
	}

	return nullptr;
}

SSL_CTX* CtxManager::getDefault() const {
	std::lock_guard locker(this->lock);
	return this->defaultCtx->getCtx();
}

CtxManager* CtxManager::getInstance() {
	return CtxManager::instance ? CtxManager::instance
		: (CtxManager::instance = new CtxManager{});
}

void CtxManager::releaseInstance() {
	if (CtxManager::instance) {
		delete CtxManager::instance;
		CtxManager::instance = nullptr;
	}
}

CtxManager* CtxManager::instance = nullptr;
