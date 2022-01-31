#pragma once
#include "./TimeDomainSmoother.hpp"


#include <tdigest2/TDigest.h>


struct TDigestSmoother: public PerPointTimeDomainSmoother{
	TDigestSmoother(size_t pixels, double compression);
	~TDigestSmoother();
};
