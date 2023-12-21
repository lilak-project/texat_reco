#include "GSpectra.h"
#include "GNetServerRoot.h"
//#include "WaveletNew.h"
#include "mfm/BitField.h"
#include "mfm/Field.h"
#include "mfm/Frame.h"
#include "mfm/FrameBuilder.h"
#include "mfm/FrameDictionary.h"
#include "mfm/Item.h"
#include <sstream>
#include <cstdio>
#include <boost/utility/binary.hpp>
const int no_cobos=3;
using namespace std;

#include "LKLogger.h"
#include "LKFrameBuilder.h"
#include "GETChannel.h"
#include "LKEventHeader.h"

LKFrameBuilder::LKFrameBuilder(int port) {
    fName = "LKFrameBuilder";
    //lk_debug << endl;
    //spectra_ = new GSpectra();
    //lk_debug << endl;
    //serv_ = new GNetServerRoot(port,spectra_);
    //lk_debug << endl;
    //serv_->StartServer();
    //lk_debug << endl;
}

LKFrameBuilder::~LKFrameBuilder() {
    //delete serv_;
    //delete spectra_;
}

void LKFrameBuilder::processFrame(mfm::Frame &frame)
{
    //lk_debug << "New frame!" << endl;
    //lk_debug << "isBlobFrame=" << frame.header().isBlobFrame() << ", frameType=" << frame.header().frameType() << endl;
    if (frame.header().isBlobFrame())
    {
        if (frame.header().frameType() == 0x7)
        {
            lk_info << "Frame is blob frame 0x7" << endl;
            decodeCoBoTopologyFrame(frame);
        }
        else if (frame.header().frameType()  == 0x8) {
            lk_info << "Frame is blob frame 0x8" << endl;
            decodeMuTanTFrame(frame);
        }
        lk_info << "Frame is blob frame" << endl;
    }
    else
    {
        ValidateEvent(frame);
        goodsievt=1;
        goodicevt=1;

        if(goodsievt==1)
        {
            goodmmevt=1;
            Event(frame);
            fMotherTask -> SignalNextEvent();
        }
    }
}

void LKFrameBuilder::Init(int mode, int d2pmode) {
    //cout<<"INIT HIST SERVER"<<endl;
    framecounter = 0;
    enableroot = 0;
    forcereadtree = 0;
    enablecleantrack = 0;
    energymethod = 0;
    readrw = 0;
    enable2pmode = d2pmode;
    enablehist = 0;
    enableupdatefast = 0;
    maxasad = 4;
    FirsteventIdx = 0;
    IsFirstevent = true;
    LasteventIdx = 0;
    eventID = 0;
    weventTime = 0;
    coboIC=2;
    asadIC=1;
    chanIC=2;
    valueIC_min=3500;
    decoffset=500; //ofset value for Deconvolution Method
    mm_minenergy = 0;
    //mm_maxenergy = 4096;
    //mm_minenergy = 0; //for AllChannelReading
    //mm_minenergy = 0;
    mm_maxenergy = 4096;
    //mm_mintime = 130;
    //mm_maxtime = 380;
    mm_mintime = 0;
    mm_maxtime = 350;
    si_minenergy = 0;
    si_maxenergy = 5000;
    si_mintime = 0;
    si_maxtime = 512;
    //driftv = 0.0025; // mm/ns, for P5 at 100 Torr and -800V
    driftv = 0.017; // mm/ns, for CO2 at 20 Torr and -800V
    fTimePerBin = 1000.; // 100 ns per bin

    lk_info << "Mode: " << mode << endl;

    if(mode==1){
        mfm::FrameDictionary::instance().addFormats("/usr/local/get/share/get-bench/format/CoboFormats.xcfg");
        waveforms = new WaveForms();
        InitWaveforms();
    }
}

void LKFrameBuilder::InitWaveforms() {
    waveforms->waveform.resize(maxasad*4);
    waveforms->hasSignal.resize(maxasad*4);
    waveforms->hasHit.resize(maxasad*4); //default set to false
    waveforms->hasFPN.resize(maxasad*4); //default set to false
    waveforms->doneFPN.resize(maxasad*4); //default set to false
    for(int i=0;i<maxasad;i++){
        for(int j=0;j<4;j++){
            waveforms->waveform[i*4+j].resize(68);
            waveforms->hasSignal[i*4+j].resize(68); //default set to false
            for(int k=0;k<68;k++){
                waveforms->waveform[i*4+j][k].resize(bucketmax);
            }
        }
    }
}

