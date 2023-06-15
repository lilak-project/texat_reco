#include "TTBottomArray.h"

ClassImp(TTBottomArray);

TTBottomArray::TTBottomArray()
{
    ;
}

bool TTBottomArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTBottomArray" << std::endl;
    
    return true;
}

void TTBottomArray::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTBottomArray container" << std::endl;
}

bool TTBottomArray::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTBottomArray::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTBottomArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTBottomArray",TTBottomArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTBottomArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTBottomArray",TTBottomArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTBottomArray::DrawEvent(Option_t *option)
{
    return true;
}

bool TTBottomArray::SetDataFromBranch()
{
    return false;
}

void TTBottomArray::DrawHist()
{
    ;
}

void TTBottomArray::DrawFrame(Option_t *option)
{
    ;
}

void TTBottomArray::MouseClickEvent(int iPlane)
{
    ;
}

void TTBottomArray::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
