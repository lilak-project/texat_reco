#ifndef TTROOTCONVERSIONTASK_HH
#define TTROOTCONVERSIONTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "TexAT2.h"
#include "TTEventHeader.h"
#include <iostream>

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
 * Simple conversion from pre-converted root file
 */
class TTRootConversionTask : public LKTask
{
    public:
        TTRootConversionTask();
        virtual ~TTRootConversionTask() {}

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:
        TexAT2 *fDetector = nullptr;

        TClonesArray* fEventHeaderArray = nullptr;
        TClonesArray* fChannelArray = nullptr;

        TString      fInputFileName;
        TFile*       fInputFile;
        TTree*       fInputTree;
        Int_t        fmmMult;
        Int_t        fmmHit;
        Int_t        fmmEventIdx;
        Int_t        fmmFrameNo[1030];    ///< [mmMult]
        Int_t        fmmDecayNo[1030];    ///< [mmMult]
        Int_t        fmmCobo[1030];       ///< [mmMult]
        Int_t        fmmAsad[1030];       ///< [mmMult]
        Int_t        fmmAget[1030];       ///< [mmMult]
        Int_t        fmmChan[1030];       ///< [mmMult]
        Float_t      fmmTime[1030];       ///< [mmMult]
        Float_t      fmmEnergy[1030];     ///< [mmMult]
        Int_t        fmmWaveformX[1030][512];  ///< [mmMult][time]
        Int_t        fmmWaveformY[1030][512];  ///< [mmMult][time]


    ClassDef(TTRootConversionTask,1);
};

#endif
