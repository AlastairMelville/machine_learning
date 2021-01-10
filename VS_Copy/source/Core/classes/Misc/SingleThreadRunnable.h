#pragma once

/**
 * Interface for ticking runnables when there's only one thread available and
 * multithreading is disabled.
 */
class FSingleThreadRunnable
{
public:
	virtual ~FSingleThreadRunnable() { }

	/* Tick function. */
	virtual void Tick() = 0;
};
