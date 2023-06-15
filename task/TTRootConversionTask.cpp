#include "TTRootConversionTask.h"
#include "MMChannel.h"

ClassImp(TTRootConversionTask);

TTRootConversionTask::TTRootConversionTask() 
{
    ;
}

bool TTRootConversionTask::Init() 
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTRootConversionTask" << std::endl;
    
    fInputFileName = fPar -> GetParString("TTRootConversionTask/inputFileName");
    fmapmmFileName = fPar -> GetParString("TTRootConversionTask/mapmmFileName");
    fmapsiFileName = fPar -> GetParString("TTRootConversionTask/mapsiFileName");
    fmapX6FileName = fPar -> GetParString("TTRootConversionTask/mapX6FileName");
    fmapCsIFileName = fPar -> GetParString("TTRootConversionTask/mapCsIFileName");
    
    fChannelArray = new TClonesArray("MMChannel",200);
    fRun -> RegisterBranch("RawData", fChannelArray);

    lx_info << "Input file is " << fInputFileName << endl;
    fInputFile = new TFile(fInputFileName, "read");
    fInputTree = (TTree*) fInputFile -> Get("TEvent");

    //SetBranch()
    {
        fInputTree -> SetBranchAddress("mmMul",&fmmMult);
        fInputTree -> SetBranchAddress("mmHit",&fmmHit);
        fInputTree -> SetBranchAddress("mmEventIdx",&fmmEventIdx);
        fInputTree -> SetBranchAddress("mmFrameNo",fmmFrameNo);
        fInputTree -> SetBranchAddress("mmDecayNo",fmmDecayNo);
        fInputTree -> SetBranchAddress("mmCobo",fmmCobo);
        fInputTree -> SetBranchAddress("mmAsad",fmmAsad);
        fInputTree -> SetBranchAddress("mmAget",fmmAget);
        fInputTree -> SetBranchAddress("mmChan",fmmChan);
        fInputTree -> SetBranchAddress("mmTime",fmmTime);
        fInputTree -> SetBranchAddress("mmEnergy",fmmEnergy);
        fInputTree -> SetBranchAddress("mmWaveformX",fmmWaveformX);
        fInputTree -> SetBranchAddress("mmWaveformY",fmmWaveformY);
        Int_t fNumEvents = fInputTree -> GetEntries();
        //Int_t fNumEvents = 2;
        fRun -> SetNumEvents(fNumEvents);
    }

    // SetDetType()
    {
        Int_t mmasad[1024];
        Int_t mmaget[1024];
        Int_t mmdchan[1024];
        Int_t mmx[1024];
        Int_t mmy[1024];
        Int_t mmpx[4][4][64];
        Int_t mmpy[4][4][64]; // [mmAsad][mmAget][dchan]

        Int_t siasad[45];
        Int_t siaget[45];
        Int_t sichan[45];
        Int_t six[45];
        Int_t siy[45];
        Int_t sipos[45]; // x: 0-4 | y: 0-1 | pos: 1,2,3,4 (in circle)
        Int_t sipx[4][4][68];
        Int_t sipy[4][4][68];
        Int_t sistrip[4][4][68];
        Int_t sidet[4][4][68]; // px: 0-4 | py: 0-1 | strip: 1,2,3,4 (in circle) | sidet: 0-9

        Int_t fcsidet[68]; // 0-9

        Int_t X6asad[600];
        Int_t X6aget[600];
        Int_t X6chan[600];
        Int_t X6flag[600];
        Int_t X6detnum[600];
        Int_t X6pos[600]; // flag: 0(junc) & 1(ohm) && 10(bottom junc) & 11(bottom ohm) | detnum: 0-32(max) | pos: 1-16(channel from the official doc.)
        Int_t X6det[4][4][68];
        Int_t X6strip[4][4][68];
        Int_t X6ud[4][4][68]; // det: 1? for LS, 2? for RS, 10? for LB, 20? for RB | strip: 1-8(junc) & 1-4(ohm) | ud: 0(pin side) & 1

        Int_t CsIasad[64];
        Int_t CsIaget[64];
        Int_t CsIchan[64];
        Int_t CsICTnum[64];
        Int_t CsIpinflag[64];
        Int_t CsItoX6det[64]; //should be changed
        Int_t CsICT[4][4][68];
        Int_t CsIpin[4][4][68];
        Int_t CsIX6det[4][4][68]; // CT: 1-64 | pin: 1 for X6 pin side | X6det: matching X6 num



        // initialized by -1
        for(Int_t i=0; i<3; i++) for(Int_t j=0; j<4; j++) for(Int_t k=0; k<4; k++) for(Int_t l=0; l<68; l++) 
        {
            fType[i][j][k][l] = eType::kNon;
            fDetLoc[i][j][k][l] = eDetLoc::kNon;
        }

        // MMS: 0 = Left Strip | 1 = Right Strip | 2 = Left Chain | 3 = Right Chain | 4 = Low Center | 5 = High Center
        Int_t line = 0;
        ifstream mapmm(fmapmmFileName);
        if(mapmm.fail()==true) lk_error << "error: mapchantomm " << fmapmmFileName << endl;
        while(mapmm.good())
        {
            mapmm >> mmasad[line] >> mmaget[line] >> mmdchan[line] >> mmx[line] >> mmy[line];
            //lk_debug << Form("%dth %d_%d_%d_%d",line,0,mmasad[line],mmaget[line],mmdchan[line]) << endl;
            line++;
            if(line==fmmnum) break;
        }
        if(line<fmmnum) lk_info << Form("mmnum check: %d/%d",line-1,fmmnum) << endl;
        mapmm.close();
        mapmm.clear();

        for(Int_t i=0; i<fmmnum; i++)
        {
            mmpx[mmasad[i]][mmaget[i]][mmdchan[i]]=mmx[i];
            mmpy[mmasad[i]][mmaget[i]][mmdchan[i]]=mmy[i];
            if(mmx[i]==-1)
            {
                if(mmasad[i]==2) fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLeftStrip;
                else fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kRightStrip;
            }
            else
            {
                if(mmx[i]>=0 && mmx[i]<64) 
                {
                    fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLeftChain;
                }
                else if(mmx[i]>69)
                {
                    fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kRightChain;
                }
                else if(mmx[i]>=65 && mmx[i]<69) 
                {
                    if(mmasad[i]==0 && mmaget[i]==3) fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kHighCenter;
                    else fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLowCenter;
                }
                else if(mmx[i]==64 || mmx[i]==69)
                {
                    if(mmaget[i]==3)
                    {
                        if(mmdchan[i]>15) fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLowCenter;
                        else fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kHighCenter;
                    }
                    else
                    {
                        fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLowCenter;
                    }
                }
            }
        }

        // Si + CsI + MMJr: 6 = Forward Si | 7 = Forward CsI | 8 = MMJr | 10 = CENS X6 | 11 = CENS CSI
        // Forward Si
        line = 0;
        ifstream mapsi;
        mapsi.open(fmapsiFileName);
        if(mapsi.fail()==true) lk_error << "error: mapchantosi " << fmapsiFileName << endl;
        while(mapsi.good())
        {
            mapsi >> siasad[line] >> siaget[line] >> sichan[line] >> six[line] >> siy[line] >> sipos[line];
            //cout << Form("%d: %d_%d_%d_%d_%d_%d",line,siasad[line],siaget[line],sichan[line],six[line],siy[line],sipos[line]) << endl;
            sipx[siasad[line]][siaget[line]][sichan[line]] = six[line];
            sipy[siasad[line]][siaget[line]][sichan[line]] = siy[line];
            sistrip[siasad[line]][siaget[line]][sichan[line]] = sipos[line];
            sidet[siasad[line]][siaget[line]][sichan[line]] = 2*(4-six[line])+(1-siy[line]);
            fType[1][siasad[line]][siaget[line]][sichan[line]] = eType::kForwardSi;

                 if(six[line]==2)                 fDetLoc[1][siasad[line]][siaget[line]][sichan[line]] = eDetLoc::kCenterFront;
            else if(six[line]==0 || six[line]==1) fDetLoc[1][siasad[line]][siaget[line]][sichan[line]] = eDetLoc::kLeft;
            else if(six[line]==3 || six[line]==4) fDetLoc[1][siasad[line]][siaget[line]][sichan[line]] = eDetLoc::kRight;

            line++;
            if(line==fsinum) break;
        }
        if(line<fsinum) lk_info << Form("sinum check: %d/%d",line-1,fsinum) << endl;
        mapsi.close();
        mapsi.clear();

        // Forward CsI
        for(Int_t i=0; i<68; i++) fcsidet[i] = -1;
        fcsidet[2] = 0;
        fcsidet[7] = 1;
        fcsidet[10] = 3;
        fcsidet[16] = 2;
        fcsidet[19] = 4;
        fcsidet[25] = 5;
        fcsidet[28] = 7;
        fcsidet[33] = 6;
        fcsidet[36] = 8;
        fcsidet[41] = 9;
        fType[1][1][0][2]  = eType::kForwardCsI;
        fType[1][1][0][7]  = eType::kForwardCsI;
        fType[1][1][0][10] = eType::kForwardCsI;
        fType[1][1][0][16] = eType::kForwardCsI;
        fType[1][1][0][19] = eType::kForwardCsI;
        fType[1][1][0][25] = eType::kForwardCsI;
        fType[1][1][0][36] = eType::kForwardCsI;
        fType[1][1][0][41] = eType::kForwardCsI;
        fDetLoc[1][1][0][2]  = eDetLoc::kRight; //0
        fDetLoc[1][1][0][7]  = eDetLoc::kRight; //1
        fDetLoc[1][1][0][10] = eDetLoc::kRight; //3
        fDetLoc[1][1][0][16] = eDetLoc::kRight; //2
        fDetLoc[1][1][0][19] = eDetLoc::kCenterFront; //4
        fDetLoc[1][1][0][25] = eDetLoc::kCenterFront; //5
        fDetLoc[1][1][0][28] = eDetLoc::kLeft; //7
        fDetLoc[1][1][0][33] = eDetLoc::kLeft; //6
        fDetLoc[1][1][0][36] = eDetLoc::kLeft; //8
        fDetLoc[1][1][0][41] = eDetLoc::kLeft; //9

        // External inputs
        fType[1][1][2][2]  = eType::kExternal; //SBD
        fType[1][1][2][7]  = eType::kExternal; //BM1
        fType[1][1][2][10] = eType::kExternal; //BM2
        fType[1][1][2][16] = eType::kExternal; //
        fType[1][1][2][19] = eType::kExternal; //BM shaping | BM1
        fType[1][1][2][25] = eType::kExternal; // | BM2
        fType[1][1][2][33] = eType::kExternal; // | BM shaping
        fType[1][1][3][2]  = eType::kExternal; //RF-PPACa
        fType[1][1][3][7]  = eType::kExternal; //PPACa-PPACb
        fType[1][1][3][10] = eType::kExternal; //BM-PPACa
        fType[1][1][3][16] = eType::kExternal; //BM raw
        fType[1][1][3][19] = eType::kExternal; //L0

        // CENS X6
        line = 0;
        ifstream mapX6;
        mapX6.open(fmapX6FileName);
        if(mapX6.fail()==true) lk_error << "error: mapchantoX6 " << fmapX6FileName << endl;
        while(mapX6.good())
        {
            mapX6 >> X6asad[line] >> X6aget[line] >> X6chan[line] >> X6flag[line] >> X6detnum[line] >> X6pos[line];
            X6det[X6asad[line]][X6aget[line]][X6chan[line]] = X6detnum[line];
            if(X6flag[line]==0) X6strip[X6asad[line]][X6aget[line]][X6chan[line]] = (Int_t) (X6pos[line]+1)/2;
            else if(X6flag[line]==1) X6strip[X6asad[line]][X6aget[line]][X6chan[line]] = X6pos[line];
            X6ud[X6asad[line]][X6aget[line]][X6chan[line]] = (X6pos[line]+1)%2;
            //cout << X6asad[line] << " " <<  X6aget[line] << " " <<  X6chan[line] << " " << X6det[X6asad[line]][X6aget[line]][X6chan[line]] << " " << X6strip[X6asad[line]][X6aget[line]][X6chan[line]] << " " << X6ud[X6asad[line]][X6aget[line]][X6chan[line]] << " " << endl;
            fType[2][X6asad[line]][X6aget[line]][X6chan[line]] = eType::kCENSX6;

                 if(X6asad[line]==0 && X6aget[line]!=3) fDetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = eDetLoc::kBottomRightX6;
            else if(X6asad[line]==1 && X6aget[line]!=3) fDetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = eDetLoc::kBottomLeftX6;
            else if(X6asad[line]==2 && X6aget[line]!=3) fDetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = eDetLoc::kLeft;
            else if(X6asad[line]==3 && X6aget[line]!=3) fDetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = eDetLoc::kRight;

            line++;
            if(line==fX6num) break;
        }
        if(line<fX6num) lk_info << Form("X6num check: %d/%d",line-1,fX6num) << endl;
        mapX6.close();
        mapX6.clear();

        // CENS CsI
        line = 0;
        ifstream mapCsI;
        mapCsI.open(fmapCsIFileName);
        if(mapCsI.fail()==true) lk_error << "error: mapchantoCsI " << fmapCsIFileName << endl;
        while(mapCsI.good())
        {
            mapCsI >> CsIasad[line] >> CsIaget[line] >> CsIchan[line] >> CsICTnum[line] >> CsIpinflag[line] >> CsItoX6det[line];
            CsICT[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsICTnum[line];
            CsIpin[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsIpinflag[line];
            CsIX6det[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsItoX6det[line];

            fType[2][CsIasad[line]][CsIaget[line]][CsIchan[line]] = eType::kCENSCsI;
            line++;
            if(line==fCsInum) break;
        }
        if(line<fCsInum) lk_info << Form("CsInum check: %d/%d",line-1,fCsInum) << endl;
        mapCsI.close();
        mapCsI.clear();
    }

    return true;
}


void TTRootConversionTask::Exec(Option_t *option) 
{
    fInputTree -> GetEntry(fRun->GetCurrentEventID());
    
    for (int iHit = 0; iHit < fmmMult; ++iHit)
    {
        /*
        int channelID = fmmChan[iHit];
        int channelID2 = 0;

             if(                channelID<11) channelID2 = channelID;
        else if(channelID>11 && channelID<22) channelID2 = channelID - 1;
        else if(channelID>22 && channelID<45) channelID2 = channelID - 2;
        else if(channelID>45 && channelID<56) channelID2 = channelID - 3;
        else if(channelID>56                ) channelID2 = channelID - 4;
        */

        auto channel = (MMChannel *) fChannelArray -> ConstructedAt(iHit);
        channel -> SetFrameNo(fmmFrameNo[iHit]);
        channel -> SetDecayNo(fmmDecayNo[iHit]);
        channel -> SetCobo(fmmCobo[iHit]);
        channel -> SetAsad(fmmAsad[iHit]);
        channel -> SetAget(fmmAget[iHit]);
        channel -> SetChan(fmmChan[iHit]);
        channel -> SetTime(fmmTime[iHit]);
        channel -> SetEnergy(fmmEnergy[iHit]);
        channel -> SetWaveformX(fmmWaveformX[iHit]);
        channel -> SetWaveformY(fmmWaveformY[iHit]);

        //if (iHit==0 || iHit==1) channel -> Print();
    }

    //lk_debug << fmmMult << " " << fmmChan[10] << " " << fmmWaveformY[10][0] << endl;
    
    lk_info << "TTRootConversionTask container found " << fChannelArray -> GetEntriesFast() << " channels" << std::endl;
}

bool TTRootConversionTask::EndOfRun() 
{
    return true;
}
