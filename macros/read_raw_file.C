#include "LKLogger.h"

void read_raw_file()
{
    auto run = new LKRun();
    run -> AddInputFile("data/texat_0801.all.root"); lk_logger("log_after.log");
    //run -> AddInputFile("~/data/texat/reco/texat_0801.conv.root"); lk_logger("log_before.log");
    run -> Init();
    auto headerArray = run -> GetBranchA("EventHeader");
    auto channelArray = run -> GetBranchA("RawData");

    run -> RunSelectedEvent("EventHeader[0].fEventNumber==133146");
    //run -> RunSelectedEvent("true");

    auto numChannels = channelArray -> GetEntries();
    e_info << "Number of channels = " << numChannels << endl;
    e_info << "channel-ids: ";
    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel = (GETChannel*) channelArray -> At(iChannel);
        auto id = channel -> GetCAAC();
        e_cout << id << " ";
    }
    e_cout << endl;

    auto header = headerArray -> At(0); header -> Print();

    GETChannel *channel;
    channel = (GETChannel*) channelArray -> At(0); channel -> Print();
    channel -> Draw();
    //channel = (GETChannel*) channelArray -> At(1); channel -> Print();
    //channel = (GETChannel*) channelArray -> At(2); channel -> Print();
}
