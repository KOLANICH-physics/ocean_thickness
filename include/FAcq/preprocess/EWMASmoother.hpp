#pragma once
#include "./TimeDomainSmoother.hpp"


struct EWMASmoother: public TimeDomainSmoother{
	bool resetActivated = false;

	float alpha;

	EWMASmoother(size_t pixels, float alpha = 0.8);

	virtual void reset();

	virtual void smooth(SignalT &signal);
};
