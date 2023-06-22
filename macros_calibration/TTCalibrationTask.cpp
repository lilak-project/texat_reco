#include "TTCalibrationTask.h"

ClassImp(TTCalibrationTask);

TTCalibrationTask::TTCalibrationTask()
{
    fName = "TTCalibrationTask";
}

bool TTCalibrationTask::Init()
{
    // Put intialization todos here which are not iterative job though event
    lk_info << "Initializing TTCalibrationTask" << std::endl;

    TString calibrate = fPar -> GetParString("TTCalibrationTask/Calibrate");
    if (!calibrate.IsNull())
    {
        calibrate.ToLower();

        fCalibrateChannel = false;
        fCalibrateAGET = false;

        if (calibrate=="channel") fCalibrateChannel = true;
        if (calibrate=="aget") fCalibrateAGET = true;
        else fCalibrateChannel = true;
    }

    fChannelArray = fRun -> GetBranchA("RawData");

    if (fCalibrateChannel)
    {
        lk_info << "Calibrate Channel" << endl;
        fCalibrationDataArray = new TClonesArray("TTChannelCalibrationData", 68*4*4*3);
        for (auto cobo=0; cobo<3; ++cobo)
            for (auto asad=0; asad<4; ++asad)
                for (auto aget=0; aget<4; ++aget)
                    for (auto chan=0; chan<68; ++chan) {
                        auto id = chan + aget*68 + asad*68*4 + cobo*68*4*4;
                        auto calbrationData = (TTChannelCalibrationData *) fCalibrationDataArray -> ConstructedAt(id);
                        calbrationData -> Init(id);
                        calbrationData -> SetChan(chan);
                        //calbrationData -> SetDChan(dchan);
                        calbrationData -> SetCobo(cobo);
                        calbrationData -> SetAsad(asad);
                        calbrationData -> SetAget(aget);
                    }
    }
    else if (fCalibrateAGET)
    {
        lk_info << "Calibrate AGET" << endl;
        fCalibrationDataArray = new TClonesArray("TTChannelCalibrationData", 4*4*3);
        for (auto cobo=0; cobo<3; ++cobo)
            for (auto asad=0; asad<4; ++asad)
                for (auto aget=0; aget<4; ++aget) {
                    auto id = aget + asad*4 + cobo*4*4;
                    auto calbrationData = (TTChannelCalibrationData *) fCalibrationDataArray -> ConstructedAt(id);
                    calbrationData -> Init(id);
                    calbrationData -> SetCobo(cobo);
                    calbrationData -> SetAsad(asad);
                    calbrationData -> SetAget(aget);
                }
    }

    fRun -> RegisterObject("CalibrationData", fCalibrationDataArray);

    return true;
}

void TTCalibrationTask::Exec(Option_t *option)
{
    // Construct (new) channel from fChannelArray and set data value
    int numChannel = fChannelArray -> GetEntriesFast();
    for (int iChannel = 0; iChannel < numChannel; ++iChannel)
    {
        auto channel = (MMChannel *) fChannelArray -> At(iChannel);
        Int_t chan = channel -> GetChan();
        Int_t dchan = channel -> GetDChan();
        Int_t cobo = channel -> GetCobo();
        Int_t asad = channel -> GetAsad();
        Int_t aget = channel -> GetAget();
        Int_t *waveformY = channel -> GetWaveformY();
        TTChannelCalibrationData *calbrationData = nullptr;
        if (fCalibrateChannel) {
            auto id = chan + aget*68 + asad*68*4 + cobo*68*4*4;
            calbrationData = (TTChannelCalibrationData *) fCalibrationDataArray -> At(id);
        }
        else if (fCalibrateAGET) {
            auto id = aget + asad*4 + cobo*4*4;
            calbrationData = (TTChannelCalibrationData *) fCalibrationDataArray -> At(id);
        }
        calbrationData -> AddWaveformY(waveformY);
    }

    lk_info << "TTCalibrationTask" << std::endl;
}

bool TTCalibrationTask::EndOfRun()
{
    return true;
}

