#ifndef TTHOUGHTESTTASK_HH
#define TTHOUGHTESTTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"

/*
 * Remove this comment block after reading it through
 * Or use print_example_comments=False option to omit printing
 *
 * # Example LILAK task class
 *
 * - Write Init() method.
 * - Write Exec() or/and EndOfRun() method.
 */

/**
 * Test hough transform from raw data
 * produce root file containing histograms of hough transform result
 */
class TTHoughTestTask : public LKTask
{
    public:
        TTHoughTestTask();
        virtual ~TTHoughTestTask() { ; }

        enum class eType
        {
            kNon,
            kLeftStrip,     // 0
            kRightStrip,    // 1
            kLeftChain,     // 2
            kRightChain,    // 3
            kLowCenter,     // 4
            kHighCenter,    // 5
            kForwardSi,     // 6
            kForwardCsI,    // 7
            kMMJr,          // 8
            kCENSX6,        // 10
            kCENSCsI,       // 11
            kExternal,      // 100
        };

        enum class eDetLoc
        {
            kNon,
            kLeft,          // 0
            kRight,         // 1
            kCenterFront,   // 2
            kBottomLeftX6,  // 10
            kBottomRightX6, // 11
            kCsI,           // -1
        };

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:
        TClonesArray *fEventHeader = nullptr;
        TClonesArray *fChannelArray = nullptr;

        eType        fType[3][4][4][68];
        eDetLoc      fDetLoc[3][4][4][68];
        const Int_t  fmmnum = 1024;       ///<  # of all channels
        const Int_t  fsinum = 45;         ///<  quadrant*9
        const Int_t  fX6num = 600;        ///<  20chan*30det
        const Int_t  fCsInum = 64;        ///<  1chan*64det
        TString      fmapmmFileName;
        TString      fmapsiFileName;
        TString      fmapX6FileName;
        TString      fmapCsIFileName;

        Int_t mmpx[4][4][64];
        Int_t mmpy[4][4][64]; // [mmAsad][mmAget][dchan]

        const Int_t MaxBuck = 512;

        TH2D *timing_dt_xy;
        TH2D *timing_dt_zy;
        TH2D *timing_dt_xz;

        Int_t track_px[512];
        Int_t track_py[512];

        // Beam setup
        Int_t Beam_med = 180; //CRIB
        Int_t Beam_er = 40; //0822
        Int_t Bi = Beam_med-Beam_er;
        Int_t Bf = Beam_med+Beam_er;
        Int_t Bw = 150; //0822 chain width
        Int_t BwT = 100; //0822 chain width for find timing

        TH1D* LCWaveFormbyPixel[6];
        TH1D* HWaveFormbyPixel[64];
        TH1D *HWaveFormbyPixel_temp;
        TH2D* Hough_xt;
        TH2D* fhough_xt;
        TH2D* Hough_zt;
        TH2D* fhough_zt;
        TH2D* fhough_xz;
        TH2D* fhough_xz_check;


    ClassDef(TTHoughTestTask,1);
};

#endif
