#include <string>

#include "config/Config.h"

int main(int argc, char* argv[]){
	/** Load Configs */
	std::string configPath = "config.lua";
	if (argc == 2) {
		configPath = argv[1];
	}
	Config::getInstance()->loadFrom(configPath);

	/** Shutdown */
	Config::releaseInstance();

	/** Exit Code */
	return 0;
}
