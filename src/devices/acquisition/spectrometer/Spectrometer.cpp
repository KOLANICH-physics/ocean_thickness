#include <FAcq/devices/acquisition/spectrometer/Spectrometer.hpp>

ISpectrometer::ISpectrometer() = default;
ISpectrometer::~ISpectrometer() = default;

Spectrometer::Spectrometer() {
	sig = &sigImpl;
	sigImpl.wavelengths = &wavelengths;
};
Spectrometer::~Spectrometer() = default;

void Spectrometer::setWavelengthsCount(size_t count){
	wavelengths.reserve(count);
	wavelengths.resize(count);

	spectrum.reserve(count);
	spectrum.resize(count);
}

void Spectrometer::acqFunc(){
	int error;

	while(acquisitionRunning){
		acquireOneSpectrum(spectrum);
		debugPoint(spectrum);

		{
			std::lock_guard<std::mutex> guard(sigImpl.allowWrite);
			std::copy(begin(spectrum), end(spectrum), begin(*sigImpl.dst));
		}
	}
}

void Spectrometer::debugPoint(SignalT &spectrum){
	if(debugLevel){
		for(int i = 0; i< spectrum.size(); ++i){
			std::cerr << spectrum[i] << " ";
		}
		std::cerr << std::endl;
		std::cerr << "Acquired" << std::endl;
	}
}

void Spectrometer::startAcquisition(){
	if(!acquisitionRunning){
		acquisitionRunning = true;
		acquisitionThread = std::thread(&Spectrometer::acqFunc, this);
	}
}

void Spectrometer::stopAcquisition(){
	if(acquisitionRunning){
		acquisitionRunning = false;
		acquisitionThread.join();
	}
}
