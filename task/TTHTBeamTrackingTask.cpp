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

    fPar -> UpdatePar(fNumTrackHitsCut,  "TTHTBeamTrackingTask/num_track_hits_cut");
    fPar -> UpdatePar(fNumBeamTracksCut, "TTHTBeamTrackingTask/num_beam_tracks_cut");
    fPar -> UpdatePar(fZT,  "TTHTBeamTrackingTask/transform_center_zy", 0);
    fPar -> UpdatePar(fYT,  "TTHTBeamTrackingTask/transform_center_zy", 1);
    fPar -> UpdatePar(fNR,  "TTHTBeamTrackingTask/radius_binning",0);
    fPar -> UpdatePar(fR1,  "TTHTBeamTrackingTask/radius_binning",1);
    fPar -> UpdatePar(fR2,  "TTHTBeamTrackingTask/radius_binning",2);
    fPar -> UpdatePar(fNT,  "TTHTBeamTrackingTask/theta_binning",0);
    fPar -> UpdatePar(fT1,  "TTHTBeamTrackingTask/theta_binning",1);
    fPar -> UpdatePar(fT2,  "TTHTBeamTrackingTask/theta_binning",2);
    fPar -> UpdatePar(fNZ,  "TTHTBeamTrackingTask/z_binning",0);
    fPar -> UpdatePar(fZ1,  "TTHTBeamTrackingTask/z_binning",1);
    fPar -> UpdatePar(fZ2,  "TTHTBeamTrackingTask/z_binning",2);
    fPar -> UpdatePar(fNY,  "TTHTBeamTrackingTask/y_binning",0);
    fPar -> UpdatePar(fY1,  "TTHTBeamTrackingTask/y_binning",1);
    fPar -> UpdatePar(fY2,  "TTHTBeamTrackingTask/y_binning",2);

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
