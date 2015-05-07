#ifndef SZSNNTIMER_HPP_
#define SZSNNTIMER_HPP_
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace szsnn {
class SzsnnTimer
{
public:
	typedef enum 
	{
		SZSNN_TIMER_RESET = 0,
		SZSNN_TIMER_STOP,
		SZSNN_TIMER_RUN,
		SZSNN_TIMER_PAUSE,
		SZSNN_TIMER_ERROR,
	} TimerState;


	SzsnnTimer():state(SZSNN_TIMER_RESET) {resetTimer();};
	TimerState step();
	unsigned long getRealTime() const;
	unsigned long getRealFullTime() const;
	unsigned long getSimualtionTime() const;
	TimerState getState() const {return state;};
	bool isRunning() const {return state == SZSNN_TIMER_RUN; };

	TimerState resetTimer();
	TimerState startTimer();
	TimerState pauseTimer();
	TimerState resumeTimer();
	TimerState continueTimer();
	TimerState stopTimer();


private:

	unsigned long elapsedFrom(const boost::posix_time::ptime& timer) const;
	unsigned long elapsedFrom(const boost::posix_time::ptime& startTime,
								const boost::posix_time::ptime& stopTime) const;
	TimerState state;
	unsigned long cycle;
	unsigned long elapsedSum;
	boost::posix_time::ptime started;
	boost::posix_time::ptime resumed;
	boost::posix_time::ptime stopped;

	SzsnnTimer(const SzsnnTimer&);
	SzsnnTimer& operator=(const SzsnnTimer&);

};

} // namespace szsnn
#endif