void draw_hough()
{
    gStyle -> SetOptStat(0);

    auto run = new LKRun();
    run -> AddInputFile("data/texat_0824.hough.root");
    //run -> AddFriend("data/texat_0824.raw.root");
    run -> Init();

    //auto header = (TTEventHeader *) run -> GetBranch("EventHeader");
    auto hitArray = run -> GetBranchA("HoughHit");
    auto trackArray = run -> GetBranchA("HoughTrack");
    auto numEvents = run -> GetNumEvents();
    auto tree = run -> GetInputTree();

    new TCanvas("cvs_hough_hits","cvs_hough_hits",20,50,600,500);
    auto histAll = new TH2D("histAll","hough hits (all events);x;z",140,0,140,140,0,140);
    //tree -> Draw("HoughHit.fX:HoughHit.fZ>>histAll","fIsGoodEvent","colz");
    tree -> Draw("HoughHit.fX:HoughHit.fZ>>histAll","","colz");

    vector<int> listGoodEvents;
    for (auto iEvent=0; iEvent<numEvents; ++iEvent) {
        run -> GetEvent(iEvent);
        //if (header->GetIsMMEvent()==false) continue;
        if (hitArray -> GetEntries()<50)
            continue;
        listGoodEvents.push_back(iEvent);
    }

    int countEvents = 0;
    auto hist = new TH3D("hist",";x;y;z",140,0,140,300,-150,150,140,0,140);
    for (auto iEvent : {58})
    {
        countEvents++;
        if (countEvents>12) break;

        TString nameHist = Form("hist_hough_%d",iEvent);
        new TCanvas();
        tree -> Draw("HoughHit.fX:HoughHit.fY:HoughHit.fZ>>hist",Form("Entry$==%d",iEvent),"box");
        break;
    }
}
