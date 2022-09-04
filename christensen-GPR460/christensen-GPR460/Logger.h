#pragma once

#include <iostream>
#include <cassert>
#include "Windows.h"
#include <time.h>
#include <iomanip>

class Logger
{

#pragma region Helpers
private:
	template<typename Tout>
	static void logTimestamp(Tout& out)
	{
		//Algorithm from https://stackoverflow.com/a/58607142
		time_t utcTime = std::time(nullptr);
		struct tm localTime;
		localtime_s(&localTime, &utcTime);
		auto formatted = std::put_time(&localTime, "%H:%M:%S");
		out << formatted;
	}

	template<typename T>
	static void _logVarArgs(std::ostream& out, const T& t) { out << t; }

	template<typename T, typename... Ts>
	static void _logVarArgs(std::ostream& out, const T& t, const Ts&... ts)
	{
		_logVarArgs<T   >(out, t);
		_logVarArgs<Ts...>(out, ts...);
	}

	struct LevelInfo
	{
		DWORD sysHandleID;
		std::ostream* out;

		std::string header;
		WORD color;

		LevelInfo(DWORD sysHandleID, std::ostream& out, std::string header, WORD color);
	};

	static constexpr unsigned int NAME_WIDTH = 10;

#pragma endregion Helpers

#pragma region Public interface
	
public:
	static LevelInfo lFatal;
	static LevelInfo lError;
	static LevelInfo lWarn;
	static LevelInfo lInfo;
	static LevelInfo lDebug;

	template<typename... Ts>
	static void log(const LevelInfo& level, const Ts&... ts)
	{
		SetConsoleTextAttribute(GetStdHandle(level.sysHandleID), level.color); //Set color to red
		logTimestamp(level.out);
		*level.out << std::setw(NAME_WIDTH) << " [" << level.header << "] ";
		_logVarArgs(*level.out, ts...);
		*level.out << std::endl;
	}

	template<typename... Ts>
	static void fatal(const Ts&... ts)
	{
		log(lFatal, ts...);
		abort();
	}

	template<typename... Ts>
	static void error(const Ts&... ts) { log(lError, ts...); }

	template<typename... Ts>
	static void warn (const Ts&... ts) { log(lWarn , ts...); }

	template<typename... Ts>
	static void info (const Ts&... ts) { log(lInfo , ts...); }

	template<typename... Ts>
	static void debug(const Ts&... ts) { log(lDebug, ts...); }

#pragma endregion Public interface

};
