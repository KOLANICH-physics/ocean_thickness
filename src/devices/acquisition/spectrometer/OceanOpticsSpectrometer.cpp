#include <cstddef>
#include <cstring>
#include <stdexcept>

#include <SeaBreeze/api/SeaBreezeWrapper.h>
#include <SeaBreeze/api/seabreezeapi/SeaBreezeAPIConstants.h>
#include <SeaBreeze/api/seabreezeapi/SeaBreezeAPI.h>

#include <SeaBreeze/common/Log.h>
#include <SeaBreeze/vendors/OceanOptics/features/spectrometer/SpectrometerTriggerMode.h>

#include <FAcq/devices/acquisition/spectrometer/OceanOpticsSpectrometer.hpp>

struct DeviceFeaturesAcquisitor{
	typedef int (SeaBreezeAPI::*CountGetterFnT)(long deviceID, int *errorCode);
	typedef int (SeaBreezeAPI::*ArrayGetterFnT)(long deviceID, int *errorCode, long *buffer, unsigned int maxLength);

	CountGetterFnT countGetter;
	ArrayGetterFnT arrayGetter;

	DeviceFeaturesAcquisitor(CountGetterFnT countGetter, ArrayGetterFnT arrayGetter): countGetter(countGetter), arrayGetter(arrayGetter){}

	std::vector<long> operator()(SeaBreezeAPI& api, long deviceID){
		int err = 0;
		auto count = std::invoke(countGetter, api, deviceID, &err);
		std::vector<long> res(count);
		std::invoke(arrayGetter, api, deviceID, &err, res.data(), count);
		return res;
	}
};

template <typename ValueT>
struct ArrayAcquisitor{
	typedef int (SeaBreezeAPI::*ArrayGetterFnT)(long deviceID, long featureID, int *errorCode, ValueT *buffer, int maxLength);
	ArrayGetterFnT arrayGetter;

	ArrayAcquisitor(ArrayGetterFnT arrayGetter): arrayGetter(arrayGetter){}

	virtual size_t getCount(SeaBreezeAPI& api, long deviceID, long featureID) = 0;

	std::vector<ValueT> operator()(SeaBreezeAPI& api, long deviceID, long featureID){
		int err = 0;
		auto count = getCount(api, deviceID, featureID);
		std::vector<ValueT> res(count);
		std::invoke(arrayGetter, api, deviceID, featureID, &err, res.data(), count);
		return res;
	}
};

template <typename ValueT>
struct ArrayAcquisitorGetCount: public ArrayAcquisitor<ValueT>{
	typedef int (SeaBreezeAPI::*CountGetterFnT)(long deviceID, long featureID, int *errorCode);
	CountGetterFnT countGetter;

	using ArrayGetterFnT = typename ArrayAcquisitor<ValueT>::ArrayGetterFnT;

	ArrayAcquisitorGetCount(CountGetterFnT countGetter, ArrayGetterFnT arrayGetter): countGetter(countGetter), ArrayAcquisitor<ValueT>(arrayGetter){}

	virtual size_t getCount(SeaBreezeAPI& api, long deviceID, long featureID) override {
		int err = 0;
		return std::invoke(countGetter, api, deviceID, featureID, &err);
	}
};

template <typename ValueT>
struct ArrayAcquisitorKnownCount: public ArrayAcquisitor<ValueT>{
	using ArrayGetterFnT = typename ArrayAcquisitor<ValueT>::ArrayGetterFnT;
	size_t knownCount;

	ArrayAcquisitorKnownCount(size_t knownCount, ArrayGetterFnT arrayGetter): knownCount(knownCount), ArrayAcquisitor<ValueT>(arrayGetter){}

	virtual size_t getCount(SeaBreezeAPI& api, long deviceID, long featureID) override {
		return knownCount;
	}
};

struct DeviceFeaturesAcquisitorAndPrinterAndSelector{
	DeviceFeaturesAcquisitor &acq;
	std::string name;

	DeviceFeaturesAcquisitorAndPrinterAndSelector(DeviceFeaturesAcquisitor &acq, std::string name): acq(acq), name(name){}

