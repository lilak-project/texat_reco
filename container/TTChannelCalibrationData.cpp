#include "TTChannelCalibrationData.h"

ClassImp(TTChannelCalibrationData);

TTChannelCalibrationData::TTChannelCalibrationData()
{
    Clear();
}

void TTChannelCalibrationData::Clear(Option_t *option)
{
    LKContainer::Clear(option);
    fID = -1;
    fCobo = -1;
    fAsad = -1;
    fAget = -1;
    fChan = -1;
    fDChan = -1;
    fPedestal = 0;
    fPedestalError = 0;
    fCountChannels = 0;
}

void TTChannelCalibrationData::Init(Int_t id)
{
    fID = id;
    if (fHistWaveformY==nullptr)
        fHistWaveformY = new TH1D(Form("hist_calibration_data_%d",fID),";x;WaveformY/NChannels",512,0,512);
}

void TTChannelCalibrationData::Print(Option_t *option) const
{
    // You will probability need to modify here
    e_info << "TTChannelCalibrationData" << std::endl;
    e_info << "fID : " << fID << std::endl;
    e_info << "fCobo : " << fCobo << std::endl;
    e_info << "fAsad : " << fAsad << std::endl;
    e_info << "fAget : " << fAget << std::endl;
    e_info << "fChan : " << fChan << std::endl;
    e_info << "fDChan : " << fDChan << std::endl;
    e_info << "fPedestal : " << fPedestal << std::endl;
    e_info << "fPedestalError : " << fPedestalError << std::endl;
    e_info << "fCountChannels : " << fCountChannels << std::endl;
    ;
}

void TTChannelCalibrationData::Copy(TObject &object) const
{
    // You should copy data from this container to objCopy
    LKContainer::Copy(object);
    auto objCopy = (TTChannelCalibrationData &) object;
    objCopy.SetID(fID);
    objCopy.SetCobo(fCobo);
    objCopy.SetAsad(fAsad);
    objCopy.SetAget(fAget);
    objCopy.SetChan(fChan);
    objCopy.SetDChan(fDChan);
    objCopy.SetPedestal(fPedestal);
    objCopy.SetPedestalError(fPedestalError);
    objCopy.SetCountChannels(fCountChannels);
    objCopy.SetHistWaveformY(fHistWaveformY);
}

void TTChannelCalibrationData::AddWaveformY(Int_t* waveform)
{
    for (auto i=0; i<512; ++i) {
        auto inContent = waveform[i];
        auto preContent = fHistWaveformY -> GetBinContent(i+1);
        auto newContent = (preContent*fCountChannels + inContent) / (fCountChannels + 1);
        fHistWaveformY -> SetBinContent(i+1,newContent);
    }
    fCountChannels++;
}

void TTChannelCalibrationData::Analyze()
{
    ;
}

void TTChannelCalibrationData::Draw(Option_t *option)
{
    ;
}

