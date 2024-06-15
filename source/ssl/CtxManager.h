#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>

#include "CtxHolder.h"

class CtxManager final {
public:
	CtxManager();
	~CtxManager();

	void load(const std::string& host,
		const std::string& keyPath, const std::string& cerPath);
	SSL_CTX* get(const std::string& host) const;
	SSL_CTX* getDefault() const;

private:
	std::map<std::string, std::shared_ptr<CtxHolder>> contexts;
	std::shared_ptr<CtxHolder> defaultCtx = nullptr;
	mutable std::mutex lock;

public:
	static CtxManager* getInstance();
	static void releaseInstance();

private:
	static CtxManager* instance;
};
