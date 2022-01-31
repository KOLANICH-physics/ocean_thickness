#include <FAcq/preprocess/MovingAverage.hpp>

MovingAverage::MovingAverage(size_t pixels, int count): TimeDomainSmoother(pixels), delayed(pixels), count(count), meanAccumulated(0){

}

void MovingAverage::reset(){
	meanAccumulated = 0;
}

void MovingAverage::setCount(uint16_t count){
	if(this->count != count){
		reset();
		for(size_t i = 0; i< smoothed.size(); ++i){
			smoothed[i] = 0;
		}
	}
}

void MovingAverage::smooth(SignalT &signal){
	size_t i = 0;

	if(meanAccumulated >= count){
		for(auto px: signal){
			smoothed[i] += px / count - delayed[i];
			++i;
		}
		std::transform(begin(signal), end(signal), begin(delayed), [this](auto x){ return x / this->count;});
	} else {
		++meanAccumulated;
		for(auto px: signal){
			smoothed[i] += px / count;
			++i;
		}
	}
}

