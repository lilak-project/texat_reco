void run_conversion()
{
    auto run = new LKRun();
    run -> Add(new TTRootConversionTask());
    run -> Add(new TTHitFindingTask());
    run -> AddPar("config_conversion.mac");

    run -> Init();
    run -> Run();
}
