#ifndef TTHOUGHTESTTASK_HH
#define TTHOUGHTESTTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "TexAT2.h"
#include "TTEventHeader.h"

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

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:
        TexAT2 *fDetector = nullptr;

        TTEventHeader *fEventHeader = nullptr;
        TClonesArray *fChannelArray = nullptr;

        TString fHistDataPath;

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
