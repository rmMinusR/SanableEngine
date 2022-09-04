#include "Logger.h"

inline Logger::LevelInfo::LevelInfo(DWORD sysHandleID, std::ostream& out, std::string header, WORD color)
{
	this->sysHandleID = sysHandleID;
	this->out         = &out;
	this->header      = header;
	this->color       = color;
}

Logger::LevelInfo Logger::lFatal = Logger::LevelInfo(STD_ERROR_HANDLE, std::cerr, "FATAL", 0b11000000);
Logger::LevelInfo Logger::lError = Logger::LevelInfo(STD_ERROR_HANDLE, std::cerr, "ERROR", 0b00001100);
Logger::LevelInfo Logger::lWarn  = Logger::LevelInfo(STD_ERROR_HANDLE, std::cout, "WARN" , 0b00001110);
Logger::LevelInfo Logger::lInfo  = Logger::LevelInfo(STD_ERROR_HANDLE, std::cout, "INFO" , 0b00000111);
Logger::LevelInfo Logger::lDebug = Logger::LevelInfo(STD_ERROR_HANDLE, std::cout, "DEBUG", 0b00001000);
