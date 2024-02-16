#ifndef TTHTBEAMTRACKINGTASK_HH
#define TTHTBEAMTRACKINGTASK_HH

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

class TTHTBeamTrackingTask : public LKTask
{
    public:
        TTHTBeamTrackingTask();
        virtual ~TTHTBeamTrackingTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

        LKHTLineTracker* GetTracker() { return fTrackerZY; }

    private:
        TClonesArray* fHitArray = nullptr;
        TClonesArray* fTrackArray = nullptr;
        TClonesArray* fEventHeaderHolder = nullptr;

        LKHTLineTracker* fTrackerZY;

        int          fNumTracks = 0;

        int          fNX = 12;
        double       fX1 = -20.58; // MMAllLong*6
        double       fX2 = 20.58; // MMAllLong*6
        int          fNY = 120;
        double       fY1 = 0;
        double       fY2 = 360;
        int          fNZ = 200;
        double       fZ1 = 156.7;
        double       fZ2 = 506.7;

        double       fZT = 0.5*(fZ1+fZ2);
        double       fYT = fY1-(fY2-fY1);

        int          fNT = 5;
        double       fT1 = 80;
        double       fT2 = 100;

        int          fNR = 40;
        double       fR1 = fY1-fYT;
        double       fR2 = fY2-fYT;

        const int    kCenter = 0;
        const int    kLStrip = 1;
        const int    kLChain = 2;
        const int    kRStrip = 3;
        const int    kRChain = 4;
        const int    kOthers = 5;

        const int    kViewXY = 0;
        const int    kViewZY = 1;
        const int    kViewXZ = 2;

        //const int    kStrip = 0;
        //const int    kChain = 1;
        ////const int    kStripAndChain = 2;

        //const int    kLeft = 0;
        //const int    kRight = 1;
        ////const int    kCenter = 2;

        int          fNumTrackHitsCut = 10;
        int          fNumBeamTracksCut = 4;

        /* par-name: TTHTTrackingTask/use_transform_with_chain_strip_combination
         * If fUseTransformCSCombination is true, both chain- and strip-hits will be used for filling parameter space
         * while fitting will only be done using chain-hits(strip-hits) for XY-plane(ZY-plane).
         * If fUseTransformCSCombination is false, both only chain-hits(strip-hits) will used for filling parameter space of XY-plane(ZY-plane).
         */
        //bool         fUseTransformCSCombination = false;

    ClassDef(TTHTBeamTrackingTask,1);
};

#endif
