#include <FAcq/preprocess/KLLSketchSmoother.hpp>

#include <DataSketches/kll_sketch.hpp>

static constexpr double medianQuantile = 0.5;

struct KLLSinglePointSketch: public SinglePointSketch{
	using SketchT = datasketches::kll_sketch<SignalT::value_type>;
	SketchT tds;

	KLLSinglePointSketch(uint16_t k): tds(k){}

	virtual void add(SignalT::value_type value){
		tds.update(value);
	}
	virtual double median(){
		return tds.get_quantile(medianQuantile);
	}
	virtual void reset(){
		tds = SketchT(tds.get_k());
	}

	virtual ~KLLSinglePointSketch() = default;
};

struct KLLSketchSinglePointFactory: public SinglePointSketchFactory{
	uint16_t k;
	KLLSketchSinglePointFactory(uint16_t k): SinglePointSketchFactory(){
		this->k = k;
	}
	virtual std::unique_ptr<SinglePointSketch> operator()() override {
		return std::make_unique<KLLSinglePointSketch>(k);
	}
	virtual ~KLLSketchSinglePointFactory() override = default;
};

KLLSketchSmoother::KLLSketchSmoother(size_t pixels, uint16_t k):
	PerPointTimeDomainSmoother(pixels, std::make_unique<KLLSketchSinglePointFactory>(k)){
}
KLLSketchSmoother::~KLLSketchSmoother() = default;


