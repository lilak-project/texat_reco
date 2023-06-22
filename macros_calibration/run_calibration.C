#include "TTCalibrationTask.cpp"
#include "TTCalibrationTask.h"

void run_calibration()
{
    auto run = new LKRun();
    run -> AddDetector(new TexAT2());

    run -> AddInputFile("../macros/data/texat_0824.raw.root");
    run -> AddPar("TTCalibrationTask.mac");
    run -> SetTag("calibration");
    run -> Add(new TTCalibrationTask());
    run -> SetOutputFile("./calibration_data.root");

    run -> Init();
    //run -> Run(0,10);
    run -> Run();
}
