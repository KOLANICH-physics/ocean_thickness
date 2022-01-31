#pragma once

#include <SeaBreeze/api/SeaBreezeWrapper.h>
#include <SeaBreeze/api/seabreezeapi/SeaBreezeAPIConstants.h>
#include <SeaBreeze/common/Log.h>

#include "Spectrometer.hpp"

struct OceanOpticsSpectrometerAddr{
	long id,
	sptr,
	irradCal,
	nonlinCorrection,
	sn,
	usb,
	eeprom,
	strobe,
	contStrobe,
	stray;
};


struct OceanOpticsSpectrometer: public Spectrometer{
	OceanOpticsSpectrometerAddr addr;
	void *sbaOpaque;

	std::vector<double> stray;
	std::vector<int> electricDark;
	std::vector<double> nonlinearity;

	OceanOpticsSpectrometer(OceanOpticsSpectrometerAddr addr);

	static OceanOpticsSpectrometerAddr discover();

	virtual ~OceanOpticsSpectrometer();

	virtual void setExposition(double exp) override;
	virtual double getMinimumExposition() override;

	virtual void acquireOneSpectrum(SignalT &spectrum) override;
};
