#include "LKLogger.h"

void run_conversion()
{
    lk_logger("data/run_conversion.log");

    auto run = new LKRun();
    run -> AddDetector(new TexAT2());
    run -> Add(new TTRootConversionTask());
    run -> Add(new TTHoughTestTask());
    run -> AddPar("config_conversion.mac");

    run -> Init();
    run -> Run();
    run -> Print();
}
