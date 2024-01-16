void next() {
    LKRun::GetRun() -> RunSelectedEvent("EventHeader[0].IsGoodEvent()");
    //LKRun::GetRun() -> ExecuteNextEvent();
}
void write() { TTMicromegas::GetPlane() -> WriteCurrentChannel(); }

void run_eve()
{
    auto run = new LKRun();
    run -> SetTag("eve");
    run -> AddPar("config_eve.mac");
    run -> AddInputFile("texat_0801.27.conv.root");
    run -> AddFriend("texat_0801.reco.root");
    //run -> AddInputFile("/home/ejungwoo/lilak/data/texat_0801.reco.root");
    auto tt = new TexAT2();
    run -> AddDetector(tt);
    run -> Add(new LKEveTask);
    run -> Init();

    LKWindowManager::GetWindowManager() -> FixCanvasPosition();
    next();
    next();
}
