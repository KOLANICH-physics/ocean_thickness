#include <FAcq/preprocess/REQSketchSmoother.hpp>

#include <DataSketches/req_sketch.hpp>

static constexpr double medianQuantile = 0.5;

struct REQSinglePointSketch: public SinglePointSketch{
	using SketchT = datasketches::req_sketch<SignalT::value_type>;
	SketchT tds;

	REQSinglePointSketch(uint16_t k): tds(k){}

	virtual void add(SignalT::value_type value){
		tds.update(value);
	}
	virtual double median(){
		return tds.get_quantile(medianQuantile);
	}
	virtual void reset(){
		tds = SketchT(tds.get_k());
	}

	virtual ~REQSinglePointSketch() = default;
};

struct REQSketchSinglePointFactory: public SinglePointSketchFactory{
	uint16_t k;
	REQSketchSinglePointFactory(uint16_t k): SinglePointSketchFactory(){
		this->k = k;
	}
	virtual std::unique_ptr<SinglePointSketch> operator()() override {
		return std::make_unique<REQSinglePointSketch>(k);
	}
	virtual ~REQSketchSinglePointFactory() override = default;
};

REQSketchSmoother::REQSketchSmoother(size_t pixels, uint16_t k):
	PerPointTimeDomainSmoother(pixels, std::make_unique<REQSketchSinglePointFactory>(k)){
}
REQSketchSmoother::~REQSketchSmoother() = default;


