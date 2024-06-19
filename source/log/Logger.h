#pragma once

#include <string>

class Logger final {
public:
	Logger();
	~Logger();

	static void debug(const std::string& mes);
	static void info(const std::string& mes);
	static void warning(const std::string& mes);
	static void error(const std::string& mes);
	static void fatal(const std::string& mes);

private:
	void debugInternal(const std::string& mes);
	void infoInternal(const std::string& mes);
	void warningInternal(const std::string& mes);
	void errorInternal(const std::string& mes);
	void fatalInternal(const std::string& mes);

	static void createDirIfNeed(const std::string& path);

public:
	static Logger* getInstance();
	static void releaseInstance();

private:
	static Logger* instance;
};
