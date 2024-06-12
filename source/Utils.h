#pragma once

#include <string>

namespace utils {
	int getMajorVersion();
	int getMinorVersion();
	int getPatchVersion();

	const std::string getVersionStr();
}
