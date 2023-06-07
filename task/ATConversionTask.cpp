#include "ATConversionTask.h"

ClassImp(ATConversionTask);

ATConversionTask::ATConversionTask()
:LKTask("ATConversionTask","")
{
}

bool ATConversionTask::Init() 
{
    fInputFileName = fPar -> GetParString("ATConversionTask/inputFile");
    fInputFile = new TFile(fInputFileName, "read");
    fInputTree = (TTree*) fInputFile -> Get("TEvent");

    //SetBranch()
    {
        //Int_t   mmMul;
        //Int_t   mmHit;
        //Int_t   mmEventIdx;
        //Int_t   mmFrameNo[1030];   //[mmMul]
        //Int_t   mmDecayNo[1030];   //[mmMul]
        //Int_t   mmCobo[1030];   //[mmMul]
        //Int_t   mmAsad[1030];   //[mmMul]
        //Int_t   mmAget[1030];   //[mmMul]
        //Int_t   mmChan[1030];   //[mmMul]
        //Float_t mmTime[1030];   //[mmMul]
        //Float_t mmEnergy[1030];   //[mmMul]
        //Int_t   mmWaveformX[1030][512];   //[mmMul][time]
        //Int_t   mmWaveformY[1030][512];   //[mmMul][time]
        fInputTree -> SetBranchAddress("mmMul",&mmMul);
        //fInputTree -> SetBranchAddress("mmHit",&mmHit);
        //fInputTree -> SetBranchAddress("mmEventIdx",&mmEventIdx);
        //fInputTree -> SetBranchAddress("mmFrameNo",mmFrameNo);
        //fInputTree -> SetBranchAddress("mmDecayNo",mmDecayNo);
        fInputTree -> SetBranchAddress("mmCobo",mmCobo);
        fInputTree -> SetBranchAddress("mmAsad",mmAsad);
        fInputTree -> SetBranchAddress("mmAget",mmAget);
        fInputTree -> SetBranchAddress("mmChan",mmChan);
        //fInputTree -> SetBranchAddress("mmTime",mmTime);
        //fInputTree -> SetBranchAddress("mmEnergy",mmEnergy);
        //fInputTree -> SetBranchAddress("mmWaveformX",mmWaveformX);
        fInputTree -> SetBranchAddress("mmWaveformY",mmWaveformY);
        Int_t fNumEvents = fInputTree -> GetEntries();
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
            type[i][j][k][l]=-1;
            DetLoc[i][j][k][l]=-1;
        }

        // MMS: 0 = Left Strip | 1 = Right Strip | 2 = Left Chain | 3 = Right Chain | 4 = Low Center | 5 = High Center
        Int_t line = 0;
        TString mapmmFileName = fPar -> GetParString("ATConversionTask/mapmm");
        ifstream mapmm(mapmmFileName);
        if(mapmm.fail()==true) lk_error << "error: mapchantomm " << mapmmFileName << endl;
        while(mapmm.good())
        {
            mapmm >> mmasad[line] >> mmaget[line] >> mmdchan[line] >> mmx[line] >> mmy[line];
            //lk_debug << Form("%dth %d_%d_%d_%d",line,0,mmasad[line],mmaget[line],mmdchan[line]) << endl;
            line++;
            if(line==mmnum) break;
        }
        if(line<mmnum) lk_info << Form("mmnum check: %d/%d",line-1,mmnum) << endl;
        mapmm.close();
        mapmm.clear();

        for(Int_t i=0; i<mmnum; i++)
        {
            mmpx[mmasad[i]][mmaget[i]][mmdchan[i]]=mmx[i];
            mmpy[mmasad[i]][mmaget[i]][mmdchan[i]]=mmy[i];
            if(mmx[i]==-1)
            {
                if(mmasad[i]==2) type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kLeftStrip;
                else type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kRightStrip;
            }
            else
            {
                if(mmx[i]>=0 && mmx[i]<64) 
                {
                    type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kLeftChain;
                }
                else if(mmx[i]>69)
                {
                    type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kRightChain;
                }
                else if(mmx[i]>=65 && mmx[i]<69) 
                {
                    if(mmasad[i]==0 && mmaget[i]==3) type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kHighCenter;
                    else type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kLowCenter;
                }
                else if(mmx[i]==64 || mmx[i]==69)
                {
                    if(mmaget[i]==3)
                    {
                        if(mmdchan[i]>15) type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kLowCenter;
                        else type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kHighCenter;
                    }
                    else
                    {
                        type[0][mmasad[i]][mmaget[i]][mmdchan[i]] = kLowCenter;
                    }
                }
            }
        }

        // Si + CsI + MMJr: 6 = Forward Si | 7 = Forward CsI | 8 = MMJr | 10 = CENS X6 | 11 = CENS CSI
        // Forward Si
        line = 0;
        ifstream mapsi;
        TString mapsiFileName = fPar -> GetParString("ATConversionTask/mapsi");
        mapsi.open(mapsiFileName);
        if(mapsi.fail()==true) lk_error << "error: mapchantosi " << mapsiFileName << endl;
        while(mapsi.good())
        {
            mapsi >> siasad[line] >> siaget[line] >> sichan[line] >> six[line] >> siy[line] >> sipos[line];
            //cout << Form("%d: %d_%d_%d_%d_%d_%d",line,siasad[line],siaget[line],sichan[line],six[line],siy[line],sipos[line]) << endl;
            sipx[siasad[line]][siaget[line]][sichan[line]] = six[line];
            sipy[siasad[line]][siaget[line]][sichan[line]] = siy[line];
            sistrip[siasad[line]][siaget[line]][sichan[line]] = sipos[line];
            sidet[siasad[line]][siaget[line]][sichan[line]] = 2*(4-six[line])+(1-siy[line]);
            type[1][siasad[line]][siaget[line]][sichan[line]] = kForwardSi;

                 if(six[line]==2)                 DetLoc[1][siasad[line]][siaget[line]][sichan[line]] = kCenterFront;
            else if(six[line]==0 || six[line]==1) DetLoc[1][siasad[line]][siaget[line]][sichan[line]] = kLeft;
            else if(six[line]==3 || six[line]==4) DetLoc[1][siasad[line]][siaget[line]][sichan[line]] = kRight;

            line++;
            if(line==sinum) break;
        }
        if(line<sinum) lk_info << Form("sinum check: %d/%d",line-1,sinum) << endl;
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
        type[1][1][0][2]  = kForwardCsI;
        type[1][1][0][7]  = kForwardCsI;
        type[1][1][0][10] = kForwardCsI;
        type[1][1][0][16] = kForwardCsI;
        type[1][1][0][19] = kForwardCsI;
        type[1][1][0][25] = kForwardCsI;
        type[1][1][0][36] = kForwardCsI;
        type[1][1][0][41] = kForwardCsI;
        DetLoc[1][1][0][2]  = kRight; //0
        DetLoc[1][1][0][7]  = kRight; //1
        DetLoc[1][1][0][10] = kRight; //3
        DetLoc[1][1][0][16] = kRight; //2
        DetLoc[1][1][0][19] = kCenterFront; //4
        DetLoc[1][1][0][25] = kCenterFront; //5
        DetLoc[1][1][0][28] = kLeft; //7
        DetLoc[1][1][0][33] = kLeft; //6
        DetLoc[1][1][0][36] = kLeft; //8
        DetLoc[1][1][0][41] = kLeft; //9

        // External inputs
        type[1][1][2][2]  = kExternal; //SBD
        type[1][1][2][7]  = kExternal; //BM1
        type[1][1][2][10] = kExternal; //BM2
        type[1][1][2][16] = kExternal; //
        type[1][1][2][19] = kExternal; //BM shaping | BM1
        type[1][1][2][25] = kExternal; // | BM2
        type[1][1][2][33] = kExternal; // | BM shaping
        type[1][1][3][2]  = kExternal; //RF-PPACa
        type[1][1][3][7]  = kExternal; //PPACa-PPACb
        type[1][1][3][10] = kExternal; //BM-PPACa
        type[1][1][3][16] = kExternal; //BM raw
        type[1][1][3][19] = kExternal; //L0

        // CENS X6
        line = 0;
        ifstream mapX6;
        TString mapX6FileName = fPar -> GetParString("ATConversionTask/mapX6");
        mapX6.open(mapX6FileName);
        if(mapX6.fail()==true) lk_error << "error: mapchantoX6 " << mapX6FileName << endl;
        while(mapX6.good())
        {
            mapX6 >> X6asad[line] >> X6aget[line] >> X6chan[line] >> X6flag[line] >> X6detnum[line] >> X6pos[line];
            X6det[X6asad[line]][X6aget[line]][X6chan[line]] = X6detnum[line];
            if(X6flag[line]==0) X6strip[X6asad[line]][X6aget[line]][X6chan[line]] = (Int_t) (X6pos[line]+1)/2;
            else if(X6flag[line]==1) X6strip[X6asad[line]][X6aget[line]][X6chan[line]] = X6pos[line];
            X6ud[X6asad[line]][X6aget[line]][X6chan[line]] = (X6pos[line]+1)%2;
            //cout << X6asad[line] << " " <<  X6aget[line] << " " <<  X6chan[line] << " " << X6det[X6asad[line]][X6aget[line]][X6chan[line]] << " " << X6strip[X6asad[line]][X6aget[line]][X6chan[line]] << " " << X6ud[X6asad[line]][X6aget[line]][X6chan[line]] << " " << endl;
            type[2][X6asad[line]][X6aget[line]][X6chan[line]] = kCENSX6;

                 if(X6asad[line]==0 && X6aget[line]!=3) DetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = kBottomRightX6;
            else if(X6asad[line]==1 && X6aget[line]!=3) DetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = kBottomLeftX6;
            else if(X6asad[line]==2 && X6aget[line]!=3) DetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = kLeft;
            else if(X6asad[line]==3 && X6aget[line]!=3) DetLoc[2][X6asad[line]][X6aget[line]][X6chan[line]] = kRight;

            line++;
            if(line==X6num) break;
        }
        if(line<X6num) lk_info << Form("X6num check: %d/%d",line-1,X6num) << endl;
        mapX6.close();
        mapX6.clear();

        // CENS CsI
        line = 0;
        ifstream mapCsI;
        TString mapCsIFileName = fPar -> GetParString("ATConversionTask/mapCsI");
        mapCsI.open(mapCsIFileName);
        if(mapCsI.fail()==true) lk_error << "error: mapchantoCsI " << mapCsIFileName << endl;
        while(mapCsI.good())
        {
            mapCsI >> CsIasad[line] >> CsIaget[line] >> CsIchan[line] >> CsICTnum[line] >> CsIpinflag[line] >> CsItoX6det[line];
            CsICT[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsICTnum[line];
            CsIpin[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsIpinflag[line];
            CsIX6det[CsIasad[line]][CsIaget[line]][CsIchan[line]] = CsItoX6det[line];

            type[2][CsIasad[line]][CsIaget[line]][CsIchan[line]] = kCENSCsI;
            line++;
            if(line==CsInum) break;
        }
        if(line<CsInum) lk_info << Form("CsInum check: %d/%d",line-1,CsInum) << endl;
        mapCsI.close();
        mapCsI.clear();
    }

    return true;
}

void ATConversionTask::Exec(Option_t*)
{
}

bool ATConversionTask::EndOfRun()
{
    /*
    TH2D* timing_dt_xy = new TH2D("timing_dt_xy","Y(vertical) vs Chain location;mmpx;dT(Beam-Chain)",134,0,134,300,-150,150);
    TH2D* timing_dt_zy = new TH2D("timing_dt_zy","Y(vertical) vs Strip location;mmpy;dT(Beam-Strip)",128,0,128,300,-150,150);
    TH2D* timing_dt_xz = new TH2D("timing_dt_xz","Chain location vs Strip location;mmpx;mmpy",134,0,134,64,0,128);
    Int_t track_px[512], track_py[512];
    for(Int_t i=0; i<512; i++) track_px[i] = 9999;
    for(Int_t i=0; i<512; i++) track_py[i] = 9999;

    Double_t theta, costheta, sintheta;

    // Beam setup
    Int_t Beam_med = 180; //CRIB
    Int_t Beam_er = 40; //0822
    Int_t Bi = Beam_med-Beam_er;
    Int_t Bf = Beam_med+Beam_er;
    Int_t Bw = 150; //0822 chain width
    Int_t BwT = 100; //0822 chain width for find timing

    LeftorRight_new(evt, evt+1);
    //test_LeftorRight(evt, evt+1);
    Int_t whichtype[2];
    char side[16];
    if(SiBLR[evt]==0) 
    {
        strcpy(side, "Left");
        whichtype[0] = 0;
        whichtype[1] = 2;
    }
    else if(SiBLR[evt]==1) 
    {
        strcpy(side, "Right");
        whichtype[0] = 1;
        whichtype[1] = 3;
    }
    else if(SiBLR[evt]==9) 
    {
        return;
    }
    char updown[10];
    Int_t fchan;

    Int_t Timing[6];
    TH1D* LCWaveFormbyPixel[6];
    for(Int_t i=0; i<6; i++) LCWaveFormbyPixel[i] = new TH1D(Form("LCenter%d_%d",i+64,evt),Form("LC%dWaveForm_%d",i+64,evt),2*Beam_er,Bi,Bf);

    TH1D* HWaveFormbyPixel[64];
    for(Int_t i=0; i<64; i++) HWaveFormbyPixel[i] = new TH1D(Form("H_%d_%d",i,evt),Form("H_%d",evt),512,0,512);

    fInputTree -> GetEntry(evt);

    for(Int_t hit=0; hit<mmMul; hit++)
    {
        chan = mmChan[hit];
        dchan = 0;
             if(           chan<11) dchan = chan;
        else if(chan>11 && chan<22) dchan = chan - 1;
        else if(chan>22 && chan<45) dchan = chan - 2;
        else if(chan>45 && chan<56) dchan = chan - 3;
        else if(chan>56           ) dchan = chan - 4;

        if(mmCobo[hit]==0 && !(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
        {
            if(type[0][mmAsad[hit]][mmAget[hit]][dchan]==kLowCenter) 
            {
                if(mmpy[mmAsad[hit]][mmAget[hit]][dchan]<64)
                {
                         if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==64) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[0] -> Fill(buck,mmWaveformY[hit][buck]);
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==65) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[1] -> Fill(buck,mmWaveformY[hit][buck]);
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==66) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[2] -> Fill(buck,mmWaveformY[hit][buck]);
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==67) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[3] -> Fill(buck,mmWaveformY[hit][buck]);
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==68) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[4] -> Fill(buck,mmWaveformY[hit][buck]);
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==69) for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[5] -> Fill(buck,mmWaveformY[hit][buck]);
                }
            }
        }
        strcpy(updown, "Down");
    }

    // ================================================== Beam candidates timing
    for(Int_t i=0; i<6; i++) {
        Timing[i] = Bi + LCWaveFormbyPixel[i] -> GetMaximumBin();
        if(Timing[i]==Bi+1) Timing[i]=0;
    }

    // ================================================== Find timing; using 6 chains, 60 timebin window
    Int_t ChainT[6]; // [beam]
    for(Int_t i=0; i<6; i++) ChainT[i] = 0;
    Int_t loc_chain;
    Int_t BeamT, whereisbeam;
    Int_t mintiming = 9999;

    Int_t num_chain = 0;
    Int_t whereisx, wheretox;
    if(whichtype[1]==2)
    {
        whereisx = 63;
        wheretox = -1;
    }
    else if(whichtype[1]==3)
    {
        whereisx = 70;
        wheretox = 1;
    }

    while(1)
    {
        if(num_chain==6 || whereisx==0 || whereisx==133) break;
        else
        {
            for(Int_t hit=0; hit<mmMul; hit++)
            {
                chan = mmChan[hit];
                dchan = 0;
                if(           chan<11) dchan = chan;
                else if(chan>11 && chan<22) dchan = chan - 1;
                else if(chan>22 && chan<45) dchan = chan - 2;
                else if(chan>45 && chan<56) dchan = chan - 3;
                else if(chan>56           ) dchan = chan - 4;

                if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==whereisx && !(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_chain] -> Fill(buck,mmWaveformY[hit][buck]);
                    HWaveFormbyPixel[num_chain] -> SetTitle(Form("H%d_%d_%d_%d/%d/%d",mmCobo[hit],mmAsad[hit],mmAget[hit],dchan,mmpx[mmAsad[hit]][mmAget[hit]][dchan],evt));
                    if(num_chain==0) loc_chain = mmpx[mmAsad[hit]][mmAget[hit]][dchan];
                    num_chain++;
                    continue;
                }
            }
            whereisx = whereisx + wheretox;
            continue;
        }
    }
    if(num_chain==0) return;

    TH1D *HWaveFormbyPixel_temp = new TH1D(Form("H_%dt",evt),Form("H_%dt",evt),512,0,512);
    for(Int_t j=0; j<num_chain; j++) //chain
    {
        HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone();
        for(Int_t i=0; i<6; i++) //timing
        {
            if(strcmp(updown,"Up")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(Timing[i]-BwT,Timing[i]);
            else if(strcmp(updown,"Down")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(Timing[i],Timing[i]+BwT);

            ChainT[i] = ChainT[i] + HWaveFormbyPixel_temp -> GetMaximumBin();
            if(Timing[i]==0) ChainT[i] = 0;
        }
        HWaveFormbyPixel_temp->Reset();
    }
    //cout << "where?" << endl;
    for(Int_t i=0; i<6; i++)
    {
        ChainT[i] = ChainT[i]/num_chain;
        if( ((strcmp(updown,"Up")==0&&(ChainT[i]-Timing[i])<0) || (strcmp(updown,"Down")==0&&(ChainT[i]-Timing[i])>0)) && TMath::Abs(ChainT[i]-Timing[i])<mintiming && Timing[i]!=0)
        {
            mintiming = TMath::Abs(ChainT[i]-Timing[i]);
            BeamT = Timing[i];
            whereisbeam = i;
        }
    }
    //cout << "where?" << endl;
    lk_info << evt << ": " << Timing[whereisbeam]-ChainT[whereisbeam] << " at " << loc_chain << " BT" << endl;
    //cout << "where?" << endl;

    // ================================================== Draw Y vs Chain
    Int_t ChainT_all[64]; // [chain]
    for(Int_t j=0; j<64; j++) ChainT_all[j] = 0;
    Int_t loc_chain_all[64];
    num_chain = 0;
    if(whichtype[1]==2)
    {
        whereisx = 63;
        wheretox = -1;
    }
    else if(whichtype[1]==3)
    {
        whereisx = 70;
        wheretox = 1;
    }

    while(1)
    {
        //if(num_chain==1 || whereisx==0 || whereisx==133) break;
        if(whereisx==0 || whereisx==133) break;
        else
        {
            for(Int_t hit=0; hit<mmMul; hit++)
            {
                chan = mmChan[hit];
                dchan = 0;
                if(           chan<11) dchan = chan;
                else if(chan>11 && chan<22) dchan = chan - 1;
                else if(chan>22 && chan<45) dchan = chan - 2;
                else if(chan>45 && chan<56) dchan = chan - 3;
                else if(chan>56           ) dchan = chan - 4;

                if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==whereisx && !(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_chain] -> Fill(buck,mmWaveformY[hit][buck]);
                    loc_chain_all[num_chain] = mmpx[mmAsad[hit]][mmAget[hit]][dchan];
                    num_chain++;
                    continue;
                }
            }
            whereisx = whereisx + wheretox;
            continue;
        }
    }
    if(num_chain==0) return;

    for(Int_t j=0; j<num_chain; j++) //chain
    {
        HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone();
        if(strcmp(updown,"Up")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(BeamT-Bw,BeamT);
        else if(strcmp(updown,"Down")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(BeamT,BeamT+Bw);

        ChainT_all[j] = HWaveFormbyPixel_temp -> GetMaximumBin();
        if(BeamT==0) 
        {
            ChainT_all[j] = 0;
        }
        HWaveFormbyPixel_temp->Reset();
    }
    for(Int_t i=0; i<num_chain; i++)
    {
        timing_dt_xy -> Fill(loc_chain_all[i],(BeamT-ChainT_all[i]));
        track_px[BeamT-ChainT_all[i]+256] = loc_chain_all[i];
        lk_info << evt << ": " << BeamT-ChainT_all[i] << " at " << loc_chain_all[i] << endl;
    }
    for(Int_t i=0; i<64; i++) HWaveFormbyPixel[i] -> Reset();

    //modify timing_dt_xy
    //for(Int_t i=0; i<num_chain; i++)
    //{
    //    for(Int_t time=-150; time<150; time++)
    //    {
    //        if((timing_dt_xy -> ProjectionY() -> GetBinContent(time+151))>3 && (BeamT-ChainT_all[i])==time) ChainT_all[i]=9999;
    //    }
    //}
    for(Int_t time=-150; time<150; time++)
    {
        if((timing_dt_xy -> ProjectionY() -> GetBinContent(time+151))>3)
        {
            for(Int_t i=0; i<num_chain; i++) if((BeamT-ChainT_all[i])==time) ChainT_all[i]=9999;
        }
    }
    timing_dt_xy -> Reset();
    for(Int_t i=0; i<num_chain; i++)
    {
        if(ChainT_all[i]!=9999) timing_dt_xy -> Fill(loc_chain_all[i],(BeamT-ChainT_all[i]));
    }

    Double_t rad_xt;
    TH2D* Hough_xt = new TH2D("Hough_xt","Hough_xt;theta;radius",1800,0,180,268,-134,134);
    for(Int_t i=0; i<num_chain; i++)
    {
        if(ChainT_all[i]!=9999)
        {
            for(theta=0; theta<180; theta+=0.1)
            {
                costheta = TMath::Cos(theta*TMath::DegToRad());
                sintheta = TMath::Sin(theta*TMath::DegToRad());
                rad_xt = loc_chain_all[i]*costheta + (BeamT-ChainT_all[i])*sintheta;
                Hough_xt -> Fill(theta, rad_xt);
            }
        }
    }
    Double_t maxbinxt =-9999;
    Double_t thetaxt = 0;
    Double_t radxt = 0;
    Int_t numxt = 0;
    for(Int_t i=0; i<1800; i++)
    {
        if((Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximum())>=maxbinxt)
        {
            //numxt++;
            maxbinxt = Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximum();
            thetaxt =  Hough_xt -> GetXaxis() -> GetBinCenter(i);
            //thetaxt = (thetaxt*(numxt-1) + Hough_xt -> GetXaxis() -> GetBinCenter(i))/numxt;
            radxt =  Hough_xt -> GetYaxis() -> GetBinCenter(Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin());
            //radxt = (radxt*(numxt-1) + Hough_xt -> GetYaxis() -> GetBinCenter(Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin()))/numxt;
        }
    }
    lk_info << thetaxt << " " << radxt << endl;
    Double_t tanxt = TMath::Tan(thetaxt*TMath::DegToRad());
    Double_t sinxt = TMath::Sin(thetaxt*TMath::DegToRad());
    Double_t cosxt = TMath::Cos(thetaxt*TMath::DegToRad());
    TH2D* fhough_xt = new TH2D("fhough_xt","fhough_xt;mmpx;dT(Beam-Chain)",134,0,134,300,-150,150);
    for(Double_t i=-150; i<150; i+=0.1) if(-i*tanxt+radxt*(cosxt+sinxt*tanxt)>0 && -i*tanxt+radxt*(cosxt+sinxt*tanxt)<134) fhough_xt -> Fill(-i*tanxt+radxt*(cosxt+sinxt*tanxt),i);
    for(Int_t i=0; i<num_chain; i++)
    {
        fhough_xt -> Fill(loc_chain_all[i],(BeamT-ChainT_all[i]),100);
    }
    // ================================================== Draw Y vs Chain

    // ================================================== Draw Y vs Strip
    Int_t StripT_all[128]; // [strip]
    for(Int_t j=0; j<128; j++) StripT_all[j] = 0;
    Int_t loc_strip_all[128];
    Int_t num_strip = 0;
    Int_t whereisz = 0;
    Int_t wheretoz = 2;

    while(1)
    {
        //if(num_strip==1 || whereisx==0 || whereisx==133) break;
        if(whereisz==128) break; //should be changed depend on L/R since R is soooo noisy
        else
        {
            for(Int_t hit=0; hit<mmMul; hit++)
            {
                chan = mmChan[hit];
                dchan = 0;
                if(           chan<11) dchan = chan;
                else if(chan>11 && chan<22) dchan = chan - 1;
                else if(chan>22 && chan<45) dchan = chan - 2;
                else if(chan>45 && chan<56) dchan = chan - 3;
                else if(chan>56           ) dchan = chan - 4;

                if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==-1 && mmpy[mmAsad[hit]][mmAget[hit]][dchan]==whereisz 
                        && mmAsad[hit]==whichtype[1] && !(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
                {
                    HWaveFormbyPixel[num_strip] -> Reset();
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_strip] -> Fill(buck,mmWaveformY[hit][buck]);
                    loc_strip_all[num_strip] = mmpy[mmAsad[hit]][mmAget[hit]][dchan];
                    num_strip++;
                    continue;
                }
            }
            whereisz = whereisz + wheretoz;
            continue;
        }
    }
    if(num_strip==0) return;

    for(Int_t j=0; j<num_strip; j++) //strip
    {
        HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone();
        if(strcmp(updown,"Up")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(BeamT-Bw,BeamT);
        else if(strcmp(updown,"Down")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(BeamT,BeamT+Bw);

        StripT_all[j] = HWaveFormbyPixel_temp -> GetMaximumBin();
        if(BeamT==0) StripT_all[j] = 0;
        HWaveFormbyPixel_temp->Reset();
    }
    for(Int_t i=0; i<num_strip; i++)
    {
        timing_dt_zy -> Fill(loc_strip_all[i],(BeamT-StripT_all[i]));
        track_py[BeamT-StripT_all[i]+256] = loc_strip_all[i];
        lk_info << evt << ": " << BeamT-StripT_all[i] << " at " << loc_strip_all[i] << endl;
    }

    //modify timing_dt_zy
    //for(Int_t i=0; i<num_strip; i++)
    //{
    //    for(Int_t time=-150; time<150; time++)
    //    {
    //        if((timing_dt_zy -> ProjectionY() -> GetBinContent(time+151))>3 && (BeamT-StripT_all[i])==time) StripT_all[i]=9999;
    //    }
    //}
    for(Int_t time=-150; time<150; time++)
    {
        if((timing_dt_zy -> ProjectionY() -> GetBinContent(time+151))>3)
        {
            for(Int_t i=0; i<num_strip; i++) if((BeamT-StripT_all[i])==time) StripT_all[i]=9999;
        }
    }
    timing_dt_zy -> Reset();
    for(Int_t i=0; i<num_strip; i++)
    {
        if(StripT_all[i]!=9999) timing_dt_zy -> Fill(loc_strip_all[i],(BeamT-StripT_all[i]));
    }

    Double_t rad_zt;
    TH2D* Hough_zt = new TH2D("Hough_zt","Hough_zt;theta;radius",1800,0,180,256,-128,128);
    for(Int_t i=0; i<num_strip; i++)
    {
        if(StripT_all[i]!=9999)
        {
            for(theta=0; theta<180; theta+=0.1)
            {
                costheta = TMath::Cos(theta*TMath::DegToRad());
                sintheta = TMath::Sin(theta*TMath::DegToRad());
                rad_zt = loc_strip_all[i]*costheta + (BeamT-StripT_all[i])*sintheta;
                Hough_zt -> Fill(theta, rad_zt);
            }
        }
    }
    Double_t maxbinzt =-9999;
    Double_t thetazt = 0;
    Double_t radzt = 0;
    Int_t numzt = 0;
    for(Int_t i=0; i<1800; i++)
    {
        if((Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximum())>=maxbinzt)
        {
            //numzt++;
            maxbinzt = Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximum();
            thetazt =  Hough_zt -> GetXaxis() -> GetBinCenter(i);
            //thetazt = (thetazt*(numzt-1) + Hough_zt -> GetXaxis() -> GetBinCenter(i))/numzt;
            radzt =  Hough_zt -> GetYaxis() -> GetBinCenter(Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin());
            //radzt = (radzt*(numzt-1) + Hough_zt -> GetYaxis() -> GetBinCenter(Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin()))/numzt;
        }
    }
    lk_info << thetazt << " " << radzt << endl;
    Double_t tanzt = TMath::Tan(thetazt*TMath::DegToRad());
    Double_t sinzt = TMath::Sin(thetazt*TMath::DegToRad());
    Double_t coszt = TMath::Cos(thetazt*TMath::DegToRad());
    //TF1* fhough_zt = new TF1("fhough_zt","[0]*x+[1]",0,128);
    //fhough_zt -> SetParameter(0,-1/tanzt);
    //fhough_zt -> SetParameter(1,radzt*(coszt/tanzt+sinzt));
    TH2D* fhough_zt = new TH2D("fhough_zt","fhough_zt;mmpx;dT(Beam-Chain)",128,0,128,300,-150,150);
    for(Double_t i=-150; i<150; i+=0.1) if(-i*tanzt+radzt*(coszt+sinzt*tanzt)>0 && -i*tanzt+radzt*(coszt+sinzt*tanzt)<128) fhough_zt -> Fill(-i*tanzt+radzt*(coszt+sinzt*tanzt),i);
    for(Int_t i=0; i<num_strip; i++)
    {
        fhough_zt -> Fill(loc_strip_all[i],(BeamT-StripT_all[i]),100);
    }
    // ================================================== Draw Y vs Strip

    for(Int_t i=0; i<512; i++)
    {
        if(track_px[i]<9999)
        {
            for(Int_t j=-1; j<1; j++)
            {
                if(track_py[i+j]<9999) timing_dt_xz -> Fill(track_px[i],track_py[i+j]);
            }
        }
    }

    for(Int_t i=0; i<6; i++) LCWaveFormbyPixel[i] -> Delete(); 
    for(Int_t i=0; i<64; i++) HWaveFormbyPixel[i] -> Delete();

    //TF1* fhough_xz = new TF1("fhough_xz","[0]*x+[1]",0,134);
    //fhough_xz -> SetParameter(0,tanxt/tanzt);
    //fhough_xz -> SetParameter(1,radxt*(-sinxt*tanxt/tanzt - cosxt/tanzt)+radzt*(coszt/tanzt + sinzt));
    TH2D* fhough_xz = new TH2D("fhough_xz","fhough_xz;mmpx;mmpy",134,0,134,128,0,128);
    for(Double_t i=-150; i<150; i+=0.1) if((-i*tanxt+radxt*(cosxt+sinxt*tanxt)>0 && -i*tanxt+radxt*(cosxt+sinxt*tanxt)<134)&&(-i*tanzt+radzt*(coszt+sinzt*tanzt)>0 && -i*tanzt+radzt*(coszt+sinzt*tanzt)<128)) fhough_xz -> Fill(-i*tanxt+radxt*(cosxt+sinxt*tanxt),-i*tanzt+radzt*(coszt+sinzt*tanzt));

    TH2D* fhough_xz_check = new TH2D("fhough_xz_check","fhough_xz_check;mmpx;mmpy",134,0,134,128,0,128);
    for(Double_t i=0; i<134; i+=0.1) fhough_xz_check -> Fill(i,i*(tanzt/tanxt)-radxt*(tanzt*cosxt/tanxt+tanzt*sinxt)+radzt*(coszt+sinzt*tanzt));

    */
    return true;
}

/*
void ATConversionTask::LeftorRight_new(Int_t evtbegin, Int_t evtend)
{
    // output: SiBLR 0 for left | 1 for right | 2 for front center | 3 for more than 1 event | 9 for bad

    Int_t goodevt = 0;
    Int_t siLhit, siRhit, siChit, X6Lhit, X6Rhit;

    for(Int_t evt=evtbegin; evt<evtend; evt++)
    {
        fInputTree -> GetEntry(evt);
        GoodSiEntry[evt] = 9999;
        siLhit = 0;
        siRhit = 0;
        siChit = 0;
        X6Lhit = 0;
        X6Rhit = 0;
        for(Int_t hit=0; hit<mmMul; hit++)
        {
            if(!(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
            {
                if(type[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==kForwardSi)
                {
                         if(DetLoc[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==kLeft) siLhit++;
                    else if(DetLoc[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==kRight) siRhit++;
                    else if(DetLoc[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==kCenterFront) siChit++;
                }
                else if(type[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==kCENSX6)
                {
                         if(DetLoc[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]%10==0) X6Lhit++;
                    else if(DetLoc[mmCobo[hit]][mmAsad[hit]][mmAget[hit]][mmChan[hit]]%10==1) X6Rhit++;
                }
            }
        }

        // not considered about center,,,
             if(siLhit==2) { if(siRhit<2 && X6Lhit<3 && X6Rhit<3) SiBLR[evt]=0; }
        else if(siRhit==2) { if(siLhit<2 && X6Lhit<3 && X6Rhit<3) SiBLR[evt]=1; }
        else if(X6Lhit==3) { if(siLhit<2 && siRhit<3 && X6Rhit<3) SiBLR[evt]=0; }
        else if(X6Rhit==3) { if(siLhit<2 && siRhit<3 && X6Lhit<3) SiBLR[evt]=1; }
        else if(siChit==2) SiBLR[evt]=2;
        else SiBLR[evt] = 9;

        lk_info << Form("siL: %d | siR: %d | siC: %d | X6L: %d | X6R: %d  ->  %d", siLhit, siRhit, siChit, X6Lhit, X6Rhit, SiBLR[evt]) << endl;
        if(SiBLR[evt]!=9) goodevt++;
    }
    lk_info << "goodevt " << goodevt << endl;
}
*/
