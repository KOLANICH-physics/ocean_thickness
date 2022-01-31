#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>

#include <FAcq/types.hpp>

struct ISpectrometerSharedSignal{
	SignalT *wavelengths = nullptr;
	SignalT *dst = nullptr;
	std::mutex allowWrite;
};

struct IAcquisitor{
	virtual void startAcquisition() = 0;
	virtual void stopAcquisition() = 0;
};

struct ISpectrometer: public IAcquisitor{
	ISpectrometer();
	virtual ~ISpectrometer();
	virtual void setExposition(double exp) = 0;
	virtual double getMinimumExposition() = 0;

	ISpectrometerSharedSignal *sig;
};


struct Spectrometer: public ISpectrometer{
	std::atomic<bool> acquisitionRunning;

	SignalT wavelengths;
	SignalT spectrum;

	std::thread acquisitionThread;

	ISpectrometerSharedSignal sigImpl;

	Spectrometer();
	virtual ~Spectrometer();

	virtual void acqFunc();
	virtual void acquireOneSpectrum(SignalT &spectrum) = 0;

	void setWavelengthsCount(size_t count);

	int debugLevel = 0;
	void debugPoint(SignalT &spectrum);

	virtual void startAcquisition();

	virtual void stopAcquisition();

protected:
	double exposition;
};
