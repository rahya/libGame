#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <time.h>

class Time {

public:
  static void get(int& year, int& month, int& day, int& hour, int& min, int& second) {
	time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm utc_tm;
#ifdef _WIN32
	gmtime_s(&utc_tm, &tt);
#else
	gmtime_r(&tt, &utc_tm);
#endif
	year = utc_tm.tm_year + 1900;
	month = utc_tm.tm_mon + 1;
	day = utc_tm.tm_mday;

	hour = utc_tm.tm_hour;
	min = utc_tm.tm_min;
	second = utc_tm.tm_sec;
  }

};