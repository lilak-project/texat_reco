#include "TTHTBeamTrackingTask.h"

ClassImp(TTHTBeamTrackingTask);

TTHTBeamTrackingTask::TTHTBeamTrackingTask()
{
    fName = "TTHTBeamTrackingTask";
}

bool TTHTBeamTrackingTask::Init()
{
    lk_info << "Initializing TTHTBeamTrackingTask" << std::endl;

    fHitArray = fRun -> GetBranchA("HitCenter");
    fEventHeaderHolder = fRun -> KeepBranchA("EventHeader");
    fTrackArray = fRun -> RegisterBranchA("BeamTrack","LKLinearTrack",10);

    double y1 = fY1-(fY2-fY1);
    double dy = fY2 - y1;
    fTracker = new LKHTLineTracker();
    fTracker -> SetTransformCenter(y1,(fZ2+fZ1)/2.);
    fTracker -> SetImageSpaceRange(fNZ, fZ1, fZ2, fNY, fY1, fY2);
    fTracker -> SetParamSpaceRange(fNR, 0, dy, 10, 80, 100);
    fTracker -> SetCorrelateBoxBand();
    fTracker -> SetWFConst();

    return true;
}

void TTHTBeamTrackingTask::Exec(Option_t *option)
{
    fTrackArray -> Clear("C");
    fNumTracks = 0;

    // good event?  ------------------------------------------------------------------------
    auto eventHeader = (TTEventHeader*) fEventHeaderHolder -> At(0);
    auto goodEvent = eventHeader -> IsGoodEvent();
    if (fHitArray->GetEntries()==0||goodEvent==false) {
        eventHeader -> SetIsGoodEvent(false);
        lk_info << "Bad event. Skip" << endl;
        return;
    }

    if (fHitArray->GetEntries()<fNumHitsCutForTransform)
    {
        auto numHits = fHitArray -> GetEntries();
        for (auto iHit=0; iHit<numHits; ++iHit) {
            auto hit = (LKHit*) fHitArray -> At(iHit);
            fTracker -> AddHit(hit,LKVector3::kZ,LKVector3::kY);
        }

        while (1)
        {
            fTracker -> Transform();
            auto paramPoint = fTracker -> FindNextMaximumParamPoint();
            if (paramPoint->GetWeight() < fNumHitsCutForTransform)
                break;


            fTracker -> SelectPoints(paramPoint);
            auto hitArray = fTracker -> GetSelectedHitArray();
            auto track = (LKLinearTrack*) fTrackArray -> ConstructedAt(fNumTracks++);
            TIter next(hitArray);
            LKHit* hit;
            while ((hit=(LKHit*)next()))
                track -> AddHit(hit);
            track -> Fit();
            fTracker -> RemoveSelectedPoints();
        }
    }

    lk_info << "Found " << fNumTracks << " tracks" << endl;
}

bool TTHTBeamTrackingTask::EndOfRun()
{
    return true;
}
