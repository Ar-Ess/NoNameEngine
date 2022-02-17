#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"

enum TimerState
{
	RUNNING,
	PAUSED,
	STOPPED,
};

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Resume();
	void Stop();
	void Pause();
	void Restart();

	uint32 Read() const;
	float ReadSec() const;
	uint32 RealRead() const;
	float RealReadSec() const;

	TimerState GetTimerState() const
	{
		return state;
	}

private:
	uint32 initialTime;
	uint32 finalTime;
	uint32 time;

	TimerState state;
};

#endif //__TIMER_H__
