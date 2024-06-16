#pragma once

#include <string>
#include <vector>

namespace utils {
	int getMajorVersion();
	int getMinorVersion();
	int getPatchVersion();

	const std::string getVersionStr();

	const std::vector<char> loadFile(const std::string& path);

	const char* getResponseReason(int code);
}
