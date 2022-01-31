#pragma once
#include "./TimeDomainSmoother.hpp"


#include <p2.h>


struct PSquaresSmoother: public PerPointTimeDomainSmoother{
	PSquaresSmoother(size_t pixels);
	virtual ~PSquaresSmoother();
};
