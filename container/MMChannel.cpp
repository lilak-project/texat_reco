#include "MMChannel.h"

ClassImp(MMChannel);

MMChannel::MMChannel()
{
    Clear();
}

void MMChannel::Clear(Option_t *option)
{
    LKChannel::Clear(option);
    fFrameNo = -1;
    fDecayNo = -1;
    fCobo = -1;
    fAsad = -1;
    fAget = -1;
    fChan = -1;
    fTime = -1;
    fEnergy = -1;
    for (auto i=0; i<512; ++i) fWaveformX[i] = -1;
    for (auto i=0; i<512; ++i) fWaveformY[i] = -1;
}

void MMChannel::Print(Option_t *option) const 
{
    // You will probability need to modify here
    LKChannel::Print();
    lx_info << "MMChannel container" << std::endl;
    lx_info << "fFrameNo : " << fFrameNo << std::endl;
    lx_info << "fDecayNo : " << fDecayNo << std::endl;
    lx_info << "fCobo : " << fCobo << std::endl;
    lx_info << "fAsad : " << fAsad << std::endl;
    lx_info << "fAget : " << fAget << std::endl;
    lx_info << "fChan : " << fChan << std::endl;
    lx_info << "fTime : " << fTime << std::endl;
    lx_info << "fEnergy : " << fEnergy << std::endl;
    lx_info << "fWaveformX[512] : " << std::endl;
    for (auto i=0; i<512; ++i)
        lx_cout << fWaveformX[i] << " ";
    lx_cout << std::endl;
    lx_info << "fWaveformY[512] : " << std::endl;
    for (auto i=0; i<512; ++i)
        lx_cout << fWaveformY[i] << " ";
    lx_cout << std::endl;
}

void MMChannel::Copy(TObject &object) const 
{
    // You should copy data from this container to objCopy
    LKChannel::Copy(object);
    auto objCopy = (MMChannel &) object;
    objCopy.SetFrameNo(fFrameNo);
    objCopy.SetDecayNo(fDecayNo);
    objCopy.SetCobo(fCobo);
    objCopy.SetAsad(fAsad);
    objCopy.SetAget(fAget);
    objCopy.SetChan(fChan);
    objCopy.SetTime(fTime);
    objCopy.SetEnergy(fEnergy);
    objCopy.SetWaveformX(fWaveformX);
    objCopy.SetWaveformY(fWaveformY);
}
