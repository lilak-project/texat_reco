#ifndef TTROOTCONVERSIONTASK_HH
#define TTROOTCONVERSIONTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
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
        
        TString GetInputFileName() const  { return fInputFileName; }
        TString GetMapmmFileName() const  { return fmapmmFileName; }
        TString GetMapsiFileName() const  { return fmapsiFileName; }
        TString GetMapX6FileName() const  { return fmapX6FileName; }
        TString GetMapCsIFileName() const  { return fmapCsIFileName; }
        
        void SetInputFileName(TString inputFileName);
        void SetMapmmFileName(TString mapmmFileName);
        void SetMapsiFileName(TString mapsiFileName);
        void SetMapX6FileName(TString mapX6FileName);
        void SetMapCsIFileName(TString mapCsIFileName);

    private:
        TClonesArray *fChannelArray = nullptr;
        
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
        eType        fType[3][4][4][68]; 
        eDetLoc      fDetLoc[3][4][4][68];    
        const Int_t  fmmnum = 1024;       ///<  # of all channels
        const Int_t  fsinum = 45;         ///<  quadrant*9
        const Int_t  fX6num = 600;        ///<  20chan*30det
        const Int_t  fCsInum = 64;        ///<  1chan*64det
        TString      fInputFileName;     
        TString      fmapmmFileName;     
        TString      fmapsiFileName;     
        TString      fmapX6FileName;     
        TString      fmapCsIFileName;    

    ClassDef(TTRootConversionTask,1);
};

#endif
