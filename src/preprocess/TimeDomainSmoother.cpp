#include <FAcq/preprocess/TimeDomainSmoother.hpp>

ITimeDomainSmoother::ITimeDomainSmoother() = default;
ITimeDomainSmoother::~ITimeDomainSmoother() = default;

TimeDomainSmoother::TimeDomainSmoother(size_t pixels): ITimeDomainSmoother(), smoothed(pixels){}
TimeDomainSmoother::~TimeDomainSmoother() = default;
SignalT& TimeDomainSmoother::getSmoothed(){
	return smoothed;
}

SinglePointSketch::~SinglePointSketch() = default;

SinglePointSketchFactory::~SinglePointSketchFactory() = default;

PerPointTimeDomainSmoother::~PerPointTimeDomainSmoother() = default;

void PerPointTimeDomainSmoother::reset(){
	resetActivated = true;
}

PerPointTimeDomainSmoother::PerPointTimeDomainSmoother(size_t pixels, std::unique_ptr<SinglePointSketchFactory> factory): TimeDomainSmoother(pixels){
	sketches.reserve(pixels);
	for(auto i = 0; i < pixels; ++i){
		sketches.emplace_back((*factory)());
	}
}

void PerPointTimeDomainSmoother::smooth(SignalT &signal){
	size_t i = 0;
	for(auto px: signal){
		if(resetActivated){
			sketches[i]->reset();
		}
		sketches[i]->add(px);
		smoothed[i] = sketches[i]->median();
		++i;
	}
	if(resetActivated){
		resetActivated = false;
	}
}

