#include <iostream>
using namespace std;

#include "LKMFMConversionTask.h"
#include "GETChannel.h"

#include "GSpectra.h"
#include "GNetServerRoot.h"

LKMFMConversionTask::LKMFMConversionTask()
:LKTask("LKMFMConversionTask","")
{
}

bool LKMFMConversionTask::Init() 
{
    fEventHeaderArray = fRun -> RegisterBranchA("FrameHeader", "LKEventHeader", 1);
    fChannelArray     = fRun -> RegisterBranchA("RawData", "GETChannel", 1000);

    int mode            = fPar -> GetParInt("LKMFMConversionTask/RunMode");
    int readtype        = fPar -> GetParInt("LKMFMConversionTask/ReadType");
    int converterPort   = fPar -> GetParInt("LKMFMConversionTask/ConverterPort");
    int bucketSize      = fPar -> GetParInt("LKMFMConversionTask/BucketSize");
    int scalerMode      = fPar -> GetParInt("LKMFMConversionTask/ScalerMode");
    int d2pMode         = fPar -> GetParInt("LKMFMConversionTask/2pMode");
    int updatefast      = fPar -> GetParInt("LKMFMConversionTask/UpdateFast");
    auto inputFileName  = fPar -> GetParString("LKMFMConversionTask/InputFileName");

    fFrameBuilder = new LKFrameBuilder(converterPort);
    fFrameBuilder -> SetBucketSize(bucketSize);
    fFrameBuilder -> Init(mode,d2pMode);
    fFrameBuilder -> SetReadMode(mode);
    fFrameBuilder -> SetReadType(readtype);
    fFrameBuilder -> SetScaler(scalerMode);
    fFrameBuilder -> Set2pMode(d2pMode);
    fFrameBuilder -> SetUpdateSpeed(updatefast);
    fFrameBuilder -> SetChannelArray(fChannelArray);
    fFrameBuilder -> SetEventHeaderArray(fEventHeaderArray);
    fFrameBuilder -> SetMotherTask(this);

    lk_info << "Opening file " << inputFileName << endl;
    lk_info << "Block size is " << matrixSize << endl;
    fFileStream.open(inputFileName.Data(), std::ios::binary | std::ios::in);
    if(!fFileStream) {
        lk_error << "Could not open input file!" << std::endl;
        return false;
    }

    return true;
}

void LKMFMConversionTask::Exec(Option_t*)
{
    if (fFileStream.eof()) {
        lk_warning << "end of MFM file!" << endl;
        fRun -> SignalEndOfRun();
        return;
    }

    fContinueEvent = true;
    int countAddDataChunk = 0;

    char *buffer = (char *) malloc (matrixSize);

    int filebuffer = 0;
    while (!fFileStream.eof() && fContinueEvent)
    {
        //fFileStream.seekg(filebuffer, std::ios_base::cur);
        fFileStream.read(buffer,matrixSize);
        filebuffer += matrixSize;
        //lk_debug << countAddDataChunk++ << endl;

        if(!fFileStream.eof()) {
            // addDataChunk ////////////////////////////////////////////////////////////////////////////////
            try {
                ++countAddDataChunk;
                fFrameBuilder -> addDataChunk(buffer,buffer+matrixSize);
            }catch (const std::exception& e){
                lk_debug << "Error occured from " << countAddDataChunk << "-th addDataChunk()" << endl;
                e_cout << e.what() << endl;
                return;
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////
        }
        else if(fFileStream.gcount()>0) {
            // addDataChunk ////////////////////////////////////////////////////////////////////////////////
            try {
                ++countAddDataChunk;
                fFrameBuilder -> addDataChunk(buffer,buffer+fFileStream.gcount());
                break;
            }catch (const std::exception& e){
                lk_debug << "Error occured from LAST " << countAddDataChunk << "-th addDataChunk()" << endl;
                e_cout << e.what() << endl;
                return;
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////
        }
    }
}

bool LKMFMConversionTask::EndOfRun()
{
    return true;
}

void LKMFMConversionTask::SignalNextEvent()
{
    fContinueEvent = fRun -> ExecuteNextEvent();
}
