#include "TTLeftArray.h"

ClassImp(TTLeftArray);

TTLeftArray::TTLeftArray()
{
    ;
}

bool TTLeftArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTLeftArray" << std::endl;
    
    return true;
}

void TTLeftArray::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTLeftArray container" << std::endl;
}

bool TTLeftArray::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTLeftArray::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTLeftArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTLeftArray",TTLeftArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTLeftArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTLeftArray",TTLeftArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTLeftArray::DrawEvent(Option_t *option)
{
    return true;
}

bool TTLeftArray::SetDataFromBranch()
{
    return false;
}

void TTLeftArray::DrawHist()
{
    ;
}

void TTLeftArray::DrawFrame(Option_t *option)
{
    ;
}

void TTLeftArray::MouseClickEvent(int iPlane)
{
    ;
}

void TTLeftArray::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
