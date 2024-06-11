#include "config/Config.h"

int main(int argc, char* argv[]){
	/** Load Configs */
	Config::getInstance()->loadFrom("config.lua");

	/** Shutdown */
	Config::releaseInstance();

	/** Exit Code */
	return 0;
}
