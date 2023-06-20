void test_read()
{
    auto run = new LKRun();
    run -> AddInputFile("data/texat_0824.raw.master.44.41dbcda.root");
    run -> SetTag("read");
    run -> Init();

    auto headerArray = run -> GetBranchA("EventHeader");
    auto channelArray = run -> GetBranchA("RawData");

    auto numEvents = run -> GetNumEvents();
    auto countGoodEvent = 0;
    for (auto iEvent=0; iEvent<numEvents; ++iEvent)
    {
        run -> GetEvent(iEvent);

        auto header = (TTEventHeader *) headerArray -> At(0);
        if (!header->GetIsGoodEvent())
            continue;
        header -> Print();

        auto numChannels = channelArray -> GetEntries();
        cout << numEvents << " " << numChannels << endl;

        for (auto iChannel=0; iChannel<numChannels; ++iChannel) {
            auto channel = (MMChannel *) channelArray -> At(iChannel);
            channel -> Print();
        }
        break;
    }
}

