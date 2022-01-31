// Based on: https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <random>
#include <filesystem>

#include <unistd.h>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <implot.h>

#include <SeaBreeze/api/SeaBreezeWrapper.h>
#include <SeaBreeze/api/seabreezeapi/SeaBreezeAPIConstants.h>
#include <SeaBreeze/common/Log.h>


#include <FAcq/devices/acquisition/spectrometer/OceanOpticsSpectrometer.hpp>
#include <FAcq/devices/acquisition/spectrometer/SimulatedSpectrometer.hpp>
#include <FAcq/SmoothingMode.hpp>

#include <FAcq/preprocess/DummySmoother.hpp>
#include <FAcq/preprocess/TimeDomainSmoother.hpp>
#include <FAcq/preprocess/EWMASmoother.hpp>
#include <FAcq/preprocess/MovingAverage.hpp>
#include <FAcq/preprocess/TDigestSmoother.hpp>
#include <FAcq/preprocess/PSquaresSmoother.hpp>
#include <FAcq/preprocess/DDSketchSmoother.hpp>
#include <FAcq/preprocess/KLLSketchSmoother.hpp>
#include <FAcq/preprocess/REQSketchSmoother.hpp>

#include "GUI.hpp"

#include <ImGuiFileDialog.h>

struct SmoothersBank: public ITimeDomainSmoother{
	DummySmoother dummy;
	PSquaresSmoother psqS;
	TDigestSmoother tdS;
	DDSketchSmoother ddsS;
	KLLSketchSmoother kklS;
	REQSketchSmoother reqS;

	SmoothingMode smoothingMode = SmoothingMode::none;

	SmoothersBank(size_t pixels): ITimeDomainSmoother(), dummy(), psqS(pixels), kklS(pixels, 200), reqS(pixels, 200), ddsS(pixels, 0.01), tdS(pixels, 100){}

	ITimeDomainSmoother& getSmoother(){
		switch(smoothingMode){
			case SmoothingMode::none:
				return dummy;
			break;
			case SmoothingMode::ddSketch:
				return ddsS;
			break;
			case SmoothingMode::psq:
				return psqS;
			break;
			case SmoothingMode::kll:
				return kklS;
			break;
			case SmoothingMode::req:
				return reqS;
			break;
			case SmoothingMode::tdigest:
				return tdS;
			break;

			default:
				std::cerr << "Invalid smoothing mode " << (int) smoothingMode << std::endl;
		}
		return dummy;
	}

	virtual void smooth(SignalT &signal) override {
		getSmoother().smooth(signal);
	}

	virtual void reset() override {
		getSmoother().reset();
	}

	virtual SignalT& getSmoothed() override {
		return getSmoother().getSmoothed();
	}
};

struct DevicesBank: public ISpectrometer{
	bool simulation = true;
	bool prevSimulationResult = simulation;

	OceanOpticsSpectrometer ooDvc;

	DevicesBank(OceanOpticsSpectrometerAddr sAddr): ISpectrometer(), ooDvc(sAddr){
		sig = getSpectrometer().sig;
	}
	virtual ~DevicesBank() = default;

	ISpectrometer& getSpectrometer(){
		return ooDvc;
	}

	virtual void setExposition(double exp){
		getSpectrometer().setExposition(exp);
	}

	virtual double getMinimumExposition(){
		return getSpectrometer().getMinimumExposition();
	}

	virtual void startAcquisition(){
		getSpectrometer().startAcquisition();
	}

	virtual void stopAcquisition(){
		getSpectrometer().stopAcquisition();
	}
};

struct OurGUI: public GUI{
	OceanOpticsSpectrometer device;
	bool resetSmoothingClicked = false;
	bool captureDarkButtonClicked = false;
	bool captureLightButtonClicked = false;
	bool maxSelectionMode = false;
	bool fixElectricDark = false;
	bool subtractDark = false;

	bool doAcquisition = false;

	SmoothersBank smoothers;
	EWMASmoother ewmaS;
	uint32_t currentAveCount;

	static constexpr int minAveraging = 1;
	static constexpr int maxAveraging = 1000;

	static constexpr float defaultIRRSmooth = 0.99;

	float minIntTime;
	float currentIntTime;
	float maxIntTime;

	SignalT spectrum;
	SignalT dark;
	SignalT * spectrumToSelectMaxId;
	int maxEltIdx = 0;

	std::filesystem::path darkSpectrumPath;
	std::filesystem::path lightSpectrumPath;
	std::filesystem::path pnruPath;


	OurGUI(OceanOpticsSpectrometerAddr sAddr): GUI(), device(sAddr), smoothers(device.sig->wavelengths->size()), ewmaS(device.sig->wavelengths->size(), defaultIRRSmooth){

		device.debugLevel = 0;

		unsigned pixels = device.sig->wavelengths->size();
		std::cerr << "Pixels: " << pixels << std::endl;

		spectrum.reserve(pixels);
		spectrum.resize(pixels);
		device.sig->dst = &spectrum;

		dark.reserve(pixels);
		dark.resize(pixels);


		minIntTime = device.getMinimumExposition();
		std::cerr << "minIntTime: " << minIntTime << std::endl;
		maxIntTime = minIntTime * 30000;

		currentIntTime = minIntTime;

		currentAveCount = minAveraging;

		maxEltIdx = 0;
		spectrumToSelectMaxId = &spectrum;

		ewmaS.smooth(spectrum);
		smoothers.smooth(ewmaS.getSmoothed());
	}

