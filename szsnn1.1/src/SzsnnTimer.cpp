#include "SzsnnTimer.hpp"

namespace szsnn {

SzsnnTimer::TimerState SzsnnTimer::resetTimer()
{
	using namespace boost::posix_time;
	state = SZSNN_TIMER_RESET;
	cycle = 0;
	elapsedSum = 0;
	started = ptime();
	resumed = ptime();
	stopped = ptime();
	return state;
};

SzsnnTimer::TimerState SzsnnTimer::startTimer()
{
	using namespace boost::posix_time;
	if (state == SZSNN_TIMER_RESET)
	{
		started = ptime(microsec_clock::local_time());
		resumed = ptime(started);
		state = SZSNN_TIMER_RUN;
		return state;
	}
	if (state == SZSNN_TIMER_STOP) {resetTimer(); startTimer(); }
	return SZSNN_TIMER_ERROR;
};


SzsnnTimer::TimerState SzsnnTimer::pauseTimer()
{
	using namespace boost::posix_time;
	if (state != SZSNN_TIMER_RUN) return SZSNN_TIMER_ERROR;
	elapsedSum += elapsedFrom(resumed);
	state = SZSNN_TIMER_PAUSE;
	return state;
}
SzsnnTimer::TimerState SzsnnTimer::resumeTimer()
{
	using namespace boost::posix_time;
	if (state != SZSNN_TIMER_PAUSE) return SZSNN_TIMER_ERROR;
	resumed = ptime(microsec_clock::local_time());
	state = SZSNN_TIMER_RUN;
	return state;
}

SzsnnTimer::TimerState SzsnnTimer::continueTimer()
{
	using namespace boost::posix_time;
	if (state == SZSNN_TIMER_RESET) return startTimer();
	if (state == SZSNN_TIMER_PAUSE) return resumeTimer();
	if (state == SZSNN_TIMER_RUN) return state;
	return SZSNN_TIMER_ERROR;
}
SzsnnTimer::TimerState SzsnnTimer::stopTimer()
{
	using namespace boost::posix_time;
	if (state != SZSNN_TIMER_RUN && state != SZSNN_TIMER_PAUSE) return SZSNN_TIMER_ERROR;
	stopped = ptime(microsec_clock::local_time());
	state = SZSNN_TIMER_STOP;
	return state;
}

SzsnnTimer::TimerState SzsnnTimer::step()
{
	if (state != SZSNN_TIMER_RUN) return SZSNN_TIMER_ERROR;
	cycle++;
	return state;
}

unsigned long SzsnnTimer::getRealTime() const
{
	if(state == SZSNN_TIMER_STOP)
	{
		return elapsedSum + elapsedFrom(resumed, stopped);
	}
	if(state == SZSNN_TIMER_RUN)
	{
		return elapsedSum + elapsedFrom(resumed);
	}
	if(state == SZSNN_TIMER_PAUSE)
	{
		return elapsedSum;
	}
	return 0;
}

unsigned long SzsnnTimer::getRealFullTime() const
{
	if(state == SZSNN_TIMER_STOP)
	{
		return elapsedFrom(started, stopped);
	}
	if(state == SZSNN_TIMER_RUN || state == SZSNN_TIMER_PAUSE)
	{
		return elapsedFrom(started);
	}
	return 0;
}

unsigned long SzsnnTimer::getSimualtionTime() const
{
	if(state == SZSNN_TIMER_ERROR || state == SZSNN_TIMER_RESET) return 0;
	return cycle;
}


unsigned long SzsnnTimer::elapsedFrom(const boost::posix_time::ptime& start) const
{
	using namespace boost::posix_time;

	ptime now = ptime(microsec_clock::local_time());
	return elapsedFrom(start, now);
}

unsigned long SzsnnTimer::elapsedFrom(
	const boost::posix_time::ptime& startTime, const boost::posix_time::ptime& stopTime) const
{
	using namespace boost::posix_time;
	time_duration elapsed = stopTime - startTime;
	return boost::numeric_cast<unsigned long, time_duration::tick_type>(elapsed.total_milliseconds());
}

}