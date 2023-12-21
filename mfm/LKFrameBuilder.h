#ifndef LKFRAMEBUILDER_H
#define LKFRAMEBUILDER_H

#include "mfm/FrameBuilder.h"
#include <map>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TTree.h>
#include <TKey.h>
#include <TH1F.h>
#include <TH2I.h>
#include <TGraph2D.h>
#include <TProfile.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TF2.h>
#include <TRandom.h>
#include <TMath.h>
#include <TCutG.h>
#include <string.h>
#include <TError.h>
#include <fstream>

#include "TClonesArray.h"

#include "LKTask.h"

using namespace std;
class GSpectra;
class GNetServerRoot;

class WaveForms {
    public:
        WaveForms() {};
        ~WaveForms() {};
        Bool_t isRejected;
        vector<vector<Bool_t>> hasSignal; // To let us know if there was a signal that fired
        vector<Bool_t> hasHit; // To let us know if there was any signal other than FPNs
        vector<Bool_t> hasFPN; // To let us know if there was any FPN signal
        vector<Bool_t> doneFPN; // To let us know if there was any FPN signal averaged
        vector<vector<Bool_t>> isOverflow; // To let us know if the signal was overflow.
        Bool_t hasOverflow; // To let us know if the event has overflow channels.,
        vector<vector<Int_t>> isDecay;
        Bool_t hasImplant;
        Bool_t hasDecay;
        UInt_t frameIdx;
        UInt_t decayIdx;
        UInt_t ICenergy;
        UInt_t Sienergy;
        Double_t SiX;
        Double_t SiY;
        Double_t SiZ;
        UInt_t EstripL;
        UInt_t EstripR;
        UInt_t coboIdx;
        UInt_t asadIdx;
        vector<vector<vector<UInt_t>>> waveform; // To save the waveform for each Aget, Channel and Bucket
        vector<vector<vector<Int_t>>> corrwaveform; // To save the corrected waveform by the averaged FPN waveform
        vector<vector<UInt_t>> fpnwaveform; // To save the waveform for each Aget, FPN Channel and Bucket
        vector<vector<UInt_t>> energy; // Digitized energy value
        vector<vector<UInt_t>> time; // time value
        vector<vector<UInt_t>> baseline; // baseline value
        vector<vector<Double_t>> PSDIntegral; // To save ratio of peak to integral.
        vector<vector<Double_t>> PSDRatio; // To save ratio of peak to integral.
};

class LKFrameBuilder : public mfm::FrameBuilder {
    private:
        TClonesArray *fChannelArray = nullptr;
        TClonesArray *fEventHeaderArray = nullptr;
        LKTask *fMotherTask;
        TString fName;
        int fCountChannels = 0;

    public:
        LKFrameBuilder(int);
        ~LKFrameBuilder();

        void processFrame(mfm::Frame & frame);
        void decodeMuTanTFrame(mfm::Frame & frame); // processFrame
        void decodeCoBoTopologyFrame(mfm::Frame& frame); // processFrame
        void ValidateEvent(mfm::Frame & frame);
        void ValidateFrame(mfm::Frame& frame);
        void Event(mfm::Frame & frame);
        void ResetWaveforms(); // Event
        void UnpackFrame(mfm::Frame& frame); // Event
        void RootWReset(); // UnpackFrame
        void WriteChannels();

        void Init(int mode, int d2pmode);
        void SetBucketSize(int BucketSize) { bucketmax = BucketSize; }
        void SetReadMode(int flag) { readmode = flag; }
        void SetReadType(int flag) { readtype = flag; }
        void SetScaler(int flag) { enablescaler = flag; }
        void Set2pMode(int flag) { enable2pmode = flag; }
        void SetUpdateSpeed(int flag) { enableupdatefast = flag; }
        void InitWaveforms(); // Init
        void SetChannelArray(TClonesArray *channelArray) { fChannelArray = channelArray; }
        void SetEventHeaderArray(TClonesArray *eventHeaderArray) { fEventHeaderArray = eventHeaderArray; }
        void SetMotherTask(LKTask* task) { fMotherTask = task; }

    protected:
        GSpectra* spectra_;
        GNetServerRoot* serv_;
        unsigned int triggerrate[2];
        int maxcobo;
        int maxasad;
        WaveForms* waveforms;
        WaveForms* rwaveforms[2][3];

