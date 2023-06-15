#include "TTRightArray.h"

ClassImp(TTRightArray);

TTRightArray::TTRightArray()
{
    ;
}

bool TTRightArray::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTRightArray" << std::endl;
    
    return true;
}

void TTRightArray::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTRightArray container" << std::endl;
}

bool TTRightArray::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTRightArray::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTRightArray::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTRightArray",TTRightArray);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTRightArray::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTRightArray",TTRightArray,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTRightArray::DrawEvent(Option_t *option)
{
    return true;
}

bool TTRightArray::SetDataFromBranch()
{
    return false;
}

void TTRightArray::DrawHist()
{
    ;
}

void TTRightArray::DrawFrame(Option_t *option)
{
    ;
}

void TTRightArray::MouseClickEvent(int iPlane)
{
    ;
}

void TTRightArray::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
