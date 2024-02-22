#ifndef TTPULSEANALYSISTASK_HH
#define TTPULSEANALYSISTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "TTEventHeader.h"
#include "TexAT2.h"
#include "TTMicromegas.h"
//#include "LKHit.h"
//#include "GETChannel.h"
//#include "TTEventHeader.h"

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example LILAK task class
 *
 * - Write Init() method.
 * - Write Exec() or/and EndOfRun() method.
 */

class TTPulseAnalysisTask : public LKTask
{
    public:
        TTPulseAnalysisTask();
        virtual ~TTPulseAnalysisTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:
        TClonesArray* fEventHeaderHolder = nullptr;

        TClonesArray* fChannelArray = nullptr;

        TClonesArray* fHitArrayCenter = nullptr;
        TClonesArray* fHitArrayLStrip = nullptr;
        TClonesArray* fHitArrayLChain = nullptr;
        TClonesArray* fHitArrayRStrip = nullptr;
        TClonesArray* fHitArrayRChain = nullptr;
        TClonesArray* fHitArrayOthers = nullptr;

        double fTbToLength = 0.884; // mm/bin (0.0221 mm/ns * 40 ns/bin);
        double fMMY = 64.78; // mm

        TexAT2 *fDetector;

        TTMicromegas *fDetectorPlane = nullptr;

        int fITypeLStrip;
        int fITypeRStrip;
        int fITypeLChain;
        int fITypeRChain;
        int fITypeLCenter;
        int fITypeHCenter;

        void FindMaximum(Int_t *data, Bool_t Positive, Int_t &MaxVal, Int_t &MaxBin);

    ClassDef(TTPulseAnalysisTask,1);
};

#endif
