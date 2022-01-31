#include <FAcq/preprocess/DummySmoother.hpp>

DummySmoother::DummySmoother(): ITimeDomainSmoother(), smoothed(nullptr){}

void DummySmoother::reset(){}

void DummySmoother::smooth(SignalT &signal){
	smoothed = &signal;
}

SignalT& DummySmoother::getSmoothed(){
	return *smoothed;
}
