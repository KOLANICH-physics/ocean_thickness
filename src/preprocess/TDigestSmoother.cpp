#include <FAcq/preprocess/TDigestSmoother.hpp>

#include <tdigest2/TDigest.h>

struct TDigestSinglePointSketch: public SinglePointSketch{
	tdigest::TDigest tds;

	TDigestSinglePointSketch(double compression): tds(compression){}

	virtual void add(SignalT::value_type value){
		tds.add(value);
	}
	virtual double median(){
		return tds.quantile(0.5);
	}
	virtual void reset(){
		auto comp = tds.compression();
		tds = tdigest::TDigest{comp};
	}

	virtual ~TDigestSinglePointSketch() = default;
};

struct TDigestSketchFactory: public SinglePointSketchFactory{
	double compression;
	TDigestSketchFactory(double compression): SinglePointSketchFactory(){
		this->compression = compression;
	}
	virtual std::unique_ptr<SinglePointSketch> operator()() override {
		return std::make_unique<TDigestSinglePointSketch>(compression);
	}
	virtual ~TDigestSketchFactory() override = default;
};

TDigestSmoother::TDigestSmoother(size_t pixels, double compression):
	PerPointTimeDomainSmoother(pixels, std::make_unique<TDigestSketchFactory>(compression)){
}
TDigestSmoother::~TDigestSmoother() = default;
