#include "Logger.h"

#include <glog/logging.h>
#include <iostream>

static void myPrefixFormatter(std::ostream& s, const google::LogMessage& m, void*) {
	s << '['
		<< google::GetLogSeverityName(m.severity())[0]
		<< ' '
		<< std::setw(4) << 1900 + m.time().year()
		<< std::setw(2) << 1 + m.time().month()
		<< std::setw(2) << m.time().day()
		<< ' '
		<< std::setw(2) << m.time().hour() << ':'
		<< std::setw(2) << m.time().min() << ':'
		<< std::setw(2) << m.time().sec() << '.'
		<< std::setw(6) << m.time().usec()
		<< ' '
		<< std::setfill(' ') << std::setw(5)
		<< m.thread_id() << std::setfill('0')
		<< ']';
}

Logger::Logger() {
	/** Config GLog */
	FLAGS_log_file_header = "LiteHttpd";
	FLAGS_log_year_in_prefix = true;
	FLAGS_log_dir = "logs";
	FLAGS_timestamp_in_logfile_name = true;

	FLAGS_logtostderr = false;
	FLAGS_alsologtostderr = true;
	FLAGS_colorlogtostderr = true;

	FLAGS_logbufsecs = 0;
	FLAGS_max_log_size = 10;
	FLAGS_stop_logging_if_full_disk = true;

	/** Init GLog */
	google::InitGoogleLogging("LiteHttpd");
	google::InstallPrefixFormatter(myPrefixFormatter);
}

Logger::~Logger() {
	/** Shutdown */
	google::ShutdownGoogleLogging();
}

void Logger::debug(const std::string& mes) {
	Logger::getInstance()->debugInternal(mes);
}

void Logger::info(const std::string& mes) {
	Logger::getInstance()->infoInternal(mes);
}

void Logger::warning(const std::string& mes) {
	Logger::getInstance()->warningInternal(mes);
}

void Logger::error(const std::string& mes) {
	Logger::getInstance()->errorInternal(mes);
}

void Logger::fatal(const std::string& mes) {
	Logger::getInstance()->fatalInternal(mes);
}

void Logger::debugInternal(const std::string& mes) {
	DLOG(INFO) << "(Debug)" << mes;
}

void Logger::infoInternal(const std::string& mes) {
	LOG(INFO) << mes;
}

void Logger::warningInternal(const std::string& mes) {
	LOG(WARNING) << mes;
}

void Logger::errorInternal(const std::string& mes) {
	LOG(ERROR) << mes;
}

void Logger::fatalInternal(const std::string& mes) {
	LOG(FATAL) << mes;
}

Logger* Logger::getInstance() {
	return Logger::instance ? Logger::instance
		: (Logger::instance = new Logger{});
}

void Logger::releaseInstance() {
	if (Logger::instance) {
		delete Logger::instance;
		Logger::instance = nullptr;
	}
}

Logger* Logger::instance = nullptr;
