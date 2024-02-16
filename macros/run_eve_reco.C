#include "LKLogger.h"

TCanvas *fCvs = nullptr;
TCanvas *fCvs1 = nullptr;
TTHTTrackingTask* fTrackFindingTask;
TTHTBeamTrackingTask* fBeamFindingTask;
TClonesArray *fTrackArray = nullptr;
TClonesArray *fBeamArray  = nullptr;

void debug_htt() 
{
    const int kViewXY = 0;
    const int kViewZY = 1;
    const int kLeft = 0;
    const int kRight = 1;

    int eventID = LKRun::GetRun() -> GetCurrentEventID();

    TString name0, title0;
    if (fCvs!=nullptr)
    {
        e_cout << endl;
        e_info << "track" << endl;
        for (auto j : {kLeft,kRight})
        {
            if (j==kLeft) name0 = "Left";
            if (j==kRight) name0 = "Right";
            for (auto i : {kViewXY,kViewZY})
            {
                if (i==kViewXY) title0 = "x;y";
                if (i==kViewZY) title0 = "z;y";
                auto tracker = fTrackFindingTask -> GetTracker(i,j);
                auto paramPoint = tracker -> GetCurrentMaximumParamPoint();

                tracker -> Draw(fCvs -> cd(j*4+1+i*2), fCvs -> cd(j*4+2+i*2), paramPoint);
            }
        }
    }

    e_cout << endl;
    e_info << "beam" << endl;
    {
        name0 = "Beam";
        title0 = "z;y";
        auto tracker = fBeamFindingTask -> GetTracker();
        auto paramPoint = tracker -> GetCurrentMaximumParamPoint();

        tracker -> Draw(fCvs1->cd(1), fCvs1->cd(2), paramPoint);
    }
}

void next(int eventID=-1)
{
    if (eventID<0)
        LKRun::GetRun() -> ExecuteNextEvent();
    else
        LKRun::GetRun() -> ExecuteEvent(eventID);

    //LKRun::GetRun() -> PrintEvent("Track:BeamTrack");
    debug_htt();
}

void run_eve_reco()
{
    lk_logger("data/run_eve_reco.log");

    auto run = new LKRun();
    run -> SetTag("eve_reco");
    run -> AddPar("config_reco.mac");
    run -> AddPar("config_eve.mac");
    run -> AddInputFile("texat_0801.27.conv.root");
    run -> SetDivision(27);
    run -> AddDetector(new TexAT2());
    run -> Add(new TTEventPreviewTask());
    run -> Add(new TTPulseAnalysisTask());
    fTrackFindingTask = new TTHTTrackingTask();
    run -> Add(fTrackFindingTask);
    fBeamFindingTask = new TTHTBeamTrackingTask();
    run -> Add(fBeamFindingTask);
    run -> Add(new LKEveTask);
    run -> Init();

    fTrackArray = run -> GetBranchA("Track");
    fBeamArray = run -> GetBranchA("BeamTrack");

    gStyle -> SetOptStat(0);

    fCvs = new TCanvas("cvs_0","",2400,1200);
    fCvs -> Divide(4,2);

    fCvs1 = new TCanvas("cvs_1","",1500,750);
    fCvs1 -> Divide(2,1);

    //next(3);
    next(104);
    //next(178);
    //next(21);
}
