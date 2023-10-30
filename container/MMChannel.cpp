#include "MMChannel.h"

ClassImp(MMChannel);

MMChannel::MMChannel()
{
    Clear();
}

void MMChannel::Clear(Option_t *option)
{
    LKChannel::Clear(option);
    fDetType = -1;
    fFrameNo = -1;
    fDecayNo = -1;
    fCobo = -1;
    fAsad = -1;
    fAget = -1;
    fChan = -1;
    fDChan = -1;
    fTime = -1;
    fEnergy = -1;
    for (auto i=0; i<512; ++i) fWaveformX[i] = -1;
    for (auto i=0; i<512; ++i) fWaveformY[i] = -1;
}

void MMChannel::Print(Option_t *option) const
{
    // You will probability need to modify here
    e_info << "MMChannel container" << std::endl;
    e_info << "fDetType : " << fDetType << std::endl;
    e_info << "fFrameNo : " << fFrameNo << std::endl;
    e_info << "fDecayNo : " << fDecayNo << std::endl;
    e_info << "fCobo : " << fCobo << std::endl;
    e_info << "fAsad : " << fAsad << std::endl;
    e_info << "fAget : " << fAget << std::endl;
    e_info << "fChan : " << fChan << std::endl;
    e_info << "fDChan : " << fDChan << std::endl;
    e_info << "fTime : " << fTime << std::endl;
    e_info << "fEnergy : " << fEnergy << std::endl;
    for (auto i=0; i<512; ++i) e_cout << fWaveformX[i] << " "; e_cout << std::endl;
    for (auto i=0; i<512; ++i) e_cout << fWaveformY[i] << " "; e_cout << std::endl;
}

void MMChannel::Copy(TObject &object) const
{
    // You should copy data from this container to objCopy
    LKChannel::Copy(object);
    auto objCopy = (MMChannel &) object;
    objCopy.SetDetType(fDetType);
    objCopy.SetFrameNo(fFrameNo);
    objCopy.SetDecayNo(fDecayNo);
    objCopy.SetCobo(fCobo);
    objCopy.SetAsad(fAsad);
    objCopy.SetAget(fAget);
    objCopy.SetChan(fChan);
    objCopy.SetDChan(fDChan);
    objCopy.SetTime(fTime);
    objCopy.SetEnergy(fEnergy);
    objCopy.SetWaveformX(fWaveformX);
    objCopy.SetWaveformY(fWaveformY);
}

MMChannel* MMChannel::CloneChannel() const
{
    auto clone = new MMChannel();
    clone -> SetDetType(fDetType);
    clone -> SetFrameNo(fFrameNo);
    clone -> SetDecayNo(fDecayNo);
    clone -> SetCobo(fCobo);
    clone -> SetAsad(fAsad);
    clone -> SetAget(fAget);
    clone -> SetChan(fChan);
    clone -> SetDChan(fDChan);
    clone -> SetTime(fTime);
    clone -> SetEnergy(fEnergy);
    clone -> SetWaveformX(fWaveformX);
    clone -> SetWaveformY(fWaveformY);
    return clone;
}
