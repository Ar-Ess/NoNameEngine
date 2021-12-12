#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"

enum timerState
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
	float ReadSec();

	bool running;
	uint32 startTime;
	uint32 stopTime;
	uint32 pausedAt;
	uint32 resumedAt;
	uint32 time;

	timerState state;
private:
	
};

#endif //__TIMER_H__
