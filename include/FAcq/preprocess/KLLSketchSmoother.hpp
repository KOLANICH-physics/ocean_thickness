#pragma once
#include "./TimeDomainSmoother.hpp"

struct KLLSketchSmoother: public PerPointTimeDomainSmoother{
	KLLSketchSmoother(size_t pixels, uint16_t k);
	virtual ~KLLSketchSmoother();
};
