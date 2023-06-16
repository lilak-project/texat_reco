#include "TTEventHeader.h"

ClassImp(TTEventHeader);

TTEventHeader::TTEventHeader()
{
    Clear();
}

void TTEventHeader::Clear(Option_t *option)
{
    LKContainer::Clear(option);
    fIsGoodEvent = true;
    fSiBLR = -1;
    fsiLhit = -1;
    fsiRhit = -1;
    fsiChit = -1;
    fX6Lhit = -1;
    fX6Rhit = -1;
}

void TTEventHeader::Print(Option_t *option) const
{
    // You will probability need to modify here
    LKContainer::Print();
    lx_info << "TTEventHeader container" << std::endl;
    lx_info << "fIsGoodEvent : " << fIsGoodEvent << std::endl;
    lx_info << "SiBLR : " << fSiBLR << std::endl;
    lx_info << "siLhit : " << fsiLhit << std::endl;
    lx_info << "siRhit : " << fsiRhit << std::endl;
    lx_info << "siChit : " << fsiChit << std::endl;
    lx_info << "X6Lhit : " << fX6Lhit << std::endl;
    lx_info << "X6Rhit : " << fX6Rhit << std::endl;
}

void TTEventHeader::Copy(TObject &object) const
{
    // You should copy data from this container to objCopy
    LKContainer::Copy(object);
    auto objCopy = (TTEventHeader &) object;
    objCopy.SetIsGoodEvent(fIsGoodEvent);
    objCopy.SetSiBLR(fSiBLR);
    objCopy.SetSiLhit(fsiLhit);
    objCopy.SetSiRhit(fsiRhit);
    objCopy.SetSiChit(fsiChit);
    objCopy.SetX6Lhit(fX6Lhit);
    objCopy.SetX6Rhit(fX6Rhit);
}
