#include "TTLeftArray.h"

ClassImp(TTLeftArray);

TTLeftArray::TTLeftArray()
{
    fName = "TTLeftArray";
    if (fChannelArray==nullptr)
        fChannelArray = new TObjArray();
}

bool TTLeftArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTLeftArray" << std::endl;

    return true;
}

void TTLeftArray::Clear(Option_t *option)
{
    LKPadPlane::Clear(option);
}

void TTLeftArray::Print(Option_t *option) const
{
    // You will probability need to modify here
    lx_info << "TTLeftArray" << std::endl;
}

bool TTLeftArray::IsInBoundary(Double_t x, Double_t y)
{
    // example (x,y) is inside the plane boundary
    //if (x>-10 and x<10)
    //    return true;
    //return false;
    return true;
}

Int_t TTLeftArray::FindChannelID(Double_t x, Double_t y)
{
    // example find id
    // int id = 100*x + y;
    // return id;
    return -1;
}

Int_t TTLeftArray::FindChannelID(Int_t section, Int_t row, Int_t layer)
{
    // example find id
    // int id = 10000*section + 100*row + layer;
    // return id;
    return -1;
}

/*
TCanvas* TTLeftArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTLeftArray",TTLeftArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}
*/

TH2* TTLeftArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTLeftArray",TTLeftArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

/*
bool TTLeftArray::SetDataFromBranch()
{
    return false;
}
*/

/*
void TTLeftArray::FillDataToHist()
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTLeftArray",TTLeftArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}
*/

void TTLeftArray::DrawFrame(Option_t *option)
{
    ;
}

/*
void TTLeftArray::Draw(Option_t *option)
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
void TTLeftArray::MouseClickEvent(int iPlane)
{
    ;
}
*/
