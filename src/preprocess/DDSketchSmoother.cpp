#include <FAcq/preprocess/DDSketchSmoother.hpp>

#include <ddsketch/ddsketch.h>

static constexpr double medianQuantile = 0.5;

struct SinglePointDDSketch: public SinglePointSketch{
	ddsketch::DDSketch tds;
	double accuracy;

	SinglePointDDSketch(double accuracy): tds(accuracy), accuracy(accuracy){}

	virtual void add(SignalT::value_type value){
		tds.add(value);
	}
	virtual double median(){
		return tds.get_quantile_value(medianQuantile);
	}
	virtual void reset(){
		tds = ddsketch::DDSketch(accuracy);
	}

	virtual ~SinglePointDDSketch() = default;
};

struct DDSketchSinglePointFactory: public SinglePointSketchFactory{
	double accuracy;
	DDSketchSinglePointFactory(double accuracy): SinglePointSketchFactory(){
		this->accuracy = accuracy;
	}
	virtual std::unique_ptr<SinglePointSketch> operator()() override {
		return std::make_unique<SinglePointDDSketch>(accuracy);
	}
	virtual ~DDSketchSinglePointFactory() override = default;
};

DDSketchSmoother::DDSketchSmoother(size_t pixels, double compression):
	PerPointTimeDomainSmoother(pixels, std::make_unique<DDSketchSinglePointFactory>(compression)){
}
DDSketchSmoother::~DDSketchSmoother() = default;


