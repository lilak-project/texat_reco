#include "TTHTBeamTrackingTask.h"
#include "LKGeoBox.h"

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

    fTrackArray = fRun -> RegisterBranchA("BeamTrack","LKLinearTrack",100);

    fPar -> UpdatePar(fZT,"TTHTBeamTrackingTask/transform_center_zy", 0);
    fPar -> UpdatePar(fYT,"TTHTBeamTrackingTask/transform_center_zy", 1);
    fPar -> UpdateBinning("TTHTBeamTrackingTask/y_binning", fNY, fY1, fY2);
    fPar -> UpdateBinning("TTHTBeamTrackingTask/z_binning", fNZ, fZ1, fZ2);
    fZT = 0.5*(fZ1+fZ2);
    fYT = fY1-(fY2-fY1);
    fR1 = fY1-fYT;
    fR2 = fY2-fYT;
    fPar -> UpdatePar(fNumTrackHitsCut,  "TTHTBeamTrackingTask/num_track_hits_cut");
    fPar -> UpdatePar(fNumBeamTracksCut, "TTHTBeamTrackingTask/num_beam_tracks_cut");
    fPar -> UpdateBinning("TTHTBeamTrackingTask/r_binning", fNR, fR1, fR2);
    fPar -> UpdateBinning("TTHTBeamTrackingTask/t_binning", fNT, fT1, fT2);

    fTrackerZY = new LKHTLineTracker();
    fTrackerZY -> SetTransformCenter(fZT, fYT);
    fTrackerZY -> SetParamSpaceRange(fNR, fR1, fR2, fNT, fT1, fT2);
    fTrackerZY -> SetImageSpaceRange(fNZ, fZ1, fZ2, fNY, fY1, fY2);
    fTrackerZY -> SetCorrelateBoxBand();
    fTrackerZY -> SetWFConst();
    fTrackerZY -> Print();

    return true;
}

void TTHTBeamTrackingTask::Exec(Option_t *option)
{
    fTrackArray -> Clear("C");
    fNumTracks = 0;

    auto numHitsAll = 0;
    numHitsAll += fHitArray -> GetEntries();

    // good event?  ------------------------------------------------------------------------
    auto eventHeader = (TTEventHeader*) fEventHeaderHolder -> At(0);
    auto goodEvent = eventHeader -> IsGoodEvent();
    if (numHitsAll==0||goodEvent==false) {
        //eventHeader -> SetIsGoodEvent(false);
        lk_info << "Bad event. Skip" << endl;
        return;
    }
    else
        lk_info << "Good event. Reconstruct beam: " << eventHeader -> GetEventNumber() << endl;

    // clear ------------------------------------------------------------------------
    fTrackerZY -> Clear();

    //LKGeoBox beamRegion(0.5*(fX1+fX2),0.5*(fY2+fY1),0.5*(fZ2+fZ1),fX2-fX1,fY2-fY1,fZ2-fZ1);

    // add transform and fit ------------------------------------------------------------------------
    if (fHitArray->GetEntries()>fNumTrackHitsCut)
    {
        auto numHits = fHitArray -> GetEntries();
        for (auto iHit=0; iHit<numHits; ++iHit) {
            auto hit = (LKHit*) fHitArray -> At(iHit);
            fTrackerZY -> AddHit(hit,LKVector3::kZ,LKVector3::kY);
        }

        fTrackerZY -> Transform();

        for (auto iFind=0; iFind<fNumBeamTracksCut; ++iFind)
        {
            if (fTrackerZY->GetNumImagePoints()<fNumTrackHitsCut)
                break;

            auto paramPointZY = fTrackerZY -> FindNextMaximumParamPoint();
            //paramPointZY -> Print();

            if (paramPointZY->GetWeight()<fNumTrackHitsCut)
                break;

            auto trackFitted = fTrackerZY -> FitTrack3D(paramPointZY);
            auto track = (LKLinearTrack*) fTrackArray -> ConstructedAt(fNumTracks);
            track -> CopyFrom(trackFitted);
            track -> SetTrackID(1000+fNumTracks);

            TVector3 point1 = track -> GetPoint1();
            TVector3 point2 = track -> GetPoint2();
            if (point2.Z()<point1.Z())
                point2 = point1;
            point1 = track -> GetPointAtZ(fZ1);
            track -> SetTrack(point1, point2);
            track -> Print();

            //beamRegion.GetCrossingPoints(*track,point1,point2);

            fTrackerZY -> RemoveSelectedPoints(); // TODO
            fTrackerZY -> Reset();
            fTrackerZY -> Transform();

            ++fNumTracks;
        }
    }

    lk_info << "Found " << fNumTracks << " beam tracks" << endl;
}

bool TTHTBeamTrackingTask::EndOfRun()
{
    return true;
}
