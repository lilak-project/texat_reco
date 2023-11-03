void draw_raw()
{
    auto run = new LKRun();
    run -> AddDetector(new TexAT2());
    run -> AddInputFile("data/texat_0824.raw.root");
    run -> Init();
    run -> Print();
}
