#include "Utils.h"

namespace utils {
	int getMajorVersion() {
		return PROJECT_VERSION_MAJOR;
	}

	int getMinorVersion() {
		return PROJECT_VERSION_MINOR;
	}

	int getPatchVersion() {
		return PROJECT_VERSION_PATCH;
	}

	const std::string getVersionStr() {
		return std::to_string(getMajorVersion()) + "."
			+ std::to_string(getMinorVersion()) + "."
			+ std::to_string(getPatchVersion());
	}
}