	long operator()(SeaBreezeAPI& api, long deviceID){
		std::vector<long> features = acq(api, deviceID);
		auto sz = features.size();
		std::cerr << features.size() << " " << name << "s";

		if(sz){
			std::cerr << ":" << std::endl;
		}

		long res = -1;
		for(auto f: features){
			std::cerr << f << " ";
			res = f;
		}
		std::cerr << std::endl;

		return res;
	}
};

DeviceFeaturesAcquisitor spectrAcq{&SeaBreezeAPI::getNumberOfSpectrometerFeatures, &SeaBreezeAPI::getSpectrometerFeatures};
DeviceFeaturesAcquisitor irradCalAcq{&SeaBreezeAPI::getNumberOfIrradCalFeatures, &SeaBreezeAPI::getIrradCalFeatures};
DeviceFeaturesAcquisitor nonlinAcq{&SeaBreezeAPI::getNumberOfNonlinearityCoeffsFeatures, &SeaBreezeAPI::getNonlinearityCoeffsFeatures};
DeviceFeaturesAcquisitor specProcAcq{&SeaBreezeAPI::getNumberOfSpectrumProcessingFeatures, &SeaBreezeAPI::getSpectrumProcessingFeatures};

DeviceFeaturesAcquisitor acqDelayAcq{&SeaBreezeAPI::getNumberOfAcquisitionDelayFeatures, &SeaBreezeAPI::getAcquisitionDelayFeatures};
DeviceFeaturesAcquisitor dataBufAcq{&SeaBreezeAPI::getNumberOfDataBufferFeatures, &SeaBreezeAPI::getDataBufferFeatures};
DeviceFeaturesAcquisitor fastBufAcq{&SeaBreezeAPI::getNumberOfFastBufferFeatures, &SeaBreezeAPI::getFastBufferFeatures};

DeviceFeaturesAcquisitor rawUsbAcq{&SeaBreezeAPI::getNumberOfRawUSBBusAccessFeatures, &SeaBreezeAPI::getRawUSBBusAccessFeatures};
DeviceFeaturesAcquisitor snAcq{&SeaBreezeAPI::getNumberOfSerialNumberFeatures, &SeaBreezeAPI::getSerialNumberFeatures};
DeviceFeaturesAcquisitor tecAcq{&SeaBreezeAPI::getNumberOfThermoElectricFeatures, &SeaBreezeAPI::getThermoElectricFeatures};
DeviceFeaturesAcquisitor eepromAcq{&SeaBreezeAPI::getNumberOfEEPROMFeatures, &SeaBreezeAPI::getEEPROMFeatures};
DeviceFeaturesAcquisitor lampAcq{&SeaBreezeAPI::getNumberOfLampFeatures, &SeaBreezeAPI::getLampFeatures};
DeviceFeaturesAcquisitor lightSourceAcq{&SeaBreezeAPI::getNumberOfLightSourceFeatures, &SeaBreezeAPI::getLightSourceFeatures};
DeviceFeaturesAcquisitor contStrobeAcq{&SeaBreezeAPI::getNumberOfContinuousStrobeFeatures, &SeaBreezeAPI::getContinuousStrobeFeatures};
DeviceFeaturesAcquisitor tempAcq{&SeaBreezeAPI::getNumberOfTemperatureFeatures, &SeaBreezeAPI::getTemperatureFeatures};
DeviceFeaturesAcquisitor revAcq{&SeaBreezeAPI::getNumberOfRevisionFeatures, &SeaBreezeAPI::getRevisionFeatures};
DeviceFeaturesAcquisitor oBenchAcq{&SeaBreezeAPI::getNumberOfOpticalBenchFeatures, &SeaBreezeAPI::getOpticalBenchFeatures};
DeviceFeaturesAcquisitor strayAcq{&SeaBreezeAPI::getNumberOfStrayLightCoeffsFeatures, &SeaBreezeAPI::getStrayLightCoeffsFeatures};
DeviceFeaturesAcquisitor binningAcq{&SeaBreezeAPI::getNumberOfPixelBinningFeatures, &SeaBreezeAPI::getPixelBinningFeatures};
DeviceFeaturesAcquisitor gpioAcq{&SeaBreezeAPI::getNumberOfGPIOFeatures, &SeaBreezeAPI::getGPIOFeatures};
DeviceFeaturesAcquisitor i2cAcq{&SeaBreezeAPI::getNumberOfI2CMasterFeatures, &SeaBreezeAPI::getI2CMasterFeatures};


