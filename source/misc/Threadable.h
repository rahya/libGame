#pragma once

#include <thread>
#include <mutex>

#include "log.h"

namespace misc {


	class Threadable {

		enum class step {
			none,
			initializing,
			initialized,
			running,
			closing,
			closed,
		};

		step _currentStep = step::none;
		std::thread _thread;

	public:


	public:
		Threadable() = default;


		bool isInitializing() const { return _currentStep == step::initializing; }
		bool isInitialized() const { return _currentStep == step::initialized; }
		bool isRunning() const { return _currentStep == step::running; }
		bool isClosing() const { return _currentStep == step::closing; }
		bool isClosed() const { return _currentStep == step::closed; }

		bool init() {
			setInitializing();
			if (subInit()) {
				setInitialized();
				return true;
			}
			setClosed();
			return false;
		}

		virtual const char* getLogPrefix() const = 0;

		void run() {
			if (_currentStep == step::running) {
				LOG_WARNING(getLogPrefix() << "Already running");
			}
			else if (_currentStep != step::initialized) {
				LOG_WARNING(getLogPrefix() << "Object not initialized");
			}
			else {
				//LOG_INFO(getLogPrefix() << "Starting");
				_thread = std::thread{ &Threadable::runAux, this };
			}

		}
		void join() {
			_thread.join();
		}

		void setClosing() { _currentStep = step::closing; }
	protected:
		void setInitializing() { _currentStep = step::initializing; }
		void setInitialized() { _currentStep = step::initialized; }
		void setRunning() { _currentStep = step::running; }
		void setClosed() { _currentStep = step::closed; }


		virtual void subRun() = 0;
		virtual bool subInit() { return true; }

		void runAux() {
			srand(static_cast<int>(time(nullptr)));
			setRunning();
			subRun();
			setClosed();
		}


	private:
	};


};