void next() { LKRun::GetRun() -> RunSelectedEvent("EventHeader[0].IsGoodEvent()"); }
void write() { TTMicromegas::GetPlane() -> WriteCurrentChannel(); }

void run_eve()
{
    auto run = new LKRun();
    run -> SetTag("eve");
    run -> AddPar("config_eve.mac");
    run -> AddInputFile("~/data/texat/reco/texat_0801.all.root");
    run -> AddFriend("~/data/texat/reco/texat_0801.reco.root");
    auto tt = new TexAT2();
    run -> AddDetector(tt);
    run -> Add(new LKEveTask);
    run -> Init();

    next();
}
