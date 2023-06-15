void run_conversion()
{
    auto run = new LKRun();
    run -> Add(new TTRootConversionTask());
    run -> AddPar("config_conversion.mac");
    run -> Print();
    run -> Init();
    run -> Print();
    run -> Run();
}
