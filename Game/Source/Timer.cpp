
#include "Timer.h"
#include "External\SDL\include\SDL_timer.h"
#include "Globals.h"

Timer::Timer() : initialTime(0), finalTime(0), time(0), state(STOPPED)
{
}

void Timer::Start()
{
	if (state == STOPPED)
	{
		Restart();
		initialTime = SDL_GetTicks();
		state = RUNNING;
		LOG("Time: %f", ReadSec());
	}
}

void Timer::Resume()
{
	if (state == PAUSED)
	{
		initialTime = SDL_GetTicks();
		state = RUNNING;
		LOG("Time: %f", ReadSec());
	}
}

void Timer::Pause()
{
	if (state == RUNNING)
	{
		finalTime = SDL_GetTicks();
		time += finalTime - initialTime;
		state = PAUSED;
		LOG("Time: %f", ReadSec());
	}
}	

void Timer::Stop()
{
	if (state == RUNNING)
	{
		finalTime = SDL_GetTicks();
		time += finalTime - initialTime;
		LOG("Time: %f", ReadSec());
	}

	state = STOPPED;
}

void Timer::Restart()
{
	initialTime = 0;
	finalTime = 0;
	time = 0;
	state = STOPPED;
}

uint32 Timer::Read() const
{
	if (state == RUNNING)
	{
		return (SDL_GetTicks() - initialTime + time);
	}
	else
	{
		return time;
	}
}

float Timer::ReadSec() const
{
	if (state == RUNNING)
	{
		return (SDL_GetTicks() - initialTime + time) / 1000.0f;
	}
	else
	{
		return time / 1000.0f;
	}
}

uint32 Timer::RealRead() const
{
	return SDL_GetPerformanceCounter();
}

float Timer::RealReadSec() const
{
	return SDL_GetTicks() / 1000.0f;
}