#ifndef TTCHANNELCALIBRATIONDATA_HH
#define TTCHANNELCALIBRATIONDATA_HH

#include "TClonesArray.h"
#include "LKContainer.h"
#include "LKLogger.h"
#include "TH1D.h"

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example LILAK container class
 *
 * ## Must
 * - Write Clear() method
 * : Clear() method clears and intialize the data class.
 * This is "Must Write Method" because containers are not recreated each event,
 * but their memories are reused after Clear method is called.
 * and they are filled up and written to tree each event.
 * See: https://root.cern/doc/master/classTClonesArray.html, https://opentutorials.org/module/2860/19477
 *
 * - Version number (2nd par. in ClassDef of source file) should be changed if the class has been modified.
 * This notifiy users that the container has been update in the new LILAK (or side project version).
 *
 * ## Recommended
 * - Documentaion like this!
 * - Write Print() to see what is inside the container;
 *
 * ## If you have time
 * - Write Copy() for copying object
 */

/**
 * calibration data for get channels
 */
class TTChannelCalibrationData : public LKContainer
{
    public:
        TTChannelCalibrationData();
        virtual ~TTChannelCalibrationData() { ; }

        void Init(Int_t id);
        void Clear(Option_t *option="");
        void Print(Option_t *option="") const;
        void Copy(TObject &object) const;

        Int_t GetID() const  { return fID; }
        Int_t GetCobo() const  { return fCobo; }
        Int_t GetAsad() const  { return fAsad; }
        Int_t GetAget() const  { return fAget; }
        Int_t GetChan() const  { return fChan; }
        Int_t GetDChan() const  { return fDChan; }
        Double_t GetPedestal() const  { return fPedestal; }
        Double_t GetPedestalError() const  { return fPedestalError; }
        Int_t GetCountChannels() const  { return fCountChannels; }
        TH1D* GetHistWaveformY() const  { return fHistWaveformY; }

        void SetID(Int_t iD) { fID = iD; }
        void SetCobo(Int_t cobo) { fCobo = cobo; }
        void SetAsad(Int_t asad) { fAsad = asad; }
        void SetAget(Int_t aget) { fAget = aget; }
        void SetChan(Int_t chan) { fChan = chan; }
        void SetDChan(Int_t dChan) { fDChan = dChan; }
        void SetPedestal(Double_t pedestal) { fPedestal = pedestal; }
        void SetPedestalError(Double_t pedestalError) { fPedestalError = pedestalError; }
        void SetCountChannels(Int_t countChannels) { fCountChannels = countChannels; }
        void SetHistWaveformY(TH1D* histWaveformY) { fHistWaveformY = histWaveformY; }

        void AddWaveformY(Int_t* waveform);
        void Analyze();
        void Draw(Option_t *option="");

        Int_t        fID = -1;
        Int_t        fCobo = -1;
        Int_t        fAsad = -1;
        Int_t        fAget = -1;
        Int_t        fChan = -1;
        Int_t        fDChan = -1;
        Double_t     fPedestal = 0;
        Double_t     fPedestalError = 0;
        Int_t        fCountChannels = 0;
        TH1D*        fHistWaveformY = nullptr; ///<

    ClassDef(TTChannelCalibrationData, 1);
};

#endif