	void checkAcquisition(){
		if(doAcquisition){
			device.startAcquisition();
		} else {
			device.stopAcquisition();
		}
	}

	virtual void checkSettings(){
		device.setExposition(currentIntTime);
		if(resetSmoothingClicked){
			ewmaS.reset();
			smoothers.getSmoother().reset();
		}
		checkAcquisition();
	}

	virtual void drawSettingsWindow(){
		ImGui::Begin("Settings");
		{
			ImGui::Checkbox("Do acquisition", &doAcquisition);
			ImGui::Checkbox("Subtract dark", &subtractDark);
			//ImGui::Checkbox("Don't use real spectrum, use simulated one", &simulation);

			ImGui::SliderFloat("exp", &currentIntTime, minIntTime, maxIntTime);
			//ImGui::Checkbox("Fix electric dark", &fixElectricDark);

			ImGui::SliderFloat("IRR alpha", &ewmaS.alpha, 0.8, 0.99);

			ImGui::Checkbox("Select max from immed", &maxSelectionMode);

			ImGui::RadioButton("none", reinterpret_cast<int*>(&smoothers.smoothingMode), static_cast<int>(SmoothingMode::none));
			ImGui::RadioButton("t-digest", reinterpret_cast<int*>(&smoothers.smoothingMode), static_cast<int>(SmoothingMode::tdigest));
			ImGui::RadioButton("p^2", reinterpret_cast<int*>(&smoothers.smoothingMode), static_cast<int>(SmoothingMode::psq));
			ImGui::RadioButton("KLL", reinterpret_cast<int*>(&smoothers.smoothingMode), static_cast<int>(SmoothingMode::kll));
			ImGui::RadioButton("REQ", reinterpret_cast<int*>(&smoothers.smoothingMode), static_cast<int>(SmoothingMode::req));

			/*if(smoothers.smoothingMode == SmoothingMode::mean){
				ImGui::SliderInt("averaging", &smoothers.mavS.count, minAveraging, maxAveraging);
			}*/

			resetSmoothingClicked = ImGui::Button("Reset smoothing");
			captureDarkButtonClicked = ImGui::Button("Dark");
			ImGui::SameLine();
			captureLightButtonClicked = ImGui::Button("Light");

			if(ImGui::Button("Dark spectrum file")){
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Dark Spectrum File", ".tsv", ".");
			}

			if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")){
				if (ImGuiFileDialog::Instance()->IsOk()){
					darkSpectrumPath = ImGuiFileDialog::Instance()->GetFilePathName();
					std::cerr << "ImGuiFileDialog::Instance()->GetFilePathName()" << ImGuiFileDialog::Instance()->GetFilePathName() << std::endl;
					std::cerr << "ImGuiFileDialog::Instance()->GetCurrentPath()" << ImGuiFileDialog::Instance()->GetCurrentPath() << std::endl;
				}
				ImGuiFileDialog::Instance()->Close();
			}

			ImGui::Button("Save");
		}
		ImGui::End();
	}

	static constexpr auto padding = 3;

	virtual void drawSpectrumWindow(){
		ImGui::Begin("Spectrum");
		if (ImPlot::BeginPlot("Spectrum")) {
			auto & smoothed = smoothers.getSmoothed();
			ImPlot::PlotLine("Smoothed", device.sig->wavelengths->data() + padding, smoothed.data() + padding, smoothed.size() - padding);

			if(device.sig->wavelengths->size()){
				auto maxW = (*device.sig->wavelengths)[maxEltIdx];
				auto maxI = smoothed[maxEltIdx];
				ImPlot::TagX(maxW, ImPlot::GetLastItemColor(), "%f", maxW);
				ImPlot::TagY(maxI, ImPlot::GetLastItemColor(), "%f", maxI);
			}
		}
		ImPlot::EndPlot();
		if(device.sig->wavelengths->size()){
			ImGui::Text("Max element: %d", maxEltIdx);
			ImGui::Text("At wavelength %lf:", (*device.sig->wavelengths)[maxEltIdx]);
			ImGui::Text("Intensity %lf:", spectrum[maxEltIdx]);
		}
		ImGui::End();
	}

	virtual void processAcquiredData(){
		if(doAcquisition){
			auto procLam = [&]{
				ewmaS.smooth(spectrum);

				auto &sm = smoothers.getSmoother();
				sm.smooth(ewmaS.getSmoothed());
				auto &smoothed = sm.getSmoothed();

				if(captureDarkButtonClicked){
					std::copy(begin(smoothed), end(smoothed), begin(dark));
				}

				if(subtractDark){
					for(size_t i=0; i< smoothed.size(); ++i){
						smoothed[i] -= dark[i];
					}
				}

				maxEltIdx = std::distance(smoothed.data(), std::max_element(smoothed.data() + padding, smoothed.data() + smoothed.size()));
			};

			if(smoothers.smoothingMode != SmoothingMode::none){
				std::lock_guard<std::mutex> guard(device.sig->allowWrite);
				procLam();
			} else {
				maxSelectionMode = false;
				procLam();
			}
		}
	}
};


int main(int, char**)
{
	fLI::FLAGS_minloglevel=3;
	auto spectrIdx = OceanOpticsSpectrometer::discover();
	if(spectrIdx.sptr<0){
		std::cerr << "No spectrometers found!" << std::endl;
		return 1;
	}

	if (!glfwInit())
		return 1;

	OurGUI gui(spectrIdx);

	gui.mainLoop();
	gui.device.stopAcquisition();

	return 0;
}
