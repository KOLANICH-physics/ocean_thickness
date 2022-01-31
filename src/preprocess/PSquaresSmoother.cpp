#include <FAcq/preprocess/PSquaresSmoother.hpp>

#include <p2.h>

static constexpr double medianQuantile = 0.5;


struct PSquaresSinglePointSketch: public SinglePointSketch{
	p2_t psq{0.5};

	PSquaresSinglePointSketch(){}

	virtual void add(SignalT::value_type value){
		psq.add(value);
	}
	virtual double median(){
		return psq.result();
	}
	virtual void reset(){
		psq = p2_t{medianQuantile};
	}

	virtual ~PSquaresSinglePointSketch() = default;
};

struct PSquaresSinglePointSketchFactory: public SinglePointSketchFactory{
	PSquaresSinglePointSketchFactory(): SinglePointSketchFactory(){}
	virtual std::unique_ptr<SinglePointSketch> operator()() override {
		return std::make_unique<PSquaresSinglePointSketch>();
	}
	virtual ~PSquaresSinglePointSketchFactory() override = default;
};

PSquaresSmoother::PSquaresSmoother(size_t pixels):
	PerPointTimeDomainSmoother(pixels, std::make_unique<PSquaresSinglePointSketchFactory>()){
}
PSquaresSmoother::~PSquaresSmoother() = default;

