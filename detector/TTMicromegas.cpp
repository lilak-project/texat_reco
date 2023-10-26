#include "TTMicromegas.h"
#include "MMChannel.h"
#include "TPaveText.h"
#include "TexAT2.h"
#include "TTEventHeader.h"

ClassImp(TTMicromegas);

TTMicromegas* TTMicromegas::fInstance = nullptr;
TTMicromegas* TTMicromegas::GetMMCC() { return fInstance; }

TTMicromegas::TTMicromegas()
{
    fInstance = this;
    fName = "TTMicromegas";
    fAxis1 = LKVector3::kX;
    fAxis2 = LKVector3::kZ;

}

bool TTMicromegas::Init()
{
    e_info << "Initializing TTMicromegas" << std::endl;

    fHistChannelChain = new TH1D("hist_channel_buffer_chain","channel buffer;time-bucket;charge",360,0,360);
    fHistChannelStrip = new TH1D("hist_channel_buffer_strip","channel buffer;time-bucket;charge",360,0,360);
    for (auto histChannel : {fHistChannelChain,fHistChannelStrip}) {
        histChannel -> SetStats(0);
        histChannel -> GetXaxis() -> SetLabelSize(0.065);
        histChannel -> GetYaxis() -> SetLabelSize(0.065);
        histChannel -> GetXaxis() -> SetTitleSize(0.065);
        histChannel -> GetYaxis() -> SetTitleSize(0.065);
        histChannel -> GetXaxis() -> SetTitleOffset(1.20);
        //histChannel -> GetYaxis() -> SetTitleOffset(0.68);
        histChannel -> GetYaxis() -> SetTitleOffset(0.80);
    }

    int cobo, asad, aget, chan, dumi;
    double x0, x1, x2, z0, z1, z2;
    TString mapFileNameChain;
    TString mapFileNameStrip;
    if (fPar!=nullptr) {
        mapFileNameChain = fPar -> GetParString("TTMicromegas/map_position_center_chain");
        mapFileNameStrip = fPar -> GetParString("TTMicromegas/map_position_center_strip");
    }
    if (mapFileNameChain.IsNull()) mapFileNameChain = "position_channels_center_and_chain.txt";
    if (mapFileNameStrip.IsNull()) mapFileNameStrip = "position_channels_center_and_strip.txt";

    fHistPlaneChainFrame = new TH2Poly("frame_TexATv2_plane_center_chain","TexAT2 Micromegas Center and Chain;x (mm);z (beam-axis)",-130,130,180,420);
    fHistPlaneChainFrame -> SetStats(0);
    fHistPlaneChainFrame -> SetLineColorAlpha(kBlack,0.5);
    fHistPlaneChain = new TH2Poly("hist_TexATv2_plane_center_chain","TexAT2 Micromegas Center and Chain;x (mm);z (beam-axis)",-130,130,180,420);
    fHistPlaneChain -> SetStats(0);
    ifstream fileCC(mapFileNameChain);
    while (fileCC >> cobo >> asad >> aget >> chan >> x0 >> x1 >> x2 >> z0 >> z1 >> z2) {
        if (fXMin > x1) fXMin = x1;
        if (fXMax < x1) fXMin = x1;
        if (fXMin > x2) fXMin = x2;
        if (fXMax < x2) fXMin = x2;
        if (fZMin > z1) fXMin = z1;
        if (fZMax < z1) fXMin = z1;
        if (fZMin > z2) fXMin = z2;
        if (fZMax < z2) fXMin = z2;
        auto bin = fHistPlaneChainFrame -> AddBin(x1,z1,x2,z2);
        fHistPlaneChain -> AddBin(x1,z1,x2,z2);
        int caac = cobo*10000 + asad*1000 + aget*100 + chan;
        fMapCAACToBinChain.insert(std::pair<int, int>(caac,bin));
        fMapBinToCAACChain.insert(std::pair<int, int>(bin,caac));
        fMapBinToX1Chain.insert(std::pair<int, double>(bin,x1));
        fMapBinToX2Chain.insert(std::pair<int, double>(bin,x2));
        fMapBinToZ1Chain.insert(std::pair<int, double>(bin,z1));
        fMapBinToZ2Chain.insert(std::pair<int, double>(bin,z2));
    }

    fHistPlaneStripFrame = new TH2Poly("frame_TexATv2_plane_center_strip","TexAT2 Micromegas Center and Strip;x (mm);z (beam-axis)",-130,130,180,420);
    fHistPlaneStripFrame -> SetStats(0);
    fHistPlaneChainFrame -> SetLineColorAlpha(kGray,0.5);
    fHistPlaneStrip = new TH2Poly("hist_TexATv2_plane_center_strip","TexAT2 Micromegas Center and Strip;x (mm);z (beam-axis)",-130,130,180,420);
    fHistPlaneStrip -> SetStats(0);
    ifstream fileCS(mapFileNameStrip);
    while (fileCS >> cobo >> asad >> aget >> chan >> x0 >> x1 >> x2 >> z0 >> z1 >> z2) {
        if (fXMin > x1) fXMin = x1;
        if (fXMax < x1) fXMin = x1;
        if (fXMin > x2) fXMin = x2;
        if (fXMax < x2) fXMin = x2;
        if (fZMin > z1) fXMin = z1;
        if (fZMax < z1) fXMin = z1;
        if (fZMin > z2) fXMin = z2;
        if (fZMax < z2) fXMin = z2;
        auto bin = fHistPlaneStripFrame -> AddBin(x1,z1,x2,z2);
        fHistPlaneStrip -> AddBin(x1,z1,x2,z2);
        int caac = cobo*10000 + asad*1000 + aget*100 + chan;
        fMapCAACToBinStrip.insert(std::pair<int, int>(caac,bin));
        fMapBinToCAACStrip.insert(std::pair<int, int>(bin,caac));
        fMapBinToX1Strip.insert(std::pair<int, double>(bin,x1));
        fMapBinToX2Strip.insert(std::pair<int, double>(bin,x2));
        fMapBinToZ1Strip.insert(std::pair<int, double>(bin,z1));
        fMapBinToZ2Strip.insert(std::pair<int, double>(bin,z2));
    }

    SetDataFromBranch();

    fGraphChannelBoundaryChain = new TGraph();
    fGraphChannelBoundaryChain -> SetName("graphChannelBoundaryChain");
    fGraphChannelBoundaryChain -> SetLineColor(kRed);
    fGraphChannelBoundaryChain -> SetLineWidth(2);

    fGraphChannelBoundaryStrip = new TGraph();
    fGraphChannelBoundaryChain -> SetName("graphChannelBoundaryStrip");
    fGraphChannelBoundaryStrip -> SetLineColor(kRed);
    fGraphChannelBoundaryStrip -> SetLineWidth(2);

    return true;
}

