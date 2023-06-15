#include "TTBackwardArray.h"

ClassImp(TTBackwardArray);

TTBackwardArray::TTBackwardArray()
{
    ;
}

bool TTBackwardArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTBackwardArray" << std::endl;
    
    return true;
}

void TTBackwardArray::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTBackwardArray container" << std::endl;
}

bool TTBackwardArray::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTBackwardArray::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTBackwardArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTBackwardArray",TTBackwardArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTBackwardArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTBackwardArray",TTBackwardArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTBackwardArray::DrawEvent(Option_t *option)
{
    return true;
}

bool TTBackwardArray::SetDataFromBranch()
{
    return false;
}

void TTBackwardArray::DrawHist()
{
    ;
}

void TTBackwardArray::DrawFrame(Option_t *option)
{
    ;
}

void TTBackwardArray::MouseClickEvent(int iPlane)
{
    ;
}

void TTBackwardArray::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
