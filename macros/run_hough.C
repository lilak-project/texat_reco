#include "LKLogger.h"

void run_hough()
{
    lk_logger("data/run_hough.log");

    auto run = new LKRun();
    run -> AddDetector(new TexAT2());
    run -> AddInputFile("data/texat_0824.raw.master.8.ccf152e.root");
    run -> SetTag("hough");
    run -> Add(new TTHitFindingHoughTask());

    run -> Init();
    run -> Run();
}