void TTMicromegas::Clear(Option_t *option)
{
    LKPadPlane::Clear(option);
}

void TTMicromegas::Print(Option_t *option) const
{
    e_info << "TTMicromegas" << std::endl;
}

bool TTMicromegas::IsInBoundary(Double_t x, Double_t z)
{
    if (x>fXMin && x<fXMax && z>fZMin && z<fZMax)
        return true;
    return true;
}

Int_t TTMicromegas::FindChannelID(Double_t x, Double_t y)
{
    //return fHistPlaneChain -> FindBin(x,y);
    return -1;
}

Int_t TTMicromegas::FindChannelID(Int_t section, Int_t row, Int_t layer)
{
    // example find id
    // int id = 10000*section + 100*row + layer;
    // return id;
    return -1;
}

TCanvas* TTMicromegas::GetCanvas(Option_t *option)
{
    if (fCanvas==nullptr) {
        fCanvas = new TCanvas("TTMicromegas","Micromegas",fScale*1100,fScale*700);
        auto pad1 = new TPad("pad1","",0,230./700,0.5,1);
        pad1 -> SetMargin(0.12,0.15,0.1,0.1);
        pad1 -> SetNumber(1);
        pad1 -> AddExec("ex", "TTMicromegas::MouseClickEventChain()");
        pad1 -> Draw();
        auto pad2 = new TPad("pad2","",0,0,0.5,230./700);
        pad2 -> SetMargin(0.12,0.05,0.20,0.12);
        pad2 -> SetNumber(2);
        pad2 -> Draw();
        auto pad3 = new TPad("pad1","",0.5,230./700,1,1);
        pad3 -> SetMargin(0.12,0.15,0.1,0.1);
        pad3 -> SetNumber(3);
        pad3 -> AddExec("ex", "TTMicromegas::MouseClickEventStrip()");
        pad3 -> Draw();
        auto pad4 = new TPad("pad2","",0.5,0,1,230./700);
        pad4 -> SetMargin(0.12,0.05,0.20,0.12);
        pad4 -> SetNumber(4);
        pad4 -> Draw();
        fCanvas -> Modified();
        fCanvas -> Update();
    }
    return fCanvas;
}

TH2* TTMicromegas::GetHist(Option_t *option)
{
    return (TH2Poly *) fHistPlaneChain;
    //return (TH2Poly *) fHistPlaneStrip;
}

