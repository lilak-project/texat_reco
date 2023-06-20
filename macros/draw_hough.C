void draw_hough()
{
    gStyle -> SetOptStat(0);

    auto run = new LKRun();
    run -> AddInputFile("data/texat_0824.hough.master.10.228c308.root");
    run -> Init();

    auto header = (TTEventHeader *) run -> GetBranch("EventHeader");
    auto hitArray = run -> GetBranchA("HoughHit");
    auto trackArray = run -> GetBranchA("HoughTrack");
    auto numEvents = run -> GetNumEvents();
    auto tree = run -> GetInputTree();

    new TCanvas("cvs_hough_hits","cvs_hough_hits",20,50,600,500);
    auto histAll = new TH2D("histAll","hough hits (all events);x;z",140,0,140,140,0,140);
    tree -> Draw("HoughHit.fX:HoughHit.fZ>>histAll","fIsGoodEvent","colz");

    vector<int> listGoodEvents;
    for (auto iEvent=0; iEvent<numEvents; ++iEvent) {
        run -> GetEvent(iEvent);
        if (header->GetIsMMEvent()==false)
            continue;
        header -> Print();
        listGoodEvents.push_back(iEvent);
    }

    auto cvsAll = new TCanvas("cvs10","cvs10",3000,2000);
    cvsAll -> Divide(4,3);
    int countEvents = 0;
    for (auto iEvent : listGoodEvents)
    {
        cout << iEvent << endl;
        countEvents++;
        if (countEvents>12) break;

        //auto cvs = new TCanvas(Form("cvs_hough_%d",iEvent),Form("cvs_hough_%d",iEvent),20,50,1000,800);
        auto cvs = cvsAll -> cd(countEvents);
        TString nameHist = Form("hist_hough_%d",iEvent);
        auto hist = new TH2D(nameHist,Form("hist_hough_%d;x;z",iEvent),140,0,140,140,0,140);
        tree -> Draw(Form("HoughHit.fX:HoughHit.fZ>>%s",nameHist.Data()),Form("Entry$==%d",iEvent),"colz");

        run -> GetEvent(iEvent);
        auto track = (LKLinearTrack *) trackArray -> At(0);
        track -> Print();
        track -> TrajectoryOnPlane(LKVector3::kX,LKVector3::kY) -> Draw("samel");
    }
}
