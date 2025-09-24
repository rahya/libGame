#pragma once

#include <cstdint>
#include <chrono>
#include <algorithm>

namespace misc {


	class StepFixedInterval {

	public:
		StepFixedInterval(int64_t intervalTimeMs) { interval = intervalTimeMs;  reset(); }

		void makeNextStep() {
			count += 1;
		}

		void makeNextStep(int64_t aSleepTime) {
			if (aSleepTime != interval) {
				_start = _start + count * interval;
				interval = aSleepTime;
				count = 0;
			}
			else {
				count += 1;
			}
		}

		int64_t timeUntilNextStep() {
			int64_t nextStep = _start + count * interval;
			int64_t millisec_since_last_call = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			return std::max(static_cast<int64_t>(0), nextStep - millisec_since_last_call);
		}

		void reset() {
			_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			count = 0;
		}

		void reset(int64_t intervalTimeMs) {
			interval = intervalTimeMs;
			reset();
		}

		int64_t getInterval() const {
			return interval;
		}

	private:
		int64_t interval = 1000;
		int64_t _start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int64_t count = 0;

	};
};