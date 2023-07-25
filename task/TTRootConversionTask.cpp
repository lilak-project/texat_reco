#include "TTRootConversionTask.h"
#include "MMChannel.h"

ClassImp(TTRootConversionTask);

TTRootConversionTask::TTRootConversionTask()
{
    fName = "TTRootConversionTask";
}

bool TTRootConversionTask::Init()
{
    // Put intialization todos here which are not iterative job though event
    lk_info << "Initializing TTRootConversionTask" << std::endl;

    lk_debug << endl;
    fDetector = (TexAT2 *) fRun -> GetDetector();
    lk_debug << endl;

    fInputFileName = fPar -> GetParString("TTRootConversionTask/inputFileName");
    lk_debug << endl;

    fEventHeaderArray = fRun -> RegisterBranchA("EventHeader", "TTEventHeader", 1);
    fChannelArray = fRun -> RegisterBranchA("RawData", "MMChannel", 200);
    lk_debug << endl;

    lk_info << "Input file is " << fInputFileName << endl;
    fInputFile = new TFile(fInputFileName, "read");
    fInputTree = (TTree*) fInputFile -> Get("TEvent");
    lk_debug << endl;

    //SetBranch()
    {
        fInputTree -> SetBranchAddress("mmMul",&fmmMult);
        fInputTree -> SetBranchAddress("mmHit",&fmmHit);
        fInputTree -> SetBranchAddress("mmEventIdx",&fmmEventIdx);
        fInputTree -> SetBranchAddress("mmFrameNo",fmmFrameNo);
        fInputTree -> SetBranchAddress("mmDecayNo",fmmDecayNo);
        fInputTree -> SetBranchAddress("mmCobo",fmmCobo);
        fInputTree -> SetBranchAddress("mmAsad",fmmAsad);
        fInputTree -> SetBranchAddress("mmAget",fmmAget);
        fInputTree -> SetBranchAddress("mmChan",fmmChan);
        fInputTree -> SetBranchAddress("mmTime",fmmTime);
        fInputTree -> SetBranchAddress("mmEnergy",fmmEnergy);
        fInputTree -> SetBranchAddress("mmWaveformX",fmmWaveformX);
        fInputTree -> SetBranchAddress("mmWaveformY",fmmWaveformY);
        Int_t fNumEvents = fInputTree -> GetEntries();
        //Int_t fNumEvents = 2;
        fRun -> SetNumEvents(fNumEvents);
    }
    lk_debug << endl;

    return true;
}


void TTRootConversionTask::Exec(Option_t *option)
{
    fInputTree -> GetEntry(fRun->GetCurrentEventID());

    fEventHeaderArray -> Clear("C");
    fChannelArray -> Clear("C");

    Int_t SiBLR = 0;
    Int_t siLhit = 0;
    Int_t siRhit = 0;
    Int_t siChit = 0;
    Int_t X6Lhit = 0;
    Int_t X6Rhit = 0;
    for(Int_t iChannel=0; iChannel<fmmMult; iChannel++)
    {
        if(!(fmmChan[iChannel]==11 || fmmChan[iChannel]==22 || fmmChan[iChannel]==45 || fmmChan[iChannel]==56))
        {
            auto type = fDetector -> GetType(fmmCobo[iChannel],fmmAsad[iChannel],fmmAget[iChannel],fmmChan[iChannel]);
            auto detloc = fDetector -> GetDetLoc(fmmCobo[iChannel],fmmAsad[iChannel],fmmAget[iChannel],fmmChan[iChannel]);
            if(type==TexAT2::eType::kForwardSi)
            {
                     if(detloc==TexAT2::eDetLoc::kLeft)        siLhit++;
                else if(detloc==TexAT2::eDetLoc::kRight)       siRhit++;
                else if(detloc==TexAT2::eDetLoc::kCenterFront) siChit++;
            }
            else if(type==TexAT2::eType::kCENSX6)
            {
                     if(detloc==TexAT2::eDetLoc::kBottomLeftX6)  X6Lhit++;
                else if(detloc==TexAT2::eDetLoc::kBottomRightX6) X6Rhit++;
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

    auto eventHeader = (TTEventHeader *) fEventHeaderArray -> ConstructedAt(0);
    eventHeader -> SetSiLhit(siLhit);
    eventHeader -> SetSiRhit(siRhit);
    eventHeader -> SetSiChit(siChit);
    eventHeader -> SetX6Lhit(X6Lhit);
    eventHeader -> SetX6Rhit(X6Rhit);
    eventHeader -> SetSiBLR(SiBLR);

    if (SiBLR==9)  {
        lk_info << "TTRootConversionTask: Bad Event! "
            << Form("siL: %d | siR: %d | siC: %d | X6L: %d | X6R: %d  ->  %d", siLhit, siRhit, siChit, X6Lhit, X6Rhit, SiBLR)
            << std::endl;
        eventHeader -> SetIsGoodEvent(false);
        //return;
    }

    eventHeader -> SetIsGoodEvent(true);

    for (int iChannel = 0; iChannel < fmmMult; ++iChannel)
    {
        int chan = fmmChan[iChannel];
        int dchan = 0;

             if(           chan<11) dchan = chan;
        else if(chan>11 && chan<22) dchan = chan - 1;
        else if(chan>22 && chan<45) dchan = chan - 2;
        else if(chan>45 && chan<56) dchan = chan - 3;
        else if(chan>56           ) dchan = chan - 4;

        //auto type = fDetector -> GetType(fmmCobo[iChannel],fmmAsad[iChannel],fmmAget[iChannel],fmmChan[iChannel]); // XXX
        //if (type==TexAT2::eType::kLeftStrip) ;
        //if (type==TexAT2::eType::kRightStrip) ;
        //if (type==TexAT2::eType::kLeftChain) ;
        //if (type==TexAT2::eType::kRightChain) ;
        //if (type==TexAT2::eType::kLowCenter) ;
        //if (type==TexAT2::eType::kHighCenter) ;
        //if (type==TexAT2::eType::kForwardSi) ;
        //if (type==TexAT2::eType::kForwardCsI) ;
        //if (type==TexAT2::eType::kMMJr) ;
        //if (type==TexAT2::eType::kCENSX6) ;
        //if (type==TexAT2::eType::kCENSCsI) ;
        //if (type==TexAT2::eType::kExternal) ;

        auto channel = (MMChannel *) fChannelArray -> ConstructedAt(iChannel);
        channel -> SetEventIdx(fmmEventIdx);
        channel -> SetFrameNo(fmmFrameNo[iChannel]);
        channel -> SetDecayNo(fmmDecayNo[iChannel]);
        channel -> SetCobo(fmmCobo[iChannel]);
        channel -> SetAsad(fmmAsad[iChannel]);
        channel -> SetAget(fmmAget[iChannel]);
        channel -> SetChan(fmmChan[iChannel]);
        channel -> SetDChan(dchan);
        channel -> SetTime(fmmTime[iChannel]);
        channel -> SetEnergy(fmmEnergy[iChannel]);
        channel -> SetWaveformX(fmmWaveformX[iChannel]);
        channel -> SetWaveformY(fmmWaveformY[iChannel]);

        //if (iChannel==0 || iChannel==1) channel -> Print();
    }

    //lk_debug << fmmMult << " " << fmmChan[10] << " " << fmmWaveformY[10][0] << endl;

    lk_info << "TTRootConversionTask found " << fChannelArray -> GetEntriesFast() << " channels" << std::endl;
}

bool TTRootConversionTask::EndOfRun()
{
    return true;
}
