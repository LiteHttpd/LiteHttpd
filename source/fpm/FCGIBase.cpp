#include "FCGIBase.h"
#include "log/Logger.h"

#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

extern "C" {
#include <fcgi.h>
}

FCGIBase::FCGIBase() {
	/** Init Socket */
#ifdef _WIN32
	WSADATA WSAData;
	if (WSAStartup(0x101, &WSAData) != 0) {
		Logger::error("Can't init windows socket for FCGI!");
	}
#endif
}

FCGIBase::~FCGIBase() {}

static void dataGetFunc(const char* data, int size, void* arg) {
	if (auto dataPtr = reinterpret_cast<std::vector<char>*>(arg)) {
		size_t currentSize = dataPtr->size();
		dataPtr->resize(currentSize + size);
		std::memcpy(&(dataPtr->data())[currentSize], data, size);
	}
}

const FCGIBase::Result FCGIBase::sendData(
	const std::string& addr, uint16_t port, const ParamList& params,
	const std::vector<char>& postData) {
	/** Fix Content Length */
	ParamList paramsTemp = params;
	paramsTemp["CONTENT_LENGTH"] = std::to_string(postData.size());

	/** Create FCGI Object */
	FastCgi_t fcgi{};
	FastCgi_init(&fcgi);

	/** Set Request ID */
	setRequestId(&fcgi, this->requestIDCounter++);

	/** Start Connect */
	if (startConnect(&fcgi, addr.c_str(), port) <= 0) {
		Logger::error("Can't open socket for FCGI, address: " + addr + ":" + std::to_string(port));
		return { {}, false };
	}

	/** Send Request Start */
	sendStartRequestRecord(&fcgi);

	/** Send Params */
	for (auto& i : paramsTemp) {
		sendParams(&fcgi, i.first.c_str(), i.second.c_str());
	}

	/** Send Data */
	if (postData.size() > 0) {
		sendPostData(&fcgi, postData.data(), postData.size());
	}

	/** Send End Request */
	sendEndRequestRecord(&fcgi);

	/** Get Data */
	std::vector<char> result;
	readResponseData(&fcgi, dataGetFunc, &result);

	/** Close FCGI Socket */
	FastCgi_finit(&fcgi);

	return { result, true };
}

FCGIBase* FCGIBase::getInstance() {
	return FCGIBase::instance ? FCGIBase::instance
		: (FCGIBase::instance = new FCGIBase{});
}

void FCGIBase::releaseInstance() {
	if (FCGIBase::instance) {
		delete FCGIBase::instance;
		FCGIBase::instance = nullptr;
	}
}

FCGIBase* FCGIBase::instance = nullptr;