void LKFrameBuilder::ResetWaveforms() {
    for(int i=0;i<maxasad;i++){
        for(int j=0;j<4;j++){
            if(waveforms->hasHit[i*4+j] || waveforms->hasFPN[i*4+j]){
                for(int k=0;k<68;k++){
                    if(waveforms->hasSignal[i*4+j][k]){
                        waveforms->hasSignal[i*4+j][k] = false;
                        fill(waveforms->waveform[i*4+j][k].begin(),
                                waveforms->waveform[i*4+j][k].end(),0);
                    }
                }
            }
            waveforms->hasHit[i*4+j] = false;
            waveforms->hasFPN[i*4+j] = false;
            waveforms->doneFPN[i*4+j] = false;

        }
    }
    waveforms->ICenergy = 0;
    waveforms->Sienergy = 0;
    waveforms->SiX = 0;
    waveforms->SiY = 0;
    waveforms->SiZ = 0;
    waveforms->isRejected = 0;
}

void LKFrameBuilder::ValidateEvent(mfm::Frame& frame)
{
    //lk_debug << "ValidateEvent" << endl;
    if(frame.header().isLayeredFrame())
    {
        //lk_info << "Frame is layered frame" << endl;

        unique_ptr<mfm::Frame> subFrame;
        for(int i = 0;i<frame.itemCount();i++)
        {
            // frameAt ///////////////////////////////////////////////////////////////////////////////
            try { subFrame = frame.frameAt(i); }
            catch (const std::exception& e) {
                e_debug << "Error at frameAt("<<i<<")" << endl;
                e_cout << e.what() << endl;
                return;
            }

            // subFrame.get, itemCount ///////////////////////////////////////////////////////////////
            int numItems;
            try { numItems = (*subFrame.get()).itemCount(); /*Make sure we have data*/ }
            catch (const std::exception& e){
                e_debug << "Error at (*subFrame.get()).itemCount()" << endl;
                e_cout << e.what() << endl;
                return;
            }

            // subFrame.get(), ValidateFrame /////////////////////////////////////////////////////////
            try {
                if(numItems>0) { ValidateFrame(*subFrame.get()); }
                else{
                    //lk_debug << "New subframe-" << i << " arrived, but item-count is 0!" << endl;
                }
            }
            catch (const std::exception& e) {
                e_debug << "Error at ValidateFrame" << endl;
                e_cout << e.what() << endl;
                return;
            }
            //////////////////////////////////////////////////////////////////////////////////////////
        }
    }
    else {
        if(frame.itemCount()>0){ //Make sure we have data
            lk_info << "Frame is single frame" << endl;
            ValidateFrame(frame);
        }
        else {
            //lk_debug << "New frame arrived, but item-count is 0!" << endl;
        }
    }
}

