#include "TTBackwardArray.h"

ClassImp(TTBackwardArray);

TTBackwardArray::TTBackwardArray()
{
    fName = "TTBackwardArray";
    if (fChannelArray==nullptr)
        fChannelArray = new TObjArray();
}

bool TTBackwardArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTBackwardArray" << std::endl;

    return true;
}

void TTBackwardArray::Clear(Option_t *option)
{
    LKPadPlane::Clear(option);
}

void TTBackwardArray::Print(Option_t *option) const
{
    // You will probability need to modify here
    lx_info << "TTBackwardArray" << std::endl;
}

bool TTBackwardArray::IsInBoundary(Double_t x, Double_t y)
{
    // example (x,y) is inside the plane boundary
    //if (x>-10 and x<10)
    //    return true;
    //return false;
    return true;
}

Int_t TTBackwardArray::FindChannelID(Double_t x, Double_t y)
{
    // example find id
    // int id = 100*x + y;
    // return id;
    return -1;
}

Int_t TTBackwardArray::FindChannelID(Int_t section, Int_t row, Int_t layer)
{
    // example find id
    // int id = 10000*section + 100*row + layer;
    // return id;
    return -1;
}

/*
TCanvas* TTBackwardArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTBackwardArray",TTBackwardArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}
*/

TH2* TTBackwardArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTBackwardArray",TTBackwardArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

/*
bool TTBackwardArray::SetDataFromBranch()
{
    return false;
}
*/

/*
void TTBackwardArray::FillDataToHist()
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTBackwardArray",TTBackwardArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}
*/

void TTBackwardArray::DrawFrame(Option_t *option)
{
    ;
}

/*
void TTBackwardArray::Draw(Option_t *option)
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
void TTBackwardArray::MouseClickEvent(int iPlane)
{
    ;
}
*/
