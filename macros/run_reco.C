#include "LKLogger.h"

void run_reco()
{
    lk_logger("data/run_reco.log");

    auto run = new LKRun();
    run -> SetTag("reco");
    run -> AddPar("config_reco.mac");
    run -> AddInputFile("texat_0801.27.conv.root");
    run -> AddDetector(new TexAT2());
    run -> Add(new TTEventPreviewTask());
    run -> Add(new TTPulseAnalysisTask());
    run -> Add(new TTHTTrackingTask());

    run -> Init();
    run -> Run();
}
