#include "TTMicromegas.h"

ClassImp(TTMicromegas);

TTMicromegas::TTMicromegas()
{
    fName = "TTMicromegas";
    if (fChannelArray==nullptr)
        fChannelArray = new TObjArray();
}

bool TTMicromegas::Init()
{
    // Put intialization todos here which are not iterative job though event
    e_info << "Initializing TTMicromegas" << std::endl;

    return true;
}

void TTMicromegas::Clear(Option_t *option)
{
    LKPadPlane::Clear(option);
}

void TTMicromegas::Print(Option_t *option) const
{
    // You will probability need to modify here
    e_info << "TTMicromegas" << std::endl;
}

bool TTMicromegas::IsInBoundary(Double_t x, Double_t y)
{
    // example (x,y) is inside the plane boundary
    //if (x>-10 and x<10)
    //    return true;
    //return false;
    return true;
}

Int_t TTMicromegas::FindChannelID(Double_t x, Double_t y)
{
    // example find id
    // int id = 100*x + y;
    // return id;
    return -1;
}

Int_t TTMicromegas::FindChannelID(Int_t section, Int_t row, Int_t layer)
{
    // example find id
    // int id = 10000*section + 100*row + layer;
    // return id;
    return -1;
}

/*
TCanvas* TTMicromegas::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTMicromegas",TTMicromegas);
    // return fCanvas;
    return (TCanvas *) nullptr;
}
*/

TH2* TTMicromegas::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTMicromegas",TTMicromegas,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

/*
bool TTMicromegas::SetDataFromBranch()
{
    return false;
}
*/

/*
void TTMicromegas::FillDataToHist()
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTMicromegas",TTMicromegas,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}
*/

void TTMicromegas::DrawFrame(Option_t *option)
{
    ;
}

/*
void TTMicromegas::Draw(Option_t *option)
{
    SetDataFromBranch();
    FillDataToHist();
    auto hist = GetHist();
    if (hist==nullptr)
        return;
    if (fPar->CheckPar(fName+"/histZMin")) hist -> SetMinimum(fPar->GetParDouble(fName+"/histZMin"));
    else hist -> SetMinimum(0.01);
    if (fPar->CheckPar(fName+"/histZMax")) hist -> SetMaximum(fPar->GetParDouble(fName+"/histZMin"));
    auto cvs = GetCanvas();
    cvs -> Clear();
    cvs -> cd();
    hist -> Reset();
    hist -> DrawClone("colz");
    hist -> Reset();
    hist -> Draw("same");
    DrawFrame();
}
*/

/*
void TTMicromegas::MouseClickEvent(int iPlane)
{
    ;
}
*/
