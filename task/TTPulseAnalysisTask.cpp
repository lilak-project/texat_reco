#include "TTPulseAnalysisTask.h"
#include "TTEventHeader.h"
#include "GETChannel.h"
#include "LKHit.h"

ClassImp(TTPulseAnalysisTask);

TTPulseAnalysisTask::TTPulseAnalysisTask()
{
    fName = "TTPulseAnalysisTask";
}

bool TTPulseAnalysisTask::Init()
{
    lk_info << "Initializing TTPulseAnalysisTask" << std::endl;

    fDetector = (TexAT2 *) fRun -> GetDetector();
    fDetector -> InitChannelAnalyzer();

    fChannelArray = fRun -> GetBranchA("RawData");

    fHitArrayCenter = fRun -> RegisterBranchA("HitCenter","LKHit",100);
    fHitArrayLStrip = fRun -> RegisterBranchA("HitLStrip","LKHit",100);
    fHitArrayLChain = fRun -> RegisterBranchA("HitLChain","LKHit",100);
    fHitArrayRStrip = fRun -> RegisterBranchA("HitRStrip","LKHit",100);
    fHitArrayRChain = fRun -> RegisterBranchA("HitRChain","LKHit",100);
    fHitArrayOthers = fRun -> RegisterBranchA("HitOthers","LKHit",100);

    fITypeLStrip = fDetector -> GetTypeNumber(TexAT2::eType::kLeftStrip);
    fITypeRStrip = fDetector -> GetTypeNumber(TexAT2::eType::kRightStrip);
    fITypeLChain = fDetector -> GetTypeNumber(TexAT2::eType::kLeftChain);
    fITypeRChain = fDetector -> GetTypeNumber(TexAT2::eType::kRightChain);
    fITypeLCenter = fDetector -> GetTypeNumber(TexAT2::eType::kLowCenter);
    fITypeHCenter = fDetector -> GetTypeNumber(TexAT2::eType::kHighCenter);

    fEventHeaderHolder = fRun -> KeepBranchA("EventHeader");

    return true;
}

