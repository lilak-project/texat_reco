#include "LKLogger.h"

LKRun *run;
TCanvas *cvs[2];
TTHTTrackingTask* htt;
const int kViewXY = 0;
const int kViewZY = 1;
const int kLeft = 0;
const int kRight = 1;

void next()
{
    run -> ExecuteNextEvent();
    int eventID = run -> GetCurrentEventID();

    for (auto j : {kLeft,kRight})
    {
        for (auto i : {kViewXY,kViewZY})
        {
            auto tracker = htt -> GetTracker(i,j);
            e_info << "# of points = " << tracker -> GetNumImagePoints() << endl;
            auto paramPoint = tracker -> FindNextMaximumParamPoint();

            cvs[j] -> cd(1+i*2);
            tracker -> GetHistImageSpace(Form("hist_image_%d_%d_%d",eventID,i,j)) -> Draw("colz");
            auto graphData = tracker -> GetDataGraphImageSapce();
            graphData -> SetMarkerStyle(20);
            graphData -> SetMarkerSize(0.2);
            graphData -> Draw("samep");
            paramPoint -> GetBandInImageSpace(-1000,1000,-1000,1000) -> Draw("samel");

            cvs[j] -> cd(2+i*2);
            tracker -> GetHistParamSpace(Form("hist_param_%d_%d_%d",eventID,i,j)) -> Draw("colz");
            paramPoint -> GetRangeGraphInParamSpace() -> Draw("samel");
        }
    }
}

void debug_ht()
{
    run = new LKRun();
    run -> SetTag("debug");
    run -> AddPar("config_conv.mac");
    run -> AddPar("config_eve.mac");
    run -> AddParAfter("config_reco.mac");
    run -> AddInputFile("~/data/texat/reco/texat_0801.all.root");
    run -> AddDetector(new TexAT2());
    htt = new TTHTTrackingTask();
    run -> Add(htt);
    run -> Add(new LKEveTask);
    run -> SetEventCountForMessage(1);
    run -> Init();

    gStyle -> SetOptStat(0);
    cvs[0] = new TCanvas("cvs_0", "left, xy(top), zy(bottom)",1000,1000);
    cvs[1] = new TCanvas("cvs_1","right, xy(top), zy(bottom)",1000,1000);
    cvs[0] -> Divide(2,2);
    cvs[1] -> Divide(2,2);

    next();
    next();
}
