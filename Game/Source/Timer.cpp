// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#include "External\SDL\include\SDL_timer.h"

// L07: DONE 1: Fill Start(), Read(), ReadSec() methods
// they are simple, one line each!

Timer::Timer()
	: running(false)
	, startTime(0)
	, stopTime(0)
	, pausedAt(0)
	, resumedAt(0)
	, time(0)
	, state(RUNNING)
{
	Start();
}

void Timer::Start()
{
	if (state == STOPPED)
	{
		running = true;
		startTime = SDL_GetTicks();
		state = RUNNING;
	}
}

void Timer::Resume()
{
	if (state == PAUSED)
	{
		running = true;
		resumedAt = SDL_GetTicks();
		time += resumedAt - pausedAt;
		state = RUNNING;
	}
}

void Timer::Pause()
{
	if (state == RUNNING)
	{
		running = false;
		stopTime = SDL_GetTicks();
		pausedAt = SDL_GetTicks();
		state = PAUSED;
	}
}	

void Timer::Stop()
{
	if (state == RUNNING)
	{
		running = false;
		stopTime = SDL_GetTicks();
		state = STOPPED;
	}	
}

void Timer::Restart()
{
	running = false;
	startTime = 0;
	stopTime = 0;
	pausedAt = 0;
	resumedAt = 0;
	time = 0;
	state = STOPPED;
}

uint32 Timer::Read() const
{
	if (running)
	{
		return (SDL_GetTicks() - startTime - time);
	}
	else
	{
		return (stopTime - startTime - time);
	}
}

float Timer::ReadSec()
{
	if (running)
	{
		return (SDL_GetTicks() - startTime - time) / 1000.0f;
	}
	else
	{
		return (stopTime - startTime - time) / 1000.0f;
	}
}