bool TTMicromegas::SetDataFromBranch()
{
    if (fRun==nullptr)
        return false;

    fEventHeadherHolder = fRun -> GetBranchA("EventHeader");
    fBufferArray = fRun -> GetBranchA("RawData");
    fHitCenterArray = fRun -> GetBranchA("HitCenter");
    fHitLChainArray = fRun -> GetBranchA("HitLChain");
    fHitRChainArray = fRun -> GetBranchA("HitRChain");
    fHitLStripArray = fRun -> GetBranchA("HitLStrip");
    fHitRStripArray = fRun -> GetBranchA("HitRStrip");
    return true;
}

void TTMicromegas::FillDataToHist()
{
    if (fHitCenterArray==nullptr)
        return;

    fHistPlaneChain -> Reset("ICES");
    fHistPlaneStrip -> Reset("ICES");

    Long64_t eventIDFromRun = -1;
    int evenIDFromHeader = -1;
    if (fRun!=nullptr) eventIDFromRun = fRun -> GetCurrentEventID();
    if (fEventHeadherHolder!=nullptr) {
        auto eventHeader = (TTEventHeader*) fEventHeadherHolder -> At(0);
        if (eventHeader!=nullptr) {
            evenIDFromHeader = eventHeader -> GetEventNumber();
        }
    }
    if (eventIDFromRun>=0 && evenIDFromHeader>=0) {
        fHistPlaneChain -> SetTitle("Event %d (%d)",eventIDFromRun,evenIDFromHeader);
        fHistPlaneStrip -> SetTitle("Event %d (%d)",eventIDFromRun,evenIDFromHeader);
    }
    else if (eventIDFromRun>=0 && evenIDFromHeader<0) {
        fHistPlaneChain -> SetTitle("Event %d",eventIDFromRun);
        fHistPlaneStrip -> SetTitle("Event %d",eventIDFromRun);
    }

    for (auto hitArray : {fHitCenterArray,fHitLChainArray,fHitRChainArray})
    {
        auto numHits = hitArray -> GetEntries();
        for (auto iHit=0; iHit<numHits; ++iHit)
        {
            auto hit = (LKHit *) hitArray -> At(iHit);
            auto position = hit -> GetPosition();
            fHistPlaneChain -> Fill(position.X(), position.Z(), hit->GetCharge());
        }
    }

    for (auto hitArray : {fHitCenterArray,fHitLStripArray,fHitRStripArray})
    {
        auto numHits = hitArray -> GetEntries();
        for (auto iHit=0; iHit<numHits; ++iHit)
        {
            auto hit = (LKHit *) hitArray -> At(iHit);
            auto position = hit -> GetPosition();
            fHistPlaneStrip -> Fill(position.X(), position.Z(), hit->GetCharge());
        }
    }

    SelectAndDrawChannelChain();
    SelectAndDrawChannelStrip();
}


