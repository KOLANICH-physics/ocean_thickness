#pragma once
#include "./TimeDomainSmoother.hpp"

struct REQSketchSmoother: public PerPointTimeDomainSmoother{
	REQSketchSmoother(size_t pixels, uint16_t k);
	virtual ~REQSketchSmoother();
};
