void test_track()
{
    auto file = new TFile("data/texat_0824.hough.master.10.228c308.root");
    auto tree = file -> Get<TTree>("event");

    TClonesArray *trackArray = nullptr;
    TTEventHeader *header = nullptr;
    tree -> SetBranchAddress("EventHeader",&header);
    tree -> SetBranchAddress("HoughTrack",&trackArray);

    auto numEvents = tree -> GetEntries();
    for (auto iEvent=0; iEvent<numEvents; ++iEvent) {
        tree -> GetEntry(iEvent);
        if (header->GetIsMMEvent()==false)
            continue;
        auto track = (LKLinearTrack *) trackArray -> At(0);
        track -> Print();
    }
}
