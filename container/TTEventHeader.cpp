#include "TTEventHeader.h"

ClassImp(TTEventHeader);

TTEventHeader::TTEventHeader()
{
    Clear();
}

void TTEventHeader::Clear(Option_t *option)
{
    LKContainer::Clear(option);
    fIsGoodEvent = false;
    fIsMMEvent = false;
    fGoodMMEvt = false;
    fGoodSiEvt = eGoodSiEvt::kNon;
    fIsListed = false;
    fFiredDet = -1;
    fFiredStrip = -1;
    fEventNumber = -1;
    fSiBLR = -1;
    fSiLhit = -1;
    fSiRhit = -1;
    fSiChit = -1;
    fX6Lhit = -1;
    fX6Rhit = -1;
}

void TTEventHeader::Print(Option_t *option) const
{
    e_info << "[TexAT EventHeader]" << std::endl;
    e_cout << "- fIsGoodEvent : " << fIsGoodEvent << std::endl;
    e_cout << "- fIsMMEvent   : " << fIsMMEvent << std::endl;
    e_cout << "- fGoodMMEvt   : " << fGoodMMEvt << std::endl;
    e_cout << "- fGoodSiEvt   : " << (Int_t) fGoodSiEvt << std::endl;
    e_cout << "- fIsListed    : " << fIsListed << std::endl;
    e_cout << "- fFiredDet    : " << fFiredDet << std::endl;
    e_cout << "- fFiredStrip  : " << fFiredStrip << std::endl;
    e_cout << "- fEventNumber : " << fEventNumber << std::endl;
    e_cout << "- fSiBLR  : " << fSiBLR << std::endl;
    e_cout << "- fSiLhit : " << fSiLhit << std::endl;
    e_cout << "- fSiRhit : " << fSiRhit << std::endl;
    e_cout << "- fSiChit : " << fSiChit << std::endl;
    e_cout << "- fX6Lhit : " << fX6Lhit << std::endl;
    e_cout << "- fX6Rhit : " << fX6Rhit << std::endl;
}

void TTEventHeader::Copy(TObject &object) const
{
    LKContainer::Copy(object);
    auto objCopy = (TTEventHeader &) object;
    objCopy.SetIsGoodEvent(fIsGoodEvent);
    objCopy.SetIsMMEvent(fIsMMEvent);
    objCopy.SetGoodMMEvt(fGoodMMEvt);
    objCopy.SetGoodSiEvt(fGoodSiEvt);
    objCopy.SetIsListed(fIsListed);
    objCopy.SetFiredDet(fFiredDet);
    objCopy.SetFiredStrip(fFiredStrip);
    objCopy.SetEventNumber(fEventNumber);
    objCopy.SetSiBLR(fSiBLR);
    objCopy.SetSiLhit(fSiLhit);
    objCopy.SetSiRhit(fSiRhit);
    objCopy.SetSiChit(fSiChit);
    objCopy.SetX6Lhit(fX6Lhit);
    objCopy.SetX6Rhit(fX6Rhit);
}
