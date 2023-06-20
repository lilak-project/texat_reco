void draw_all()
{
    gStyle -> SetOptStat(0);
    auto file = new TFile("../data/texat_0824.hough.master.8.ccf152e.root", "read");
    auto tree = file -> Get<TTree>("event");

    new TCanvas("cvs_hough_hits","cvs_hough_hits",20,50,600,500);
    auto histAll = new TH2D("histAll","hough hits (all events);x;z",140,0,140,140,0,140);
    tree -> Draw("HoughHit.fX:HoughHit.fZ>>histAll","","colz");

    for (auto eventID : {70,72}) {
        new TCanvas(Form("cvs_hough_hits_%d",eventID),Form("cvs%d",eventID),40,70,600,500);
        auto histAll = new TH2D(Form("hist%d",eventID),Form("hough hits (%d);x;z",eventID),140,0,140,140,0,140);
        tree -> Draw(Form("HoughHit.fX:HoughHit.fZ>>hist%d",eventID),Form("Entry$==%d",eventID),"colz");
    }
}