void LKFrameBuilder::ValidateFrame(mfm::Frame& frame)
{
    UInt_t coboIdx = frame.headerField("coboIdx").value<UInt_t>();
    UInt_t asadIdx = frame.headerField("asadIdx").value<UInt_t>();
    UInt_t ceventIdx = (Int_t)frame.headerField("eventIdx").value<UInt_t>();

    //unsigned short SiMask = BOOST_BINARY( 111011111111111011111111110111111111111111111111101111111111011111111111 );
    ULong_t SiMask = 0xFEFFDFFFFFBFF7FF; //LSB 8byte
    //unsigned short MMMask = BOOST_BINARY( 111011111111111011111111110111111111111111111111101111111111011111111111 );
    ULong_t MMMask = 0xFEFFDFFFFFBFF7FF; //LSB 8byte
    //unsigned short MMMask1 = BOOST_BINARY( 000000000000000000000000000000000000000111111111101111111111011111111111 );
    ULong_t MMMask1 = 0x3FFBFF7FF; //LSB 8byte
    //unsigned short IcMask = BOOST_BINARY( 000000100000000000000000000000000000000000000000000000000000000000000000 );
    ULong_t IcMask = 0x13; // Channel 19

    //cout << coboIdx << " " << asadIdx << "(LSB):" << endl;
    ULong_t hitPat_0 = frame.headerField(32u, 8u).value<ULong_t>();
    ULong_t hitPat_1 = frame.headerField(41u, 8u).value<ULong_t>();
    ULong_t hitPat_2 = frame.headerField(50u, 8u).value<ULong_t>();
    ULong_t hitPat_3 = frame.headerField(59u, 8u).value<ULong_t>();
    //cout << "HitPattern 0: " << hex << hitPat_0 << endl;
    //cout << "HitPattern 1: " << hex << hitPat_1 << endl;
    //cout << "HitPattern 2: " << hex << hitPat_2 << endl;
    //cout << "HitPattern 3: " << hex << hitPat_3 << endl;
    //cout << "===========================" << endl;
    //000100000000000100000000001000000000000000000000010000000000100000000000
    //Si and IC data validation
    if(coboIdx==1 && asadIdx==0 && ((hitPat_0&SiMask)||(hitPat_1&SiMask)||(hitPat_2&SiMask)||(hitPat_3&SiMask))){
        goodsievt=1;
        goodevtidx = ceventIdx;
    }

    //MM and IC data validation
    //if(coboIdx==0 && ((hitPat_0&MMMask)||(hitPat_1&MMMask)||(hitPat_2&MMMask)||(hitPat_3&MMMask)))
    if(coboIdx==0 && ((asadIdx==2||asadIdx==3) && hitPat_2&MMMask1) && (asadIdx==0 && (hitPat_0&MMMask)) && (asadIdx<2 && ((hitPat_1&MMMask)||(hitPat_2&MMMask)||(hitPat_3&MMMask))))
    {
        goodmmevt=1;
        goodevtidx = ceventIdx;
    }

    //cout << coboIdx << " " << asadIdx << "(USB):" << endl;
    hitPat_0 = frame.headerField(31u, 1u).value<ULong_t>();
    hitPat_1 = frame.headerField(40u, 1u).value<ULong_t>();
    hitPat_2 = frame.headerField(49u, 1u).value<ULong_t>();
    hitPat_3 = frame.headerField(58u, 1u).value<ULong_t>();
    //cout << "HitPattern 0: " << hex << hitPat_0 << endl;
    //cout << "HitPattern 1: " << hex << hitPat_1 << endl;
    //cout << "HitPattern 2: " << hex << hitPat_2 << endl;
    //cout << "HitPattern 3: " << hex << hitPat_3 << endl;
    //cout << "===========================" << endl;
    //000100000000000100000000001000000000000000000000010000000000100000000000

    //Si and IC data validation
    //unsigned short SiMask = BOOST_BINARY( 111011111111111011111111110111111111111111111111101111111111011111111111 );
    SiMask = 0xEF; //USB 1byte
    if(coboIdx==1 && asadIdx==0 && ((hitPat_0&SiMask)||(hitPat_1&SiMask)||(hitPat_2&SiMask)||(hitPat_3&SiMask))){
        goodsievt=1;
        goodevtidx = ceventIdx;
    }

    //MM and IC data validation
    MMMask = 0xEF; //USB 1byte
    //if(coboIdx==0 && ((hitPat_0&MMMask)||(hitPat_1&MMMask)||(hitPat_2&MMMask)||(hitPat_3&MMMask)))
    if(coboIdx==0 && (asadIdx==0 && (hitPat_0&MMMask)) && (asadIdx<2 && ((hitPat_1&MMMask)||(hitPat_2&MMMask)||(hitPat_3&MMMask))))
    {
        goodmmevt=1;
        goodevtidx = ceventIdx;
    }
    if(coboIdx==1 && asadIdx==1 && (hitPat_0&IcMask)){
        goodicevt=1;
    }
}

void LKFrameBuilder::Event(mfm::Frame& frame)
{
    //lk_debug << "Event" << endl;
    //XXX
    waveforms->frameIdx = 0;
    waveforms->decayIdx = 0;
    fChannelArray -> Clear("C");
    fCountChannels = 0;
    if(frame.header().isLayeredFrame()) {
        for(int i = 0;i<frame.itemCount();i++) {
            //cout << "1isLayered=" << frame.header().isLayeredFrame() << ", itemCount=" << frame.itemCount() << ", frameIndex=" << i << endl;
            waveforms->frameIdx = 0;
            waveforms->decayIdx = 0;
            try{
                auto_ptr<mfm::Frame> subFrame = frame.frameAt(i);
                if((*subFrame.get()).itemCount()>0){ //Make sure we have data
                    //cout << "2isLayered=" << frame.header().isLayeredFrame() << ", itemCount=" << frame.itemCount() << ", frameIndex=" << i << endl;
                    waveforms->frameIdx = i;
                    UnpackFrame(*subFrame.get());
                    WriteChannels();
                    ResetWaveforms();
                }else{
                    //cout << "2no subframe" << endl;
                }
            }catch (const std::exception& e){
                cout << e.what() << endl;
                return;
            }
        }
    } else {
        try{
            if(frame.itemCount()>0){ //Make sure we have data
                //cout << "Not Layered Frame" << endl;
                UnpackFrame(frame);
                WriteChannels();
                ResetWaveforms();
            }else{
                cout << "3no subframe" << endl;
            }
        }catch (const std::exception& e){
            cout << e.what() << endl;
            return;
        }
    }
}