        int bucketmax;
        int readmode;
        int readtype;
        int enableroot;
        int forcereadtree;
        int enablecleantrack;
        int energymethod;
        int readrw;
        int ignoremm;
        int enabledraww;
        int enableskipevent;
        int firsteventno;
        int enabletrack;
        int enablescaler;
        int enable2pmode;
        int enablehist;
        int enableupdatefast;
        int hasDrawn[16];
        UInt_t bmpos; //BM Position
        UInt_t bmsum1; //BM Energy by position ratio
        UInt_t bmsum2; //BM Energy by sig_sum
        UInt_t eventID; //weventIdx;
        UInt_t weventTime;
        UInt_t goodsievt;
        UInt_t goodmmevt;
        UInt_t prevgoodmmevt;
        UInt_t goodicevt;
        UInt_t prevgoodicevt;
        UInt_t goodevtidx;
        UInt_t goodsicsievt;
        UInt_t goodx6evt;
        UInt_t goodx6csievt;
        UInt_t goodsicsievtidx;
        UInt_t goodsicsipevt;
        UInt_t goodsicsipevtidx;
        UInt_t reventIdx;
        UInt_t reventTime;
        UInt_t rd2ptime;
        UInt_t rtstmp;
        UInt_t FirsteventIdx;
        UInt_t LasteventIdx;
        Bool_t IsFirstevent;
        Bool_t IsDecayEvt;
        UInt_t coboIC;
        UInt_t asadIC;
        UInt_t chanIC;
        UInt_t valueIC_min;
        Int_t L1Aflag;
        UInt_t IsTrig[16];
        int evtcounter;
        int goodevtcounter;
        int gatedevtcounter;
        int badevtcounter;
        int framecounter;
        int printed;
        int hitcounter;
        int prevmidx;
        int mutantcounter;
        vector<UInt_t> mevtidx; // eventidx value
        vector<UInt_t> mtstmp; // timestamp value
        vector<UInt_t> md2ptime; // d2ptime value
        double scaler1;
        double scaler1start;
        double scaler1end;
        double scaler2;
        double scaler2start;
        double scaler2end;
        double scaler3;
        double scaler3start;
        double scaler3end;
        double scaler4;
        double scaler4start;
        double scaler4end;
        double scaler5;
        double scaler5start;
        double scaler5end;
        ofstream scalerout;
        uint32_t scevent;
        uint32_t scevtidx;
        uint64_t sctstmp;
        double driftv;
        double rx;
        double ry;
        double rz;
        double rt;

        Int_t maxValue;
        Int_t maxValueBucket;
        Int_t maxValuedec;
        Int_t maxValueBucketdec;
        Double_t maxValuefit;
        Double_t maxValueBucketfit;
        UInt_t mm_minenergy;
        UInt_t mm_maxenergy;
        UInt_t mm_mintime;
        UInt_t mm_maxtime;
        UInt_t si_minenergy;
        UInt_t si_maxenergy;
        UInt_t si_mintime;
        UInt_t si_maxtime;
        TFile* fInputFile;
        TTree* fInputTree;
        TFile* fOutputFile;
        UInt_t fInputFileSize;
        TTree* fOutputTree;
        Int_t fNumberEvents;
        Float_t fTimePerBin;
        Int_t wGETMul;
        Int_t wGETHit;
        Int_t wGETEventIdx;
        Int_t wGETTimeStamp;
        Int_t wGETD2PTime;
        Int_t wGETFrameNo[4352];
        Int_t wGETDecayNo[4352];
        Int_t wGETL1Aflag[4352];
        Float_t wGETTime[4352];
        Float_t wGETEnergy[4352];
        Int_t wGETCobo[4352];
        Int_t wGETAsad[4352];
        Int_t wGETAget[4352];
        Int_t wGETChan[4352];
        Int_t wGETWaveformX[4352][512];
        Int_t wGETWaveformY[4352][512];
        Int_t wfmaxvalue[4352];
        Int_t wfminvalue[4352];
        Int_t wfbaseline[4352];
        Int_t wfdvalue[4352];
        Int_t rGETMul;
        Int_t rGETHit;
        Int_t rGETEventIdx;
        Int_t rGETTimeStamp;
        Int_t rGETD2PTime;
        Int_t rGETFrameNo[4352];
        Int_t rGETDecayNo[4352];
        Int_t rGETL1Aflag[4352];
        Float_t rGETTime[4352];
        Float_t rGETEnergy[4352];
        Int_t rGETCobo[4352];
        Int_t rGETAsad[4352];
        Int_t rGETAget[4352];
        Int_t rGETChan[4352];
        Int_t rGETWaveformX[4352][512];
        Int_t rGETWaveformY[4352][512];
        Double_t posenergysum[170];
        Double_t energysum[170];
        Double_t stripenergysum[170];
        Double_t chainenergysum[170][3];
        Int_t sumcounter[170];
        Int_t hMM_TrackCounter1[3];
        Int_t hMM_TrackCounter2[3];
        Int_t hMM_TrackdECounter[3];
        Double_t hMM_dE1[3];
        Double_t hMM_dE2[3];
        Double_t hMM_E[3];
        Int_t hMM_cornerfound[3];
        Int_t hMM_MinPxX[3];
        Int_t hMM_MinPxY[3];
        Int_t hMM_MaxPxX[3];
        Int_t hMM_MaxPxY[3];
        Double_t hMM_MinX[3];
        Double_t hMM_MinY[3];
        Double_t hMM_MaxX[3];
        Double_t hMM_MaxY[3];
        Double_t hMM_Slope[3];
        Double_t hMM_Offset[3];
        //int csi_channo[9]={2,7,10,16,19,25,28,33,36};
        int csi_channo[9]={2,7,16,10,19,25,33,28,36};
        int csi_map[64];
        Double_t trackposymin[3];
        Double_t trackposymax[3];
        int maxrespsample = 0;
        Int_t rftype;
        TF1* fResponse[10];
        TH1D* hResponse[10];
        Double_t response[10][512];
        Int_t responsesample[10][7];
        UInt_t maxresponse[10];
        UInt_t responsesigma[10];
        UInt_t deconvrep[10];
        UInt_t deconviter[10];
        UInt_t deconvboost[10];
        UInt_t decoffset;
        Double_t corrwaveformdec[512];
        Double_t corrwaveformfit[512];
};

#endif
