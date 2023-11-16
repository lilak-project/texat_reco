#include "LKLogger.h"

TObjArray* fHitArrayAll = nullptr;
TClonesArray* fHitArray[6];
TClonesArray* fTrackArray;

void nextEvent()
{
    LKRun::GetRun() -> RunSelectedEvent("EventHeader[0].IsGoodEvent()");

    // get all hits and put them into single array object ---------------------
    if (fHitArrayAll==nullptr)
        fHitArrayAll = new TObjArray();
    fHitArrayAll -> Clear();
    for (auto iArray=0; iArray<6; ++iArray) {
        auto numHits = fHitArray[iArray] -> GetEntries();
        for (auto iHit=0; iHit<numHits; ++iHit) {
            auto hit = (LKHit*) fHitArray[iArray] -> At(iHit);
            auto id = hit -> GetHitID();
            fHitArrayAll -> AddAtAndExpand(hit, id);
        }
    }
    e_info << "Number of hits = " << fHitArrayAll -> GetEntries() << endl;

    // put hits into the track ------------------------------------------------
    auto numTracks = fTrackArray -> GetEntries();
    e_info << "Number of tracks = " << numTracks << endl;
    for (auto iTrack=0; iTrack<numTracks; ++iTrack) {
        auto track = (LKLinearTrack *) fTrackArray -> At(iTrack);
        if (track->GetNumHits()>0&&track->IsHoldingHits()) 
            break; // track is already holding hits >> no need to fill hits manually!
        auto idArray0 = track -> GetHitIDArray();
        vector<int> idArray; for (auto id : *idArray0) idArray.push_back(id);
        track -> ClearHits();
        for (auto id : idArray) {
            auto hit = (LKHit*) fHitArrayAll -> At(id);
            if (hit==nullptr)
                e_warning << id << endl;
            track -> AddHit(hit);
        }
    }

    // look inside the track --------------------------------------------------
    double chargeSum = 0.;
    for (auto iTrack=0; iTrack<numTracks; ++iTrack) {
        auto track = (LKLinearTrack *) fTrackArray -> At(iTrack);
        track -> Print();
        auto hitArray = track -> GetHitArray();
        auto numHits = hitArray -> GetNumHits();
        for (auto iHit=0; iHit<numHits; ++iHit) {
            auto hit = hitArray -> GetHit(iHit);
            auto charge = hit -> GetCharge();
            chargeSum += charge;
        }
        e_info << "length / charge-sum = " << track -> TrackLength() << " / " << chargeSum << endl;
    }
}

void check_tracks()
{
    auto run = new LKRun();
    run -> AddInputFile("~/data/texat/reco/texat_0801.all.root");
    run -> AddFriend("~/data/texat/reco/texat_0801.reco.root");
    run -> Init();

    fTrackArray  = run -> GetBranchA("Track");
    fHitArray[0] = run -> GetBranchA("HitCenter");
    fHitArray[1] = run -> GetBranchA("HitLStrip");
    fHitArray[2] = run -> GetBranchA("HitLChain");
    fHitArray[3] = run -> GetBranchA("HitRStrip");
    fHitArray[4] = run -> GetBranchA("HitRChain");
    fHitArray[5] = run -> GetBranchA("HitOthers");

    if (fTrackArray==nullptr || fHitArray[0]==nullptr) {
        e_error << "data array is null!" << endl;
        return;
    }

    nextEvent();
}
