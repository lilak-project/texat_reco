#include "LKLogger.h"

void run_all()
{
    lk_logger("data/run_all.log");

    auto run = new LKRun();
    run -> SetRunName("texat",801,"all");
    run -> SetDataPath("data");

    run -> AddPar("config_all.mac");
    run -> AddDetector(new TexAT2());

    run -> SetEventTrigger(new LKMFMConversionTask());
    run -> Add(new TTEventPreviewTask());
    run -> Add(new TTPulseAnalysisTask());
    run -> Add(new TTHTTrackingTask());

    run -> Init();

    run -> SetEventCountForMessage(200);
    run -> Run();
}
