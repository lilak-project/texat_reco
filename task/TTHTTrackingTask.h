#ifndef TTHTTRACKINGTASK_HH
#define TTHTTRACKINGTASK_HH

#include "TClonesArray.h"
#include "TObjArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "LKHit.h"
#include "LKLinearTrack.h"
#include "LKHTLineTracker.h"
#include "TTEventHeader.h"
#include "TTEventEnder.h"
//#include "EventHeader"

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example LILAK task class
 *
 * - Write Init() method.
 * - Write Exec() or/and EndOfRun() method.
 */

class TTHTTrackingTask : public LKTask
{
    public:
        TTHTTrackingTask();
        virtual ~TTHTTrackingTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

        bool TransformAndSelectHits(LKHTLineTracker* trackerXY, LKHTLineTracker* trackerZY);
        bool MakeTrack(int iLeftRight);

        LKHTLineTracker* GetTracker(int i, int j) { return fTracker[i][j]; }

    private:
        //TClonesArray *fHitArray[3][2];
        TClonesArray *fHitArray[6];
        TClonesArray *fTrackArray = nullptr;
        TClonesArray* fEventHeaderHolder = nullptr;
        TClonesArray* fEventEnderHolder = nullptr;

        TObjArray* fCrossHitCollection = nullptr;

        LKHTLineTracker* fTracker[2][2];

        LKLinearTrack* fTrackXY = nullptr;
        LKLinearTrack* fTrackZY = nullptr;

        int          fNumTracks = 0;

        int          fNX = 143;
        double       fX1 = -120;
        double       fX2 = 120;

        int          fNY = 110;
        double       fY1 = 0;
        double       fY2 = 350;

        int          fNZ = 200;
        double       fZ1 = 0;
        //int          fNZ = 141;
        //double       fZ1 = 150;
        double       fZ2 = 500;

        int          fNR = 80;
        double       fR1 = 0;
        double       fR2 = 0;

        int          fNT = 100;
        double       fT1 = 0;
        double       fT2 = 0;

        double       fTCX = fX1;
        double       fTCY = fY1;
        double       fTCZ = fZ1;

        const int    kCenter = 0;
        const int    kLStrip = 1;
        const int    kLChain = 2;
        const int    kRStrip = 3;
        const int    kRChain = 4;
        const int    kOthers = 5;

        const int    kViewXY = 0;
        const int    kViewZY = 1;
        const int    kViewXZ = 2;

        const int    kStrip = 0;
        const int    kChain = 1;
        //const int    kStripAndChain = 2;

        const int    kLeft = 0;
        const int    kRight = 1;
        //const int    kCenter = 2;

        int          fNumChainHitsCutForTransform = 3; ///< number of chain hits should be larger than this cut to be reconstructed
        int          fNumStripHitsCutForTransform = 3; ///< number of strip hits should be larger than this cut to be reconstructed

        /* par-name: TTHTTrackingTask/use_transform_with_chain_strip_combination
         * If fUseTransformCSCombination is true, both chain- and strip-hits will be used for filling parameter space
         * while fitting will only be done using chain-hits(strip-hits) for XY-plane(ZY-plane).
         * If fUseTransformCSCombination is false, both only chain-hits(strip-hits) will used for filling parameter space of XY-plane(ZY-plane).
         */
        bool         fUseTransformCSCombination = true;

        double       minCharge = DBL_MIN;
        double       maxCharge = DBL_MAX;

    ClassDef(TTHTTrackingTask,1);
};

#endif
