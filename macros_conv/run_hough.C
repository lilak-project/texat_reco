#include "LKLogger.h"

void run_hough()
{
    lk_logger("data/run_hough.log");

    auto run = new LKRun();
    run -> AddPar("config_hough.mac");
    run -> AddDetector(new TexAT2());

    run -> AddInputFile("data/texat_0824.raw.root");
    run -> SetTag("hough");
    run -> SetNumPrintMessage(100000);

    //run -> Add(new TTHitFindingHoughTask());
    run -> Add(new TTHoughTestTask());

    run -> Init();
    run -> Run(0,30);
}
