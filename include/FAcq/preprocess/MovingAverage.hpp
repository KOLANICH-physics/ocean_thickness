#pragma once
#include "./TimeDomainSmoother.hpp"


struct MovingAverage: public TimeDomainSmoother{
	SignalT delayed;
	int count;
	uint16_t meanAccumulated;

	MovingAverage(size_t pixels, int count);

	virtual void reset();

	void setCount(uint16_t count);

	virtual void smooth(SignalT &signal);
};

