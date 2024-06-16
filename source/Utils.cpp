#include "Utils.h"

#include <fstream>

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

	const std::vector<char> loadFile(const std::string& path) {
		std::ifstream inputStream;
		inputStream.open(path);

		inputStream.seekg(0, std::ios::end);
		size_t length = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		std::vector<char> buffer;
		buffer.resize(length);
		inputStream.read(buffer.data(), length);
		inputStream.close();

		return buffer;
	}
}