void TTPulseAnalysisTask::Exec(Option_t *option)
{
    fHitArrayCenter -> Clear("C");
    fHitArrayLStrip -> Clear("C");
    fHitArrayLChain -> Clear("C");
    fHitArrayRStrip -> Clear("C");
    fHitArrayRChain -> Clear("C");
    fHitArrayOthers -> Clear("C");

    auto eventHeader = (TTEventHeader*) fEventHeaderHolder -> At(0);
    if (eventHeader->IsGoodEvent()==false)
        return;

    int countHits = 0;
    int countHitCenter = 0;
    int countHitLStrip = 0;
    int countHitLChain = 0;
    int countHitRStrip = 0;
    int countHitRChain = 0;
    int countHitOthers = 0;

    double buffer[350];

    double xPos;
    double yPos;
    double zPos;
    double xErr;
    double yErr;
    double zErr;

    int numChannel = fChannelArray -> GetEntriesFast();
    for (int iChannel = 0; iChannel < numChannel; ++iChannel)
    {
        auto channel = (GETChannel *) fChannelArray -> At(iChannel);
        auto chDetType = channel -> GetDetType();
        auto cobo = channel -> GetCobo();
        auto asad = channel -> GetAsad();
        auto aget = channel -> GetAget();
        auto chan = channel -> GetChan();
        auto dchan = channel -> GetChan2();
        auto data = channel -> GetWaveformY();
        auto electronicsID = fDetector -> GetElectronicsID(cobo, asad, aget, chan);
        auto x = fDetector -> Getmmpx(asad, aget, dchan);
        auto z = fDetector -> Getmmpy(asad, aget, dchan);
        auto caac = cobo*10000 + asad*1000 + aget*100 + chan;

        if(electronicsID == 14) //fCsI
        {
            Int_t amplitude = 0;
            Int_t pedestal = 0;
            Int_t alpha = 0;
            FindMaximum(data, false, amplitude);
            fDetector -> CAACToGlobalPosition(cobo,asad,aget,chan, xPos,yPos,zPos,xErr,yErr,zErr);

            LKHit* hit = nullptr;
            hit = (LKHit*) fHitArrayOthers -> ConstructedAt(countHitOthers++);

            hit -> SetHitID(countHits);
            hit -> SetChannelID(caac);
            hit -> SetPosition(xPos,yPos,zPos);
            hit -> SetPositionError(xErr,yErr,zErr);
            hit -> SetCharge(amplitude);
            hit -> SetPedestal(pedestal);
            hit -> SetAlpha(alpha);

            countHits++;
        }
        if(cobo==1 && asad==1) continue;

        for (auto tb=0; tb<350; ++tb)
        {
            if( (cobo==1 && asad==0 && aget==0) || (cobo==2 && (aget==1||aget==2)) || (cobo==2 && asad==1 && aget==3) ) buffer[tb] = 4096 - double(data[tb]);
            else buffer[tb] = double(data[tb]);
        }
        auto ana = fDetector -> GetChannelAnalyzer(electronicsID);
        ana -> Analyze(buffer);

        fDetector -> CAACToGlobalPosition(cobo,asad,aget,chan, xPos,yPos,zPos,xErr,yErr,zErr);

        double alpha = 0;
        auto detType = fDetector -> GetType(cobo,asad,aget,chan);
        if(cobo==0) detType = fDetector -> GetType(cobo,asad,aget,dchan);
             if (detType==TexAT2::eType::kLeftStrip)  alpha = -100;
        else if (detType==TexAT2::eType::kRightStrip) alpha = +100;
        else if (detType==TexAT2::eType::kLeftChain)  alpha = -200;
        else if (detType==TexAT2::eType::kRightChain) alpha = +200;

        auto numRecoHits = ana -> GetNumHits();
        for (auto iHit=0; iHit<numRecoHits; ++iHit)
        {
            auto tb        = ana -> GetTbHit(iHit);
            auto amplitude = ana -> GetAmplitude(iHit);
            auto chi2NDF   = ana -> GetChi2NDF(iHit);
            auto ndf       = ana -> GetNDF(iHit);
            auto pedestal  = ana -> GetPedestal();
            if(detType==TexAT2::eType::kForwardSi || detType==TexAT2::eType::kCENSX6)
                if(tb>30) continue;

            LKHit* hit = nullptr;
                 if (detType==TexAT2::eType::kLowCenter ) hit = (LKHit*) fHitArrayCenter -> ConstructedAt(countHitCenter++);
            else if (detType==TexAT2::eType::kHighCenter) hit = (LKHit*) fHitArrayCenter -> ConstructedAt(countHitCenter++);
            else if (detType==TexAT2::eType::kLeftStrip ) hit = (LKHit*) fHitArrayLStrip -> ConstructedAt(countHitLStrip++);
            else if (detType==TexAT2::eType::kLeftChain ) hit = (LKHit*) fHitArrayLChain -> ConstructedAt(countHitLChain++);
            else if (detType==TexAT2::eType::kRightStrip) hit = (LKHit*) fHitArrayRStrip -> ConstructedAt(countHitRStrip++);
            else if (detType==TexAT2::eType::kRightChain) hit = (LKHit*) fHitArrayRChain -> ConstructedAt(countHitRChain++);
            else                                          hit = (LKHit*) fHitArrayOthers -> ConstructedAt(countHitOthers++);
            hit -> SetHitID(countHits);
            hit -> SetChannelID(caac);
            if(cobo==0) hit -> SetPosition(xPos,tb,zPos);
            else    	hit -> SetPosition(xPos,yPos,zPos);
            hit -> SetPositionError(xErr,yErr,zErr);
            hit -> SetCharge(amplitude);
            hit -> SetPedestal(pedestal);
            hit -> SetAlpha(alpha);

            countHits++;
        }
    }

    lk_info << "Found " << countHits << " hits" << endl;
}

bool TTPulseAnalysisTask::EndOfRun()
{
    return true;
}

void TTPulseAnalysisTask::FindMaximum(Int_t *data, Bool_t Positive, Int_t &MaxVal)
{
    if(Positive==false) for(Int_t i=0; i<512; i++) data[i] = 4096-data[i];
    MaxVal = -9999;
    for(Int_t i=0; i<512; i++) if(MaxVal<data[i]) MaxVal = data[i];

    //lazy type pedestal subtraction
    Int_t base = 0;
    for(Int_t i=10; i<20; i++) base += data[i];
    MaxVal -= base/10;
}
