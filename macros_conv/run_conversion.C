#include "LKLogger.h"

void run_conversion()
{
    lk_logger("data/run_conversion.log");

    auto run = new LKRun();
    run -> SetNumPrintMessage(10);
    run -> AddPar("config_conversion.mac");
    run -> AddDetector(new TexAT2());
    run -> Add(new TTRootConversionTask());

    run -> Init();
    run -> Run();
}