void LKFrameBuilder::UnpackFrame(mfm::Frame& frame)
{
    //XXX
    UInt_t coboIdx = frame.headerField("coboIdx").value<UInt_t>();
    UInt_t asadIdx = frame.headerField("asadIdx").value<UInt_t>();
    Int_t prevweventIdx = eventID;
    UInt_t frameSize = frame.headerField("frameSize").value<UInt_t>();
    UInt_t itemSize = frame.headerField("itemSize").value<UInt_t>();
    eventID = (Int_t)frame.headerField("eventIdx").value<UInt_t>();
    UInt_t prevweventTime = weventTime;
    weventTime = (UInt_t)frame.headerField("eventTime").value<UInt_t>();
    //cout<<"eventID=" << eventID << ", prevweventIdx+1=" << (prevweventIdx+1) << endl;
    //if(eventID>prevweventIdx+1) return;
    if(IsFirstevent) {
        //lk_info<<"FIRST EVENT!!!!!"<<endl;
        FirsteventIdx = eventID;
        IsFirstevent=false;
    }

    //if(enableskipevent==1 && eventID<firsteventno) return;
    //if(enableskipevent==2 && eventID<maxevtno){
    //if(!evtmask[eventID]) return;
    //}else if(enableskipevent==2 && eventID>=maxevtno){
    //  //cout << Form("eventID >= %d!",maxevtno) << endl;
    //}
    //else cout << "eventID " << eventID << endl;

    if((prevweventIdx != eventID))
    {
        if(readmode==1)
        {
            if(wGETMul>0){
                wGETEventIdx = prevweventIdx;
                if(prevgoodmmevt==1 || 1){
                    //lk_info << "In data: " << wGETEventIdx << ", enable2pmode: " << enable2pmode << endl;
                    if(enable2pmode==1){
                        //cout<<"mevtidx.size() = "<<mevtidx.size()<<endl;
                        for(int i=prevmidx;i<mevtidx.size();i++){
                            //cout<<mevtidx.at(i)<<"\t"<<wGETEventIdx<<endl;
                            if(mevtidx.at(i)==wGETEventIdx){
                                wGETD2PTime = md2ptime.at(i);
                                wGETTimeStamp = mtstmp.at(i);
                                //cout << "In data: " << i << " " << wGETEventIdx << " " << wGETD2PTime << " " << wGETTimeStamp << endl;
                                //cout << "In data: " << i << " " << wGETEventIdx << " " << wGETD2PTime << " " << wGETTimeStamp << " " << prevweventTime << endl;
                                prevmidx = i;
                            }
                        }
                    }
                    //XXX
                    RootWReset();
                    prevgoodmmevt = goodmmevt;
                    prevgoodicevt = goodicevt;
                    goodsievt=0;
                    goodmmevt=0;
                    goodicevt=0;
                    goodevtidx=0;
                }else{
                    wGETEventIdx = prevweventIdx;
                    RootWReset();
                    prevgoodmmevt = goodmmevt;
                    prevgoodicevt = goodicevt;
                    goodsievt=0;
                    goodmmevt=0;
                    goodicevt=0;
                    goodevtidx=0;
                }
                if((wGETEventIdx-FirsteventIdx)%50==0){
                    if(wGETEventIdx==FirsteventIdx){
                        //cout << Form("M%d:Starting from Event No. %d.",readmode,wGETEventIdx) << endl;
                    }else{
                        //cout << Form("M%d:Upto Event No. %d Converted..(from Event No. %d)",readmode,wGETEventIdx, FirsteventIdx) << endl;
                    }
                }
                wGETEventIdx = eventID;
                wGETMul=0;
                wGETHit=0;
                for(int i=0;i<16;i++) IsTrig[i]=0;
            }
        }
    }

    //lk_info << "Frame type: " << frame.header().frameType() << ", eventIdx-" << eventID << endl;

    //  if(eventID<3350){
    //    return;
    //  }

    mfm::Item item = frame.itemAt(0u);
    mfm::Field field = item.field("");

    const size_t numSamples = frame.itemCount();

    const size_t numChannels = 68u;
    const size_t numChips = 4u;
    vector<uint32_t> chanIdx_(numChips, 0u);
    vector<uint32_t> buckIdx_(numChips, 0u);
    if(frame.header().frameType() == 1u) {
        mfm::Item item = frame.itemAt(0u);
        mfm::Field field = item.field("");
        mfm::BitField agetIdxField = field.bitField("agetIdx");
        mfm::BitField chanIdxField = field.bitField("chanIdx");
        mfm::BitField buckIdxField = field.bitField("buckIdx");
        mfm::BitField sampleValueField = field.bitField("sample");

        if(enable2pmode==1){
            if(IsTrig[coboIdx*4+asadIdx]>0){
                IsTrig[coboIdx*4+asadIdx]=2;
                waveforms->decayIdx = 1;
            }else{
                IsTrig[coboIdx*4+asadIdx]=1;
                waveforms->decayIdx = 0;
            }
        }else{
            IsTrig[coboIdx*4+asadIdx]=0;
            waveforms->decayIdx = 0;
        }
        //cout << "Type:" << frame.header().frameType() << " " <<  eventID << "  " << frameSize << " " << coboIdx << " " << asadIdx << " " << waveforms->frameIdx << " " << waveforms->decayIdx << " " << frame.itemCount() << endl;
        int lastaget=-1;
        for(UInt_t i=0; i<frame.itemCount(); i++) {
            item = frame.itemAt(i);
            field = item.field(field);
            agetIdxField = field.bitField(agetIdxField);
            chanIdxField = field.bitField(chanIdxField);
            buckIdxField = field.bitField(buckIdxField);
            sampleValueField =field.bitField(sampleValueField);

            UInt_t agetIdx = agetIdxField.value<UInt_t>();
            //if(agetIdx==lastaget) break;
            lastaget=agetIdx;
            if(agetIdx<0 || agetIdx>3) return;
            UInt_t chanIdx = chanIdxField.value<UInt_t>();
            UInt_t buckIdx = buckIdxField.value<UInt_t>();
            UInt_t sampleValue = sampleValueField.value<UInt_t>();

            waveforms->coboIdx = coboIdx;
            waveforms->asadIdx = asadIdx;

            if((ignoremm==0) || (ignoremm==1 && coboIdx>0)){ // skip MM waveform data
                waveforms->hasSignal[asadIdx*4+agetIdx][chanIdx] = true;
                //      cout<<"BEEP "<<coboIdx<<"\t"<<asadIdx<<"\t"<<agetIdx<<"\t"<<chanIdx<<endl;
                if((chanIdx==11||chanIdx==22||chanIdx==45||chanIdx==56)) waveforms->hasFPN[asadIdx*4+agetIdx] = true;
                else waveforms->hasHit[asadIdx*4+agetIdx] = true;
                waveforms->waveform[asadIdx*4+agetIdx][chanIdx][buckIdx] = sampleValue;
                //cout << "Type:" << frame.header().frameType() << " " <<  eventID << "  " << frameSize << " " << coboIdx << " " << asadIdx << " " << agetIdx << " " << chanIdx << " " << buckIdx <<  " " << sampleValue << endl;
            }
        }

    }
    else if (frame.header().frameType() == 2u) {
        if(enable2pmode==1){
            if(IsTrig[coboIdx*4+asadIdx]>0){
                IsTrig[coboIdx*4+asadIdx]=2;
                waveforms->decayIdx = 1;
            }else{
                IsTrig[coboIdx*4+asadIdx]=1;
                waveforms->decayIdx = 0;
            }
        }else{
            IsTrig[coboIdx*4+asadIdx]=0;
            waveforms->decayIdx = 0;
        }
        //cout << "Type:" << frame.header().frameType() << " " <<  eventID << " " << frameSize << " " << coboIdx << " " << asadIdx << " " << waveforms->frameIdx << " " << waveforms->decayIdx << " " << endl;
        // Decode first item
        if(numSamples>0) {
            mfm::Item item = frame.itemAt(0u);
            mfm::Field field = item.field("");
            mfm::BitField agetIdxField = field.bitField("agetIdx");
            mfm::BitField sampleValueField = field.bitField("sample");

            const uint32_t agetIdx = agetIdxField.value<uint32_t>();
            const uint32_t sampleValue = sampleValueField.value<uint32_t>();

            waveforms->coboIdx = coboIdx;
            waveforms->asadIdx = asadIdx;

            if((ignoremm==0) || (ignoremm==1 && coboIdx>0)){ // skip MM waveform data
                waveforms->hasSignal[asadIdx*4+agetIdx][chanIdx_[agetIdx]] = true;
                if((chanIdx_[agetIdx]==11||chanIdx_[agetIdx]==22||chanIdx_[agetIdx]==45||chanIdx_[agetIdx]==56)) waveforms->hasFPN[asadIdx*4+agetIdx] = true;
                else waveforms->hasHit[asadIdx*4+agetIdx] = true;
                waveforms->waveform[asadIdx*4+agetIdx][chanIdx_[agetIdx]][buckIdx_[agetIdx]] = sampleValue;
            }
        }
        // Loop through other items
        for(size_t itemId=1; itemId<numSamples; ++itemId) {
            item = frame.itemAt(itemId);
            field = item.field(field);
            mfm::BitField agetIdxField = field.bitField(agetIdxField);
            mfm::BitField sampleValueField = field.bitField(sampleValueField);

            if((ignoremm==0) || (ignoremm==1 && coboIdx>0)){ // skip MM waveform data
                const uint32_t agetIdx = agetIdxField.value<uint32_t>();
                if(chanIdx_[agetIdx]>=numChannels) {
                    chanIdx_[agetIdx] = 0u;
                    buckIdx_[agetIdx]++;
                }
                const uint32_t sampleValue = sampleValueField.value<uint32_t>();

                //if(itemId>57120) cout << itemId << " " << numSamples << " " << coboIdx << " " << asadIdx << " " << agetIdx << " " << chanIdx_[agetIdx] << " " << buckIdx_[agetIdx] << " " << sampleValue << endl;

                waveforms->hasSignal[asadIdx*4+agetIdx][chanIdx_[agetIdx]] = true;
                if((chanIdx_[agetIdx]==11||chanIdx_[agetIdx]==22||chanIdx_[agetIdx]==45||chanIdx_[agetIdx]==56)) waveforms->hasFPN[asadIdx*4+agetIdx] = true;
                else waveforms->hasHit[asadIdx*4+agetIdx] = true;
                waveforms->waveform[asadIdx*4+agetIdx][chanIdx_[agetIdx]][buckIdx_[agetIdx]] = sampleValue;
                chanIdx_[agetIdx]++;
            }
        }
    }
    else {
        cout << "Frame type " << frame.header().frameType() << " not found" << endl;
        return;
    }
}

