void test_read(TString inputFileName = "/home/ejungwoo/lilak/data/texat_0824.raw.master.2.db6a715.root")
{
    auto run = new LKRun();
    run -> AddInputFile(inputFileName);
    run -> SetTag("read");
    run -> Init();
    auto array = run -> GetBranchA("RawData");

    auto numEvents = run -> GetNumEvents();

    run -> GetEvent(0);

    auto numChannels = array -> GetEntries();
    cout << numEvents << " " << numChannels << endl;

    for (auto idxChannel : {0}) {
        auto channel = (MMChannel *) array -> At(idxChannel);
        channel -> Print();
    }
}

