#pragma once
#include "./TimeDomainSmoother.hpp"

#include <ddsketch/ddsketch.h>


struct DDSketchSmoother: public PerPointTimeDomainSmoother{
	DDSketchSmoother(size_t pixels, double accuracy=0.01);
	virtual ~DDSketchSmoother();
};