void LKFrameBuilder::decodeCoBoTopologyFrame(mfm::Frame& frame) {
    if (frame.header().frameType() == 0x7){
        // Print meta-data
        //cout << "Topology:" << endl;
        // Skip 8 Byte header
        istream & stream = frame.serializer().inputStream(8u);
        uint8_t value;
        stream.read(reinterpret_cast<char *>(&value), 1lu);
        //cout << " coboIdx=" << (uint16_t) value << ", ";
        stream.read(reinterpret_cast<char *>(&value), 1lu);
        //cout << " asadMask=" << hex << showbase << (uint16_t) value << dec << ", ";
        stream.read(reinterpret_cast<char *>(&value), 1lu);
        //cout << " 2pMode=" << boolalpha << (bool) value << endl;
    }
}

void LKFrameBuilder::decodeMuTanTFrame(mfm::Frame & frame)
{
    mutantcounter++;
    if (frame.header().frameType() == 0x8)
    {
        try
        {
            if(enable2pmode==1){
                mevtidx.push_back(frame.headerField(14u, 4u).value< uint32_t >());
                md2ptime.push_back(frame.headerField(60u, 4u).value< uint32_t >()*10);
                mtstmp.push_back(frame.headerField(8u, 6u).value< uint64_t >());
                cout << "decoded: " << mevtidx.at(mevtidx.size()-1) << " " << md2ptime.at(md2ptime.size()-1) << " " << mtstmp.at(mtstmp.size()-1) << endl;
            }else{
                scaler1 = frame.headerField(48u, 4u).value< uint32_t >();
                scaler2 = frame.headerField(52u, 4u).value< uint32_t >();
                scaler3 = frame.headerField(56u, 4u).value< uint32_t >();
                scaler4 = frame.headerField(40u, 4u).value< uint32_t >();
                scaler5 = frame.headerField(44u, 4u).value< uint32_t >();
                scevent = frame.headerField(24u, 4u).value< uint32_t >();
                scevtidx = frame.headerField(14u, 4u).value< uint32_t >();
                sctstmp = frame.headerField(8u, 6u).value< uint64_t >();
                //if(scaler3>0 && (int)scaler3%1000000==0)
                if(mutantcounter==100)
                {
                    scaler1end = scaler1;
                    scaler2end = scaler2;
                    scaler3end = scaler3;
                    cout << " Scaler 1 Rate = " << 100*(scaler1end-scaler1start)/(scaler3end-scaler3start) << ", " << (scaler1end-scaler1start) << " " << (scaler3end-scaler3start) << " " << scaler1end << " " << scaler1start << " " << scaler3end << " " << scaler3start << " or " << (100*scaler1end/scaler3end) << endl;
                    cout << " Scaler 2 Rate(Live) = " << 100*(scaler2end-scaler2start)/(scaler3end-scaler3start) << ", " << scaler2end << " " << scaler2start << " " << scaler3end << " " << scaler3start << " or " << (100*scaler2end/scaler3end) << endl;
                    if(enablescaler==1){
                        scalerout.open("scalers.txt", std::ofstream::out|std::ofstream::app);
                        scalerout << sctstmp << " " << scevent << " " << scevtidx << " " << scaler1 << " " << scaler2 << " " << scaler3 << endl;
                        scalerout.close();
                    }
                    mutantcounter=0;
                }
                //else if(scaler3>0 && (int)scaler3%1000000==1)
                else if(mutantcounter==1)
                {
                    scaler1start = scaler1;
                    scaler2start = scaler2;
                    scaler3start = scaler3;
                }
                if(enablescaler==0){
                    printed++;
                    if(printed>15){
                        printed=0;
                        cout << "Mutant: ";
                        cout << " TSTMP=" << frame.headerField(8u, 6u).value< uint64_t >() << ", ";
                        cout << " EVT_NO=" << frame.headerField(14u, 4u).value< uint32_t >() << ", ";
                        cout << " TRIG=" << hex << showbase << frame.headerField(18u, 2u).value< uint16_t >() << dec << ", ";
                        cout << " MULT_A="  << frame.headerField(20u, 2u).value< uint16_t >() << ", ";
                        cout << " MULT_B="  << frame.headerField(22u, 2u).value< uint16_t >() << ", ";
                        cout << " L0_EVT="  << frame.headerField(24u, 4u).value< uint32_t >() << ", ";
                        cout << " L1A_EVT="  << frame.headerField(28u, 4u).value< uint32_t >() << ", ";
                        cout << " L1B_EVT="  << frame.headerField(32u, 4u).value< uint32_t >() << ", ";
                        cout << " L2_EVT="  << frame.headerField(36u, 4u).value< uint32_t >() << ", ";
                        cout << " SCA1="  << frame.headerField(48u, 4u).value< uint32_t >() << ", ";
                        cout << " SCA2="  << frame.headerField(52u, 4u).value< uint32_t >() << ", ";
                        cout << " SCA3="  << frame.headerField(56u, 4u).value< uint32_t >() << ", ";
                        cout << " SCA4="  << frame.headerField(40u, 4u).value< uint32_t >() << ", ";
                        cout << " SCA5="  << frame.headerField(44u, 4u).value< uint32_t >() << ", ";
                        cout << " D2P="  << frame.headerField(60u, 4u).value< uint32_t >() << endl;
                        //cout << frame.headerField(8u, 6u).value< uint64_t >() << " ";
                        //cout << frame.headerField(14u, 4u).value< uint32_t >() << " ";
                        //cout << frame.headerField(24u, 4u).value< uint32_t >() << " ";
                        //cout << frame.headerField(28u, 4u).value< uint32_t >() << " ";
                        //cout << frame.headerField(32u, 4u).value< uint32_t >() << " ";
                        //cout << frame.headerField(36u, 4u).value< uint32_t >() << " ";
                        cout << scaler1 << " " << scaler2 << " " << scaler3 << ", IC_Rate=" << Form("%5.2f",(100*scaler1/scaler3)) << "pps, Live=" << Form("%5.2f",((100*scaler2)/scaler3)) << "\%" << endl;
                        //double beamrate = scaler3/2000;
                        //cout << "beamrate: " << scevent << " " << beamrate << endl;
                        /*
                           cout << "Mutant:" << endl;
                           cout << " TIMESTAMP=" << frame.headerField(8u, 6u).value< uint32_t >() << endl;
                           cout << " EVENT_NUMBER=" << frame.headerField(14u, 4u).value< uint32_t >() << endl;
                           cout << " TRIGGER_INFO=" << hex << showbase << frame.headerField(18u, 2u).value< uint16_t >() << dec << endl;
                           cout << " MULTIPLICITY_A_MEM="  << frame.headerField(20u, 2u).value< uint16_t >() << endl;
                           cout << " MULTIPLICITY_B_MEM="  << frame.headerField(22u, 2u).value< uint16_t >() << endl;
                           cout << " L0_EVT_COUNTER="  << frame.headerField(24u, 4u).value< uint32_t >() << endl;
                           cout << " L1A_EVT_COUNTER="  << frame.headerField(28u, 4u).value< uint32_t >() << endl;
                           cout << " L1B_EVT_COUNTER="  << frame.headerField(32u, 4u).value< uint32_t >() << endl;
                           cout << " L2_EVT_COUNTER="  << frame.headerField(36u, 4u).value< uint32_t >() << endl;
                           cout << " SCALER1_REG="  << frame.headerField(48u, 4u).value< uint32_t >() << endl;
                           cout << " SCALER2_REG="  << frame.headerField(52u, 4u).value< uint32_t >() << endl;
                           cout << " SCALER3_REG="  << frame.headerField(56u, 4u).value< uint32_t >() << endl;
                           cout << " SCALER4_REG="  << frame.headerField(40u, 4u).value< uint32_t >() << endl;
                           cout << " SCALER5_REG="  << frame.headerField(44u, 4u).value< uint32_t >() << endl;
                           cout << " D2P_TIME="  << frame.headerField(60u, 4u).value< uint32_t >() << endl;
                         */
                    }
                }
            }
        } catch (const mfm::Exception &)
        {}
    }
}

