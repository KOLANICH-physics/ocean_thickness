#include <FAcq/preprocess/EWMASmoother.hpp>



EWMASmoother::EWMASmoother(size_t pixels, float alpha): TimeDomainSmoother(pixels), alpha(alpha){}

void EWMASmoother::reset(){
	resetActivated = true;
}

void EWMASmoother::smooth(SignalT &signal){
	size_t i = 0;
	for(auto px: signal){
		if(resetActivated){
			smoothed[i] = px;
		}
		smoothed[i] = px * (1. - alpha) + alpha * smoothed[i];
		++i;
	}
	if(resetActivated){
		resetActivated = false;
	}
}
