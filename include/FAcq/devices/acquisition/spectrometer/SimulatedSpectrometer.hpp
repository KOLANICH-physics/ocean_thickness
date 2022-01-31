#pragma once
#include <random>

#include "Spectrometer.hpp"

struct SimulatedSpectrometer: public Spectrometer{

	std::default_random_engine generator;
	std::normal_distribution<double> addNoise;
	std::normal_distribution<double> multNoise;

	SimulatedSpectrometer(uint32_t spectrumSize, double additiveNoise=4, double multNoise=0.01);
	virtual ~SimulatedSpectrometer();

	virtual void setExposition(double exp);

	virtual void acquireOneSpectrum(SignalT &spectrum);
};