void LKFrameBuilder::RootWReset()
{
    for(int i=0;i<=wGETMul;i++){
        wGETFrameNo[i] = 0;
        wGETDecayNo[i] = 0;
        wGETTime[i] = 0;
        wGETEnergy[i] = 0;
        wGETCobo[i] = 0;
        wGETAsad[i] = 0;
        wGETAget[i] = 0;
        wGETChan[i] = 0;
        for(int j=0;j<bucketmax;j++){
            wGETWaveformX[i][j] = 0;
            wGETWaveformY[i][j] = 0;
        }
    }
}

void LKFrameBuilder::WriteChannels()
{
    //lk_debug << "WriteChannels" << endl;

    UInt_t frameIdx = waveforms->frameIdx;
    UInt_t decayIdx = waveforms->decayIdx;
    UInt_t coboIdx = waveforms->coboIdx;

    auto eventHeader = (LKEventHeader *) fEventHeaderArray -> ConstructedAt(0);
    eventHeader -> SetEventNumber(int(eventID));

    for (UInt_t asad=0; asad<maxasad; asad++)
    {
        for (UInt_t aget=0; aget<4; aget++)
        {
            if (!waveforms->hasHit[asad*4+aget])
                continue; // Skip no fired agets.

            for (UInt_t chan=0; chan<68; chan++)
            {
                if (!waveforms->hasSignal[asad*4+aget][chan])
                    continue; // Skip no fired channels.

                if (coboIdx>=0)
                {
                    if (readmode==1)
                    {
                        auto channel = (GETChannel *) fChannelArray -> ConstructedAt(fCountChannels++);
                        channel -> SetFrameNo(frameIdx);
                        channel -> SetDecayNo(decayIdx);
                        channel -> SetCobo(coboIdx);
                        channel -> SetAsad(asad);
                        channel -> SetAget(aget);
                        channel -> SetChan(chan);
                        channel -> SetTime(0);
                        channel -> SetEnergy(0);
                        channel -> SetWaveformY(waveforms->waveform[asad*4+aget][chan]);

                        wGETMul++;
                        if (chan!=11&&chan!=22&&chan!=45&&chan!=56)
                            wGETHit++;
                    }
                }
            }
        }
    }
}
