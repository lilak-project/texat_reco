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
    fSiBLR = -1;
    fsiLhit = -1;
    fsiRhit = -1;
    fsiChit = -1;
    fX6Lhit = -1;
    fX6Rhit = -1;
    fGoodSiEvt = eGoodSiEvt::kNon;
    fGoodMMEvt = false;
}

void TTEventHeader::Print(Option_t *option) const
{
    // You will probability need to modify here
    e_info << "good mm B / L R C / XL XR :"
        << " " << fIsGoodEvent
        << " " << fIsMMEvent
        << " " << fSiBLR
        << " / " << fsiLhit
        << " " << fsiRhit
        << " " << fsiChit
        << " / " << fX6Lhit
        //<< " " << fX6Rhit << std::endl;
        << " " << fX6Rhit
        << " | " << (int)fGoodSiEvt
        << " " << fGoodMMEvt << std::endl;
}

void TTEventHeader::Copy(TObject &object) const
{
    // You should copy data from this container to objCopy
    LKContainer::Copy(object);
    auto objCopy = (TTEventHeader &) object;
    objCopy.SetIsGoodEvent(fIsGoodEvent);
    objCopy.SetIsMMEvent(fIsMMEvent);
    objCopy.SetSiBLR(fSiBLR);
    objCopy.SetSiLhit(fsiLhit);
    objCopy.SetSiRhit(fsiRhit);
    objCopy.SetSiChit(fsiChit);
    objCopy.SetX6Lhit(fX6Lhit);
    objCopy.SetX6Rhit(fX6Rhit);
    objCopy.SetGoodSiEvt(fGoodSiEvt);
    objCopy.SetGoodMMEvt(fGoodMMEvt);
}
