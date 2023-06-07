//#include "ATConversionTask.h"
//#include "ATConversionTask.cpp"

void test_conversion()
{
    auto run = new LKRun();
    run -> Add(new ATConversionTask());
    run -> AddPar("config_conversion.mac");
    run -> Init();
}
