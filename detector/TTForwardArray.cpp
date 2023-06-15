#include "TTForwardArray.h"

ClassImp(TTForwardArray);

TTForwardArray::TTForwardArray()
{
    ;
}

bool TTForwardArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTForwardArray" << std::endl;
    
    return true;
}

void TTForwardArray::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTForwardArray container" << std::endl;
}

bool TTForwardArray::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTForwardArray::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTForwardArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTForwardArray",TTForwardArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTForwardArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTForwardArray",TTForwardArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTForwardArray::DrawEvent(Option_t *option)
{
    return true;
}

bool TTForwardArray::SetDataFromBranch()
{
    return false;
}

void TTForwardArray::DrawHist()
{
    ;
}

void TTForwardArray::DrawFrame(Option_t *option)
{
    ;
}

void TTForwardArray::MouseClickEvent(int iPlane)
{
    ;
}

void TTForwardArray::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
