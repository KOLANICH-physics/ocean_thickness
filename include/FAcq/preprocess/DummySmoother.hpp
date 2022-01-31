#pragma once
#include "./TimeDomainSmoother.hpp"


struct DummySmoother: public ITimeDomainSmoother{
	SignalT *smoothed;

	DummySmoother();

	virtual void reset();

	virtual void smooth(SignalT &signal);

	virtual SignalT& getSmoothed();
};