void TTMicromegas::SelectAndDrawChannel(bool isChain, Int_t bin)
{
    if (isChain) lk_info << "SelectAndDrawChannel (Chain) " << bin << endl;
    else         lk_info << "SelectAndDrawChannel (Strip) " << bin << endl;

    auto cvsPlane = fCanvas -> cd(3);
    auto cvsChannel = fCanvas -> cd(4);
    if (isChain) {
        cvsPlane = fCanvas -> cd(1);
        cvsChannel = fCanvas -> cd(2);
    }

    bool existHitArray = false;
    bool existBufferArray = (fBufferArray!=nullptr);
    if (isChain) existHitArray = (fHitCenterArray!=nullptr&&fHitLChainArray!=nullptr&&fHitRChainArray!=nullptr);
    else         existHitArray = (fHitCenterArray!=nullptr&&fHitLStripArray!=nullptr&&fHitRStripArray!=nullptr);

    vector<TClonesArray*> hitArrayList = {fHitCenterArray};
    if (isChain) {
        hitArrayList.push_back(fHitLChainArray);
        hitArrayList.push_back(fHitRChainArray);
    }
    else {
        hitArrayList.push_back(fHitLStripArray);
        hitArrayList.push_back(fHitRStripArray);
    }
    if (bin<0) {
        if (existHitArray) {
            LKHit *hit = nullptr;
            for (auto hitArray : hitArrayList) {
                auto numHits = hitArray -> GetEntries();
                for (auto iHit=0; iHit<numHits; ++iHit)
                    hit = (LKHit *) hitArray -> At(iHit);
            }
            if (hit==nullptr)
                return;
            auto caac = hit -> GetChannelID();
            if (isChain) bin = fMapCAACToBinChain[caac];
            else         bin = fMapCAACToBinStrip[caac];
        }
    }

    if (bin<0)
        return;

    double x1,x2,z1,z2;
    TGraph* graphBoundary;
    if (isChain) {
        x1 = fMapBinToX1Chain[bin];
        x2 = fMapBinToX2Chain[bin];
        z1 = fMapBinToZ1Chain[bin];
        z2 = fMapBinToZ2Chain[bin];
        graphBoundary = fGraphChannelBoundaryChain;
    }
    else {
        x1 = fMapBinToX1Strip[bin];
        x2 = fMapBinToX2Strip[bin];
        z1 = fMapBinToZ1Strip[bin];
        z2 = fMapBinToZ2Strip[bin];
        graphBoundary = fGraphChannelBoundaryStrip;
    }
    double x0 = (x1 + x2)/2.;
    double z0 = (z1 + z2)/2.;
    graphBoundary -> Set(0);
    graphBoundary -> SetPoint(0,x1,z1);
    graphBoundary -> SetPoint(1,x2,z1);
    graphBoundary -> SetPoint(2,x2,z2);
    graphBoundary -> SetPoint(3,x1,z2);
    graphBoundary -> SetPoint(4,x1,z1);
    graphBoundary -> SetLineColor(kBlue);

    cvsPlane -> cd();
    graphBoundary -> Draw("samel");

    int caac = 0;
    if (isChain) caac = fMapBinToCAACChain[bin];
    else         caac = fMapBinToCAACStrip[bin];
    
    TH1D* histChannel = nullptr;
    if (isChain) histChannel = fHistChannelChain;
    else         histChannel = fHistChannelStrip;
    histChannel -> Reset();
    histChannel -> SetTitle(Form("CAAC=%d, position=(%.2f, %.2f)",caac,x0,z0));
    
    cvsChannel -> Modified();
    cvsChannel -> Update();

    if (!existBufferArray)
        return;

    MMChannel* channel = nullptr;
    auto numChannels = fBufferArray -> GetEntries();
    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel0 = (MMChannel* ) fBufferArray -> At(iChannel);
        if (caac==channel0->GetCAAC()) {
            channel = channel0;
            break;
        }
    }

    if (channel==nullptr) {
        return;
    }

    lk_debug << endl;
    auto buffer = channel -> GetWaveformY();
    for (auto tb=0; tb<360; ++tb)
        histChannel -> SetBinContent(tb+1,buffer[tb]);
    cvsChannel -> cd();
    histChannel -> Draw();
    auto texat = (TexAT2*) fDetector;
    //if (texat==nullptr) ...;
    if (existHitArray)
    {
        int countHitsInChannel = 0;
        for (auto hitArray : hitArrayList) {
            auto numHits = hitArray -> GetEntries();
            lk_debug << numHits << endl;
            for (auto iHit=0; iHit<numHits; ++iHit)
            {
                auto hit = (LKHit *) hitArray -> At(iHit);
                if (caac==hit -> GetChannelID())
                {
                    ++countHitsInChannel;
                    auto caac0 = caac;
                    auto cobo = int(caac0/10000); caac0 -= cobo*10000;
                    auto asad = int(caac0/1000); caac0 -= asad*1000;
                    auto aget = int(caac0/100); caac0 -= aget*100;
                    auto chan = caac0;
                    lk_debug << caac << endl;
                    if (texat!=nullptr)
                    {
                        auto electronicsID = texat -> GetElectronicsID(cobo,asad,aget,chan);
                        auto pulse = texat -> GetChannelAnalyzer(electronicsID) -> GetPulse();
                        lk_debug << electronicsID << " " << hit->GetY() << " " << hit -> GetCharge() << endl;
                        auto graph = pulse -> GetPulseGraph(hit->GetY(), hit -> GetCharge());
                        cvsChannel -> cd();
                        graph -> Draw("samelx");
                    }
                    graphBoundary -> SetLineColor(kRed);
                    //cvsPlane -> cd();
                    //graphBoundary -> Draw("samel");
                }
            }
            //histChannel -> Draw();
            histChannel -> SetTitle(Form("CAAC=%d, eID=%d, position=(%.2f, %.2f), #Hits=%d",caac,electronicsID,x0,z0,countHitsInChannel));
        }
    }
    lk_debug << endl;

    fCanvas -> Modified();
    fCanvas -> Update();
}

void TTMicromegas::DrawFrame(Option_t *option)
{
    ;
}

