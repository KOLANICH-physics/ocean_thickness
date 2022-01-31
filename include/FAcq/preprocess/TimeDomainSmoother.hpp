#pragma once
#include <FAcq/types.hpp>
#include <memory>

struct ITimeDomainSmoother{
	virtual void smooth(SignalT &signal) = 0;
	virtual void reset() = 0;
	virtual SignalT& getSmoothed() = 0;

	ITimeDomainSmoother();
	virtual ~ITimeDomainSmoother();
};

struct TimeDomainSmoother: public ITimeDomainSmoother{
	SignalT smoothed;

	TimeDomainSmoother(size_t pixels);
	virtual ~TimeDomainSmoother();

	virtual SignalT& getSmoothed();
};

struct SinglePointSketch{
	virtual void add(SignalT::value_type value) = 0;
	virtual double median() = 0;
	virtual void reset() = 0;

	virtual ~SinglePointSketch();
};

struct SinglePointSketchFactory{
	virtual std::unique_ptr<SinglePointSketch> operator()() = 0;
	virtual ~SinglePointSketchFactory();
};

struct PerPointTimeDomainSmoother: public TimeDomainSmoother{
	std::vector<std::unique_ptr<SinglePointSketch>> sketches;
	bool resetActivated = false;

	virtual void reset();

	PerPointTimeDomainSmoother(size_t pixels, std::unique_ptr<SinglePointSketchFactory> factory);
	virtual ~PerPointTimeDomainSmoother();

	virtual void smooth(SignalT &signal);
};
