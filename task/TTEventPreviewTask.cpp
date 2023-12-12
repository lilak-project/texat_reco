#include "TTEventPreviewTask.h"
#include "LKEventHeader.h"
#include "TTEventHeader.h"
#include "GETChannel.h"

ClassImp(TTEventPreviewTask);

TTEventPreviewTask::TTEventPreviewTask()
{
    fName = "TTEventPreviewTask";
}

bool TTEventPreviewTask::Init()
{
    lk_info << "Initializing TTEventPreviewTask" << std::endl;

    fDetector = (TexAT2 *) fRun -> GetDetector();
    fEventHeaderArray = fRun -> RegisterBranchA("EventHeader", "TTEventHeader", 1);
    fFrameHeaderArray = fRun -> GetBranchA("FrameHeader");
    fChannelArray = fRun -> GetBranchA("RawData");

    return true;
}


void TTEventPreviewTask::Exec(Option_t *option)
{
    int SiBLR = 0;
    int siLhit = 0;
    int siRhit = 0;
    int siChit = 0;
    int X6Lhit = 0;
    int X6Rhit = 0;

    auto numChannels = fChannelArray -> GetEntries();
    for(int iChannel=0; iChannel<numChannels; iChannel++)
    {
        auto channel = (GETChannel*) fChannelArray -> At(iChannel);
        auto cobo = channel -> GetCobo();
        auto asad = channel -> GetAsad();
        auto aget = channel -> GetAget();
        auto chan = channel -> GetChan();

        auto type = fDetector -> GetType(cobo,asad,aget,chan);
        auto typeNumber = fDetector -> GetTypeNumber(cobo,asad,aget,chan);
        auto detloc = fDetector -> GetDetLoc(cobo,asad,aget,chan);
        int dchan = 0;
             if (chan<11) dchan = chan;
        else if (chan>11 && chan<22) dchan = chan - 1;
        else if (chan>22 && chan<45) dchan = chan - 2;
        else if (chan>45 && chan<56) dchan = chan - 3;
        else if (chan>56) dchan = chan - 4;

        channel -> SetDetType(typeNumber);
        channel -> SetChan2(dchan);

        if(!(chan==11 || chan==22 || chan==45 || chan==56))
        {
            if(type==TexAT2::eType::kForwardSi)
            {
                     if(detloc==TexAT2::eDetLoc::kLeft)   siLhit++;
                else if(detloc==TexAT2::eDetLoc::kRight)  siRhit++;
                else if(detloc==TexAT2::eDetLoc::kCenter) siChit++;
            }
            else if(type==TexAT2::eType::kCENSX6)
            {
                     if(detloc==TexAT2::eDetLoc::kLeft)  X6Lhit++;
                else if(detloc==TexAT2::eDetLoc::kRight) X6Rhit++;
            }
        }
    }

    // not considered about center,,,
         if(siLhit==2) { if(siRhit<2 && X6Lhit<3 && X6Rhit<3) SiBLR=0; }
    else if(siRhit==2) { if(siLhit<2 && X6Lhit<3 && X6Rhit<3) SiBLR=1; }
    else if(X6Lhit==3) { if(siLhit<2 && siRhit<3 && X6Rhit<3) SiBLR=0; }
    else if(X6Rhit==3) { if(siLhit<2 && siRhit<3 && X6Lhit<3) SiBLR=1; }
    else if(siChit==2) SiBLR=2;
    else SiBLR = 9;

    auto frameHeader = (LKEventHeader *) fFrameHeaderArray -> At(0);
    auto eventHeader = (TTEventHeader *) fEventHeaderArray -> ConstructedAt(0);
    eventHeader -> SetEventNumber(frameHeader->GetEventNumber());
    eventHeader -> SetIsGoodEvent(frameHeader->IsGoodEvent());
    eventHeader -> SetSiLhit(siLhit);
    eventHeader -> SetSiRhit(siRhit);
    eventHeader -> SetSiChit(siChit);
    eventHeader -> SetX6Lhit(X6Lhit);
    eventHeader -> SetX6Rhit(X6Rhit);
    eventHeader -> SetSiBLR(SiBLR);

    lk_info << "Event no. = " << frameHeader->GetEventNumber() << ", number of channels = " << numChannels << endl;
    if (SiBLR==9)  {
        lk_info << "Bad Event! " << Form("siL: %d | siR: %d | siC: %d | X6L: %d | X6R: %d  ->  %d", siLhit, siRhit, siChit, X6Lhit, X6Rhit, SiBLR) << std::endl;
        eventHeader -> SetIsGoodEvent(false);
        fRun -> DoNotFillCurrentEvent();
    }
    else {
        lk_info << "Good Event!" << endl;
        eventHeader -> SetIsGoodEvent(true);
    }
}

bool TTEventPreviewTask::EndOfRun()
{
    return true;
}
