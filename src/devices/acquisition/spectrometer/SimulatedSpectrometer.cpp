#include <FAcq/devices/acquisition/spectrometer/SimulatedSpectrometer.hpp>

SimulatedSpectrometer::SimulatedSpectrometer(uint32_t spectrumSize, double additiveNoise, double multNoise): addNoise(0.0, additiveNoise), multNoise(1.0, multNoise){
	wavelengths.reserve(spectrumSize);
	wavelengths.resize(spectrumSize);
}

SimulatedSpectrometer::~SimulatedSpectrometer() = default;

void SimulatedSpectrometer::setExposition(double exp){
	exposition = exp;
}

void SimulatedSpectrometer::acquireOneSpectrum(SignalT &spectrum) {
	auto A = 1000.f * multNoise(generator);
	auto std = 100;
	for(int i = 0; i< spectrum.size(); ++i){
		auto d = (i - 600);
		spectrum[i] = A * exp(- double(d * d) / (std * std)) + addNoise(generator);
	}
	usleep(exposition);
}