void TTMicromegas::Draw(Option_t *option)
{
    //SetDataFromBranch();
    FillDataToHist();

    auto cvs = GetCanvas();

    cvs -> cd(1);
    if (fHistPlaneChain->GetEntries()==0)
        fHistPlaneChainFrame -> Draw();
    else {
        fHistPlaneChainFrame -> SetMinimum(0.1);
        fHistPlaneChain -> SetMinimum(0.1);
        fHistPlaneChain -> Draw("same colz");
    }

    cvs -> cd(2);
    fHistChannelChain -> Draw();
    cvs -> cd(2) -> Modified();
    cvs -> cd(2) -> Update();
    auto ttt1 = (TPaveText*) (cvs->cd(2)->GetListOfPrimitives()) -> FindObject("title");
    ttt1 -> SetTextSize(0.065);
    ttt1 -> SetTextAlign(12);
    cvs -> cd(2) -> Modified();
    cvs -> cd(2) -> Update();

    cvs -> cd(3);
    if (fHistPlaneStrip->GetEntries()==0)
        fHistPlaneStripFrame -> Draw();
    else {
        fHistPlaneStripFrame -> Draw("");
        fHistPlaneStrip -> SetMinimum(0.1);
        fHistPlaneStrip -> Draw("same colz");
    }

    cvs -> cd(4);
    fHistChannelStrip -> Draw();
    cvs -> cd(4) -> Modified();
    cvs -> cd(4) -> Update();
    auto ttt2 = (TPaveText*) (cvs->cd(4)->GetListOfPrimitives()) -> FindObject("title");
    ttt2 -> SetTextSize(0.065);
    ttt2 -> SetTextAlign(32);
    cvs -> cd(4) -> Modified();
    cvs -> cd(4) -> Update();
}

void TTMicromegas::MouseClickEventChain()
{
    if (gPad==nullptr)
        return;

    TObject* select = gPad -> GetCanvas() -> GetClickSelected();
    if (select == nullptr)
        return;

    bool isNotH2 = !(select -> InheritsFrom(TH1::Class()));
    //bool isNotGraph = !(select -> InheritsFrom(TGraph::Class()));
    //if (isNotH2 && isNotGraph)
    if (isNotH2)
        return;

    int xEvent = gPad -> GetEventX();
    int yEvent = gPad -> GetEventY();
    int xAbs = gPad -> AbsPixeltoX(xEvent);
    int yAbs = gPad -> AbsPixeltoY(yEvent);
    double xOnClick = gPad -> PadtoX(xAbs);
    double yOnClick = gPad -> PadtoY(yAbs);

    TH2* hist = dynamic_cast<TH2*> (select);
    int binCurr = hist -> FindBin(xOnClick, yOnClick);
    int binLast = gPad -> GetUniqueID();
    if (binCurr==binLast)
        return;

    gPad -> SetUniqueID(binCurr);
    gPad -> GetCanvas() -> SetClickSelected(nullptr);

    if (binCurr<=0)
        return;

    TTMicromegas::GetMMCC() -> SelectAndDrawChannelChain(binCurr);
}

void TTMicromegas::MouseClickEventStrip()
{
    if (gPad==nullptr)
        return;

    TObject* select = gPad -> GetCanvas() -> GetClickSelected();
    if (select == nullptr) {
        gPad -> GetCanvas() -> SetClickSelected(nullptr);
        return;
    }

    bool isNotH2 = !(select -> InheritsFrom(TH1::Class()));
    //bool isNotGraph = !(select -> InheritsFrom(TGraph::Class()));
    //if (isNotH2 && isNotGraph)
    if (isNotH2) {
        gPad -> GetCanvas() -> SetClickSelected(nullptr);
        return;
    }

    int xEvent = gPad -> GetEventX();
    int yEvent = gPad -> GetEventY();
    int xAbs = gPad -> AbsPixeltoX(xEvent);
    int yAbs = gPad -> AbsPixeltoY(yEvent);
    double xOnClick = gPad -> PadtoX(xAbs);
    double yOnClick = gPad -> PadtoY(yAbs);

    TH2* hist = dynamic_cast<TH2*> (select);
    int binCurr = hist -> FindBin(xOnClick, yOnClick);
    int binLast = gPad -> GetUniqueID();
    if (binCurr==binLast || binCurr<=0) {
        gPad -> GetCanvas() -> SetClickSelected(nullptr);
        return;
    }

    gPad -> SetUniqueID(binCurr);
    gPad -> GetCanvas() -> SetClickSelected(nullptr);
    TTMicromegas::GetMMCC() -> SelectAndDrawChannelStrip(binCurr);
}
