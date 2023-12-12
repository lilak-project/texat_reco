#include "LKLogger.h"

void run_reco()
{
    lk_logger("data/run_reco.log");

    auto run = new LKRun();
    run -> SetTag("reco");
    run -> AddPar("config_conv.mac");
    run -> AddParAfter("config_reco.mac");
    run -> AddInputFile("~/data/texat/reco/texat_0801.conv.root");
    //run -> AddInputFile("~/data/texat/reco/texat_0801.all.root");
    //run -> AddInputFile("~/data/texat/conv/run_0801.root");
    run -> AddDetector(new TexAT2());
    run -> Add(new TTHTTrackingTask());

    run -> Init();
    run -> Run();
}
