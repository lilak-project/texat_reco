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

class TTHTBeamTrackingTask : public LKTask
{
    public:
        TTHTBeamTrackingTask();
        virtual ~TTHTBeamTrackingTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

        LKHTLineTracker* GetTracker() { return fTracker; }

    private:
        TClonesArray *fHitArray;
        TClonesArray *fTrackArray = nullptr;
        TClonesArray* fEventHeaderHolder = nullptr;

        LKHTLineTracker* fTracker;

        int          fNumTracks = 0;

        int          fNX = 143;
        double       fX1 = -120;
        double       fX2 = 120;
        int          fNY = 110;
        double       fY1 = 0;
        double       fY2 = 330;
        int          fNZ = 141;
        double       fZ1 = 150;
        double       fZ2 = 500;
        int          fNR = 80;
        int          fNT = 100;

        int          fNumHitsCutForTransform = 10;

    ClassDef(TTHTBeamTrackingTask,1);
};

#endif
