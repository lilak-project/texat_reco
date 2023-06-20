#include "LKLogger.h"

void run_eve()
{
    lk_logger("data/run_eve.log");

    auto run = new LKRun();
    run -> AddDetector(new TexAT2());
    run -> AddInputFile("data/texat_0824.raw.master.8.ccf152e.root");
    run -> SetTag("eve");
    run -> Add(new LKEveTask());

    run -> Init();
    run -> RunEvent(0);
}