OceanOpticsSpectrometerAddr OceanOpticsSpectrometer::discover(){
	auto &sba = *SeaBreezeAPI::getInstance();
	//auto modelMaxLen = sba->getModelStringMaximumLength();

	printf("Probing for devices...\n");
	sba.probeDevices();

	printf("Getting device count...\n");
	auto count = sba.getNumberOfDeviceIDs();
	printf("Device count is %d\n", count);

	OceanOpticsSpectrometerAddr res{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	if(count > 0) {
		std::vector<long> device_ids(count);
		count = sba.getDeviceIDs(device_ids.data(), count);
		std::cerr << count << " devices" << std::endl;

		std::string name;
		int error;
		long featureID;

		std::ios cerStateBackup(nullptr);
		cerStateBackup.copyfmt(std::cerr);
		std::cerr << std::hex;

		for(auto i = 0; i < count; i++) {
			auto sIdx = device_ids[i];
			std::cerr << sIdx << ": ";

			name.reserve(80);
			name.resize(80);
			sba.getDeviceType(sIdx, &error, name.data(), name.size() - 1);
			std::cerr << name << " ";
			std::cerr <<
				"[" << uint16_t(sba.getDeviceEndpoint(sIdx, &error, kEndpointTypePrimaryOut)) <<
				" " << uint16_t(sba.getDeviceEndpoint(sIdx, &error, kEndpointTypePrimaryIn)) <<
				" " << uint16_t(sba.getDeviceEndpoint(sIdx, &error, kEndpointTypeSecondaryOut)) <<
				" " << uint16_t(sba.getDeviceEndpoint(sIdx, &error, kEndpointTypeSecondaryIn)) <<
				" " << uint16_t(sba.getDeviceEndpoint(sIdx, &error, kEndpointTypeSecondaryIn2)) <<
			"]" << std::endl;
			sba.openDevice(sIdx, &error);

			res.sn = (DeviceFeaturesAcquisitorAndPrinterAndSelector{snAcq, "s/n"})(sba, sIdx);
			if(res.sn > -1){
				std::string sn;
				auto snSize = sba.getSerialNumberMaximumLength(sIdx, res.sn, &error);
				sn.reserve(snSize);
				sn.resize(snSize);
				sba.getSerialNumber(sIdx, res.sn, &error, sn.data(), snSize);
				std::cerr << "S/N: " << sn << std::endl;
			}

			res.sptr = (DeviceFeaturesAcquisitorAndPrinterAndSelector{spectrAcq, "subdevice"})(sba, sIdx);
			res.irradCal = (DeviceFeaturesAcquisitorAndPrinterAndSelector{irradCalAcq, "irrad cal"})(sba, sIdx);
			res.nonlinCorrection = (DeviceFeaturesAcquisitorAndPrinterAndSelector{nonlinAcq, "nonlin correction"})(sba, sIdx);
			res.usb = (DeviceFeaturesAcquisitorAndPrinterAndSelector{rawUsbAcq, "raw usb feature"})(sba, sIdx);
			res.eeprom = (DeviceFeaturesAcquisitorAndPrinterAndSelector{eepromAcq, "EEPROM"})(sba, sIdx);
			res.strobe = (DeviceFeaturesAcquisitorAndPrinterAndSelector{lampAcq, "lamp strobe"})(sba, sIdx);
			res.contStrobe = (DeviceFeaturesAcquisitorAndPrinterAndSelector{contStrobeAcq, "cont strobe"})(sba, sIdx);
			res.stray = (DeviceFeaturesAcquisitorAndPrinterAndSelector{strayAcq, "stray"})(sba, sIdx);

			(DeviceFeaturesAcquisitorAndPrinterAndSelector{specProcAcq, "processor"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{acqDelayAcq, "delay info retriever"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{dataBufAcq, "buf acq"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{fastBufAcq, "buf fast acq"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{tecAcq, "TEC"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{lightSourceAcq, "light source"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{tempAcq, "thermometer"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{revAcq, "rev info"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{oBenchAcq, "opt bench"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{binningAcq, "binning"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{gpioAcq, "gpio"})(sba, sIdx);
			(DeviceFeaturesAcquisitorAndPrinterAndSelector{i2cAcq, "i2c"})(sba, sIdx);


			sba.closeDevice(sIdx, &error);
			res.id = sIdx;
		}

		std::cerr.copyfmt(cerStateBackup);
		return res;
	}
}

/*class SeaBreezeError: public std::runtime_error{

};*/

OceanOpticsSpectrometer::OceanOpticsSpectrometer(OceanOpticsSpectrometerAddr addr): Spectrometer(), addr(addr){
	auto &sba = *SeaBreezeAPI::getInstance();
	sbaOpaque = &sba;

	int error;

	sba.openDevice(addr.id, &error);
	if(error){
		throw std::runtime_error(std::string{"Error opening device: "} + sbapi_get_error_string(error));
	}

	auto spectrumSize = sba.spectrometerGetFormattedSpectrumLength(addr.id, addr.sptr, &error);
	setWavelengthsCount(spectrumSize);
	sba.spectrometerGetWavelengths(addr.id, addr.sptr, &error, wavelengths.data(), spectrumSize);

	stray = (ArrayAcquisitorKnownCount<double>(spectrumSize, &SeaBreezeAPI::strayLightCoeffsGet))(sba, addr.id, addr.stray);
	electricDark = (ArrayAcquisitorGetCount<int>(&SeaBreezeAPI::spectrometerGetElectricDarkPixelCount, &SeaBreezeAPI::spectrometerGetElectricDarkPixelIndices))(sba, addr.id, addr.sptr);
	nonlinearity = (ArrayAcquisitorKnownCount<double>(spectrumSize, &SeaBreezeAPI::nonlinearityCoeffsGet))(sba, addr.id, addr.sptr);

	//seabreeze_set_trigger_mode(sIdx, &error, );
	//seabreeze_read_irrad_calibration(int index, int * error_code, float *  	buffer, int  	buffer_length);
}

OceanOpticsSpectrometer::~OceanOpticsSpectrometer(){
	/// TODO: WARNING!!!! We need to close only (device, feature), not whole device
	auto &sba = *reinterpret_cast<SeaBreezeAPI*>(sbaOpaque);
	int err; // SEABREEZE_ERROR
	sba.closeDevice(addr.id, &err);
	sba.shutdown();
}

void OceanOpticsSpectrometer::setExposition(double exp) {
	auto &sba = *reinterpret_cast<SeaBreezeAPI*>(sbaOpaque);

	int error;
	if(exp != exposition){
		sba.spectrometerSetIntegrationTimeMicros(addr.id, addr.sptr, &error, exp);
		//sba.setIntegrationTimeMicrosec
		exposition = exp;
	}
}

double OceanOpticsSpectrometer::getMinimumExposition() {
	int error;
	auto &sba = *reinterpret_cast<SeaBreezeAPI*>(sbaOpaque);
	return sba.spectrometerGetMinimumIntegrationTimeMicros(addr.id, addr.sptr, &error);
}

void OceanOpticsSpectrometer::acquireOneSpectrum(SignalT &spectrum) {
	auto &sba = *reinterpret_cast<SeaBreezeAPI*>(sbaOpaque);

	int error = 0;
	sba.spectrometerGetFormattedSpectrum(addr.id, addr.sptr, &error, spectrum.data(), spectrum.size());
	if(error){
		std::cerr << "Acq error " << error << std::endl;
	}

	for(size_t i=0; i < spectrum.size(); ++i){
		spectrum[i] /= exposition;
	}

	//seabreeze_fast_buffer_spectrum_request(sIdx, &error, spectrum.data(), spectrum.size(), spectrum.size());
}
