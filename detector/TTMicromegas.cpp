#include "TTMicromegas.h"

ClassImp(TTMicromegas);

TTMicromegas::TTMicromegas()
{
    ;
}

bool TTMicromegas::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTMicromegas" << std::endl;
    
    return true;
}

void TTMicromegas::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKDetectorPlane::Print();
    lx_info << "TTMicromegas container" << std::endl;
}

bool TTMicromegas::IsInBoundary(Double_t i, Double_t j)
{
    return true;
}

Int_t TTMicromegas::FindChannelID(Double_t i, Double_t j)
{
    // example find id
    // int id = 100*i + j;
    // return id;
    return -1;
}

TCanvas* TTMicromegas::GetCanvas(Option_t *option)
{
    // example canvas
    // if (fCanvas==nullptr)
    //     fCanvas = new TCanvas("TTMicromegas",TTMicromegas);
    // return fCanvas;
    return (TCanvas *) nullptr;
}

TH2* TTMicromegas::GetHist(Option_t *option)
{
    // example hist
    // if (fHist==nullptr)
    //     fHist = new TH2D("TTMicromegas",TTMicromegas,10,0,10);
    // return fHist;
    return (TH2D *) nullptr;
}

bool TTMicromegas::DrawEvent(Option_t *option)
{
    return true;
}

bool TTMicromegas::SetDataFromBranch()
{
    return false;
}

void TTMicromegas::DrawHist()
{
    ;
}

void TTMicromegas::DrawFrame(Option_t *option)
{
    ;
}

void TTMicromegas::MouseClickEvent(int iPlane)
{
    ;
}

void TTMicromegas::ClickedAtPosition(Double_t x, Double_t y)
{
    ;
}
