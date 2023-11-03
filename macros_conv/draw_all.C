LKRun* fRun = nullptr;
TexAT2* fDetector = nullptr;
TClonesArray* fChannelArray = nullptr;
TClonesArray* fEventHeaderArray = nullptr;
int MaxBuck = 512;

void draw_mm(Int_t eventID=10) // test_DrawMMS
{
    TH2D* HLWF[2][2];
    HLWF[0][0] = new TH2D("Left_strip_up"  ,"Left_strip_up",512,0,512,512,0,4096);
    HLWF[0][1] = new TH2D("Left_strip_down","Left_strip_down",512,0,512,512,0,4096);
    HLWF[1][0] = new TH2D("Left_chain_out" ,"Left_chain_out",512,0,512,512,0,4096);
    HLWF[1][1] = new TH2D("Left_chain_in"  ,"Left_chain_in",512,0,512,512,0,4096);
    TH2D* HRWF[2][2];
    HRWF[0][0] = new TH2D("Right_strip_up"  ,"Right_strip_up",512,0,512,512,0,4096);
    HRWF[0][1] = new TH2D("Right_strip_down","Right_strip_down",512,0,512,512,0,4096);
    HRWF[1][0] = new TH2D("Right_chain_out" ,"Right_chain_out",512,0,512,512,0,4096);
    HRWF[1][1] = new TH2D("Right_chain_in"  ,"Right_chain_in",512,0,512,512,0,4096);
    TH2D* CSWF[2][2];
    CSWF[0][0] = new TH2D("CenterSide_Left_up"   ,"CenterSide_Left_up",512,0,512,512,0,4096);
    CSWF[0][1] = new TH2D("CenterSide_Left_down" ,"CenterSide_Left_down",512,0,512,512,0,4096);
    CSWF[1][0] = new TH2D("CenterSide_Right_up"  ,"CenterSide_Right_up",512,0,512,512,0,4096);
    CSWF[1][1] = new TH2D("CenterSide_Right_down","CenterSide_Right_down",512,0,512,512,0,4096);
    TH2D* CWF[4];
    CWF[0] = new TH2D("Center_L1","Center_L1",512,0,512,512,0,4096);
    CWF[1] = new TH2D("Center_L2","Center_L2",512,0,512,512,0,4096);
    CWF[2] = new TH2D("Center_L3","Center_L3",512,0,512,512,0,4096);
    CWF[3] = new TH2D("Center_H4","Center_H4",512,0,512,512,0,4096);

    Int_t mmMul = fChannelArray -> GetEntries();
    for(Int_t iChannel=0; iChannel<mmMul; iChannel++)
    {
        auto channel = (MMChannel *) fChannelArray -> At(iChannel);
        Int_t chan = channel -> GetChan();
        Int_t dchan = channel -> GetDChan();
        Int_t mmCobo = channel -> GetCobo();
        Int_t mmAsad = channel -> GetAsad();
        Int_t mmAget = channel -> GetAget();
        Int_t *mmWaveformY = channel -> GetWaveformY();

        if(mmCobo==0 && !(chan==11 || chan==22 || chan==45 || chan==56))
        {
            auto type0 = fDetector -> GetType(0,mmAsad,mmAget,dchan);

            if(type0==TexAT2::eType::kLeftStrip) 
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HLWF[0][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HLWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==TexAT2::eType::kLeftChain) 
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HLWF[1][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HLWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==TexAT2::eType::kRightStrip)
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HRWF[0][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HRWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==TexAT2::eType::kRightChain) 
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HRWF[1][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HRWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==TexAT2::eType::kLowCenter) 
            {
                     if((mmAsad==0 || mmAsad==1) && mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[0]     -> Fill(buck,mmWaveformY[buck]); }
                else if((mmAsad==0 || mmAsad==1) && mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[1]     -> Fill(buck,mmWaveformY[buck]); }
                else if((mmAsad==0 || mmAsad==1) && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[2]     -> Fill(buck,mmWaveformY[buck]); }
                else if( mmAsad==2               && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][0] -> Fill(buck,mmWaveformY[buck]); }
                else if( mmAsad==2               && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
                else if( mmAsad==3               && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][0] -> Fill(buck,mmWaveformY[buck]); }
                else if( mmAsad==3               && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==TexAT2::eType::kHighCenter)
            {
                     if(mmAget==3 && (mmAsad==0 || mmAsad==1)) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[3] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==2 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==3 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][1] -> Fill(buck,mmWaveformY[buck]);
                }
            }
        }
    }

    TCanvas* mmcvs = new TCanvas("mmcvs","for MMS only",1600,1600);
    mmcvs -> Divide(4,4);

    gServer -> Register("draw_mm",mmcvs);

    mmcvs -> cd(1); HLWF[0][0] -> Draw("colz");
    mmcvs -> cd(5); HLWF[0][1] -> Draw("colz");
    mmcvs -> cd(9); HLWF[1][0] -> Draw("colz");
    mmcvs -> cd(13); HLWF[1][1] -> Draw("colz");

    mmcvs -> cd(2); HRWF[0][0] -> Draw("colz");
    mmcvs -> cd(6); HRWF[0][1] -> Draw("colz");
    mmcvs -> cd(10); HRWF[1][0] -> Draw("colz");
    mmcvs -> cd(14); HRWF[1][1] -> Draw("colz");

    mmcvs -> cd(3); CSWF[0][0] -> Draw("colz");
    mmcvs -> cd(7); CSWF[0][1] -> Draw("colz");
    mmcvs -> cd(11); CSWF[1][0] -> Draw("colz");
    mmcvs -> cd(15); CSWF[1][1] -> Draw("colz");

    mmcvs -> cd(4); CWF[0] -> Draw("colz");
    mmcvs -> cd(8); CWF[1] -> Draw("colz");
    mmcvs -> cd(12); CWF[2] -> Draw("colz");
    mmcvs -> cd(16); CWF[3] -> Draw("colz");
}

void draw_entry(Int_t evtbegin, Int_t evtend) //ana_DrawWFbyEntry
{
    TH2D* WaveFormbyEvent = new TH2D(Form("All_%d-%d",evtbegin,evtend),Form("WaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* HLWaveFormbyEvent = new TH2D(Form("HLeft_%d-%d",evtbegin,evtend),Form("MMLeft_StripChain HLWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096); //chaen: remove
    TH2D* LCWaveFormbyEvent = new TH2D(Form("LCenter_%d-%d",evtbegin,evtend),Form("MMLowCenter LCWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* LC1WaveFormbyEvent = new TH2D(Form("LCenter1_%d-%d",evtbegin,evtend),Form("Aget1 LC1WaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* LC2WaveFormbyEvent = new TH2D(Form("LCenter2_%d-%d",evtbegin,evtend),Form("Aget2 LC2WaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* LC3WaveFormbyEvent = new TH2D(Form("LCenter3_%d-%d",evtbegin,evtend),Form("Aget3 LC3WaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* LCPxYvsTimebyEvent = new TH2D(Form("LCenterPxYvsTime_%d-%d",evtbegin,evtend),Form("MMCenter LCPxYvsTime_%d-%d",evtbegin,evtend),512,0,512,128,0,128);
    TH2D* HRPxYvsTimebyEvent = new TH2D(Form("HRightPxYvsTime_%d-%d",evtbegin,evtend),Form("MMRightStrip HRPxYvsTime_%d-%d",evtbegin,evtend),512,0,512,64,0,128);
    TH2D* HLPxYvsTimebyEvent = new TH2D(Form("HLeftPxYvsTime_%d-%d",evtbegin,evtend),Form("MMLeftStrip HLPxYvsTime_%d-%d",evtbegin,evtend),512,0,512,64,0,128);
    TH2D* HCWaveFormbyEvent = new TH2D(Form("HCenter_%d-%d",evtbegin,evtend),Form("MMHighCenter HCWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* HRWaveFormbyEvent = new TH2D(Form("HRight_%d-%d",evtbegin,evtend),Form("MMRight_StripChain HRWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* SiWaveFormbyEvent = new TH2D(Form("Si_%d-%d",evtbegin,evtend),Form("SiWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* CsIWaveFormbyEvent = new TH2D(Form("CsI_%d-%d",evtbegin,evtend),Form("CsIWaveForm_%d-%d",evtbegin,evtend),512,0,512,512,0,4096);
    TH2D* MMJrWaveFormbyEvent = new TH2D(Form("MMJr_%d-%d",evtbegin,evtend),Form("MMJrWaveForm_%d-%d",evtbegin,evtend),512,0,512,128,0,1024);
    Int_t MaxVal[1030];
    TH2I* MaxValHitPattern2D = new TH2I("MaxValHitPattern","MaxValHitPattern",6000,0,6000,512,0,8192);

    for(int evtidx=evtbegin;evtidx<=evtend;evtidx++)
    {
        fRun -> GetEvent(evtidx);

        Int_t mmMul = fChannelArray -> GetEntries();
        for(Int_t iChannel=0; iChannel<mmMul; iChannel++)
        {
            auto channel = (MMChannel *) fChannelArray -> At(iChannel);
            Int_t chan = channel -> GetChan();
            Int_t dchan = channel -> GetDChan();
            Int_t mmCobo = channel -> GetCobo();
            Int_t mmAsad = channel -> GetAsad();
            Int_t mmAget = channel -> GetAget();
            Int_t *mmWaveformY = channel -> GetWaveformY();
            for(Int_t buck=0; buck<MaxBuck; buck++)
            {
                WaveFormbyEvent -> Fill(buck,mmWaveformY[buck]);
            } 
        }

        for(Int_t iChannel=0; iChannel<mmMul; iChannel++)
        {
            auto channel = (MMChannel *) fChannelArray -> At(iChannel);
            Int_t chan = channel -> GetChan();
            Int_t dchan = channel -> GetDChan();
            Int_t mmCobo = channel -> GetCobo();
            Int_t mmAsad = channel -> GetAsad();
            Int_t mmAget = channel -> GetAget();
            Int_t *mmWaveformY = channel -> GetWaveformY();

            if(mmCobo==0 && !(chan==11 || chan==22 || chan==45 || chan==56))
            {
                // 0 for strip-left | 1 for strip-right | 2 for chain-left | 3 for chain-right | 4 for low-center | 5 for high-center | 6 for Si | 7 for CsI | 8 for MMJr
                auto type0 = fDetector -> GetType(0,mmAsad,mmAget,dchan);
                auto mmpy = fDetector -> Getmmpy(mmAsad,mmAget,dchan);

                if(type0==TexAT2::eType::kLeftStrip) {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HLWaveFormbyEvent -> Fill(buck,mmWaveformY[buck],10);
                    for(Int_t buck=0; buck<MaxBuck; buck++) HLPxYvsTimebyEvent -> Fill(buck,mmpy,mmWaveformY[buck]);
                }
                else if(type0==TexAT2::eType::kRightStrip) {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HRWaveFormbyEvent -> Fill(buck,mmWaveformY[buck],10);
                    for(Int_t buck=0; buck<MaxBuck; buck++) HRPxYvsTimebyEvent -> Fill(buck,mmpy,mmWaveformY[buck]);
                }
                else if(type0==TexAT2::eType::kLeftChain) {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HLWaveFormbyEvent -> Fill(buck,mmWaveformY[buck],10);
                }
                else if(type0==TexAT2::eType::kRightChain) {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HRWaveFormbyEvent -> Fill(buck,mmWaveformY[buck],10);
                }
                else if(type0==TexAT2::eType::kLowCenter) 
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++)
                        LCWaveFormbyEvent -> Fill(buck,mmWaveformY[buck]);

                    if(mmAsad<2)
                        for(Int_t buck=0; buck<MaxBuck; buck++)
                            LCPxYvsTimebyEvent -> Fill(buck,mmpy,mmWaveformY[buck]);

                         if( (mmAsad==1&&mmAget==0) || (mmAsad==0&&mmAget==0) ) { for(Int_t buck=0; buck<MaxBuck; buck++) LC1WaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
                    else if( (mmAsad==1&&mmAget==1) || (mmAsad==0&&mmAget==1) ) { for(Int_t buck=0; buck<MaxBuck; buck++) LC2WaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
                    else if( (mmAsad==1&&mmAget==2) || (mmAsad==0&&mmAget==2) ) { for(Int_t buck=0; buck<MaxBuck; buck++) LC3WaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
                }
                else if(type0==TexAT2::eType::kHighCenter) 
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++)
                        HCWaveFormbyEvent -> Fill(buck,mmWaveformY[buck]);
                    if(mmAsad<2)
                        for(Int_t buck=0; buck<MaxBuck; buck++)
                            LCPxYvsTimebyEvent -> Fill(buck,mmpy,mmWaveformY[buck]);
                }
            }
            if (0)
            if(mmCobo!=0 && !(chan==11 || chan==22 || chan==45 || chan==56))
            {
                auto typeC = fDetector -> GetType(mmCobo,mmAsad,mmAget,dchan);
                auto type1 = fDetector -> GetType(1,mmAsad,mmAget,dchan);

                if(typeC==TexAT2::eType::kForwardSi) { for(Int_t buck=0; buck<MaxBuck; buck++) SiWaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
                if(typeC==TexAT2::eType::kCENSX6) {
                    for(Int_t buck=0; buck<MaxBuck/2; buck++) SiWaveFormbyEvent -> Fill(buck+MaxBuck/2,mmWaveformY[buck]);
                    for(Int_t buck=MaxBuck/2; buck<MaxBuck; buck++) SiWaveFormbyEvent -> Fill(buck-MaxBuck/2,mmWaveformY[buck]);
                }
                if(type1==TexAT2::eType::kForwardCsI) { for(Int_t buck=0; buck<MaxBuck; buck++) CsIWaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
                if(type1==TexAT2::eType::kMMJr      ) { for(Int_t buck=0; buck<MaxBuck; buck++) MMJrWaveFormbyEvent -> Fill(buck,mmWaveformY[buck]); }
            }
        }

        // Draw HitPattern
        for(Int_t iChannel=0; iChannel<mmMul; iChannel++)
        {
            auto channel = (MMChannel *) fChannelArray -> At(iChannel);
            Int_t chan = channel -> GetChan();
            Int_t dchan = channel -> GetDChan();
            Int_t mmCobo = channel -> GetCobo();
            Int_t mmAsad = channel -> GetAsad();
            Int_t mmAget = channel -> GetAget();
            Int_t *mmWaveformY = channel -> GetWaveformY();

            if(mmCobo==1 && mmAsad==0 && mmAget==0 && chan<64)
            {
                MaxVal[iChannel] = 9999;
                for(Int_t buck=0; buck<MaxBuck; buck++)
                {
                    if(MaxVal[iChannel]>mmWaveformY[buck]) MaxVal[iChannel] = mmWaveformY[buck];
                }
            }
            else
            {
                MaxVal[iChannel] = -9999;
                for(Int_t buck=0; buck<MaxBuck; buck++)
                {
                    if(MaxVal[iChannel]<mmWaveformY[buck]) MaxVal[iChannel] = mmWaveformY[buck];
                }
            }
            MaxValHitPattern2D -> Fill(mmCobo*2000+mmAsad*500+mmAget*100+chan, MaxVal[iChannel]);
        }
    }

    // Draw all by Event number
    TCanvas* cvs_allevt = new TCanvas("allevt","allevt",10,10,1000,800);
    WaveFormbyEvent -> Draw("colz");
    TCanvas* cvs_typeevt = new TCanvas("typeevt","typeevt",10,10,1200,1200);
    cvs_typeevt -> Divide(3,3); // left center right | Si CsI MMJr
    cvs_typeevt -> cd(1);  LC1WaveFormbyEvent  -> Draw("colz");
    cvs_typeevt -> cd(2);  LC2WaveFormbyEvent  -> Draw("colz");
    cvs_typeevt -> cd(3);  LC3WaveFormbyEvent  -> Draw("colz");
    cvs_typeevt -> cd(4);  HLWaveFormbyEvent   -> Draw("colz");
    cvs_typeevt -> cd(5);  HCWaveFormbyEvent   -> Draw("colz");
    cvs_typeevt -> cd(6);  HRWaveFormbyEvent   -> Draw("colz");
    cvs_typeevt -> cd(7);  HLPxYvsTimebyEvent  -> Draw("colz");
    cvs_typeevt -> cd(8);  LCPxYvsTimebyEvent  -> Draw("colz");
    cvs_typeevt -> cd(9);  HRPxYvsTimebyEvent  -> Draw("colz");
    //cvs_typeevt -> cd(10); SiWaveFormbyEvent   -> Draw("colz");
    //cvs_typeevt -> cd(11); CsIWaveFormbyEvent  -> Draw("colz");
    //cvs_typeevt -> cd(12); MMJrWaveFormbyEvent -> Draw("colz");

    TCanvas* cvs_hitpat = new TCanvas("hitpat","hitpat",1000,800);
    cvs_hitpat -> cd();
    MaxValHitPattern2D -> Draw("colz");

    gServer -> Register("draw_entry",cvs_allevt);
    gServer -> Register("draw_entry",cvs_typeevt);
    gServer -> Register("draw_entry",cvs_hitpat);
}

void draw_MMSLR(Int_t evt) // test_DrawMMSLR
{
    TexAT2::eType whichtype[2];
    TH2D* HWF_org;
    TH2D* HWF_nor;
    char side[16];

    auto eventHeader = (TTEventHeader*) fEventHeaderArray -> At(0);
    auto SiBLR = eventHeader -> GetSiBLR();

    if(SiBLR==0) 
    {
        strcpy(side, "Left");
        whichtype[0] = TexAT2::eType::kLeftStrip;
        whichtype[1] = TexAT2::eType::kLeftChain;
    }
    else if(SiBLR==1) 
    {
        strcpy(side, "Right");
        whichtype[0] = TexAT2::eType::kRightStrip;
        whichtype[1] = TexAT2::eType::kRightChain;
    }

    TH2D* HWF[2][2];
    HWF[0][0] = new TH2D(Form("_%s_strip_up_00",side),Form("%s_strip_up",side),512,0,512,512,0,4096);
    HWF[0][1] = new TH2D(Form("_%s_strip_up_01",side),Form("%s_strip_up",side),512,0,512,512,0,4096);
    HWF[1][0] = new TH2D(Form("_%s_strip_up_10",side),Form("%s_strip_up",side),512,0,512,512,0,4096);
    HWF[1][1] = new TH2D(Form("_%s_strip_up_11",side),Form("%s_strip_up",side),512,0,512,512,0,4096);
    TH2D* CSWF[2][2];
    CSWF[0][0] = new TH2D("_CenterSide_Left_up"   ,"CenterSide_Left_up",512,0,512,512,0,4096);
    CSWF[0][1] = new TH2D("_CenterSide_Left_down" ,"CenterSide_Left_down",512,0,512,512,0,4096);
    CSWF[1][0] = new TH2D("_CenterSide_Right_up"  ,"CenterSide_Right_up",512,0,512,512,0,4096);
    CSWF[1][1] = new TH2D("_CenterSide_Right_down","CenterSide_Right_down",512,0,512,512,0,4096);
    TH2D* CWF[4];
    CWF[0] = new TH2D("_Center_L1","Center_L1",512,0,512,512,0,4096);
    CWF[1] = new TH2D("_Center_L2","Center_L2",512,0,512,512,0,4096);
    CWF[2] = new TH2D("_Center_L3","Center_L3",512,0,512,512,0,4096);
    CWF[3] = new TH2D("_Center_H4","Center_H4",512,0,512,512,0,4096);

    Int_t mmMul = fChannelArray -> GetEntries();
    for(Int_t iChannel=0; iChannel<mmMul; iChannel++)
    {
        // 0 for strip-left | 1 for strip-right | 2 for chain-left | 3 for chain-right | 4 for low-center | 5 for high-center | 6 for Si | 7 for CsI | 8 for MMJr

        auto channel = (MMChannel *) fChannelArray -> At(iChannel);
        Int_t chan = channel -> GetChan();
        Int_t dchan = channel -> GetDChan();
        Int_t mmCobo = channel -> GetCobo();
        Int_t mmAsad = channel -> GetAsad();
        Int_t mmAget = channel -> GetAget();
        Int_t *mmWaveformY = channel -> GetWaveformY();

        auto type0 = fDetector -> GetType(0,mmAsad,mmAget,dchan);

        if(mmCobo==0 && !(chan==11 || chan==22 || chan==45 || chan==56))
        {
            if(type0==whichtype[0]) 
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HWF[0][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
            }
            else if(type0==whichtype[1]) 
            {
                     if(mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) HWF[1][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) HWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }

            else if(type0==TexAT2::eType::kLowCenter) 
            {
                     if((mmAsad==0 || mmAsad==1) && mmAget==0) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[0] -> Fill(buck,mmWaveformY[buck]); }
                else if((mmAsad==0 || mmAsad==1) && mmAget==1) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[1] -> Fill(buck,mmWaveformY[buck]); }
                else if((mmAsad==0 || mmAsad==1) && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[2] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==2 && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==2 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==3 && mmAget==2) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][0] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==3 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }

            else if(type0==TexAT2::eType::kHighCenter) 
            {
                     if(mmAget==3 && (mmAsad==0 || mmAsad==1)) { for(Int_t buck=0; buck<MaxBuck; buck++) CWF[3] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==2 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[0][1] -> Fill(buck,mmWaveformY[buck]); }
                else if(mmAsad==3 && mmAget==3) { for(Int_t buck=0; buck<MaxBuck; buck++) CSWF[1][1] -> Fill(buck,mmWaveformY[buck]); }
            }
        }
    }

    TCanvas* mmcvs = new TCanvas("mmcvs2","for MMS only",1600,1600);
    mmcvs -> Divide(3,4);

    mmcvs -> cd(1); HWF[0][0] -> Draw("colz");
    mmcvs -> cd(4); HWF[0][1] -> Draw("colz");
    mmcvs -> cd(7); HWF[1][0] -> Draw("colz");
    mmcvs -> cd(10); HWF[1][1] -> Draw("colz");

    mmcvs -> cd(2); CSWF[0][0] -> Draw("colz");
    mmcvs -> cd(5); CSWF[0][1] -> Draw("colz");
    mmcvs -> cd(8); CSWF[1][0] -> Draw("colz");
    mmcvs -> cd(11); CSWF[1][1] -> Draw("colz");

    mmcvs -> cd(3); CWF[0] -> Draw("colz");
    mmcvs -> cd(6); CWF[1] -> Draw("colz");
    mmcvs -> cd(9); CWF[2] -> Draw("colz");
    mmcvs -> cd(12); CWF[3] -> Draw("colz");

    gServer -> Register("draw_MMSLR",mmcvs);
}

void draw_all(Int_t eventID=10)
{
    fRun = new LKRun();
    fRun -> AddInputFile("data/texat_0824.raw.root");
    fRun -> AddDetector(new TexAT2());
    fRun -> Init();
    fRun -> GetEvent(eventID);

    fDetector = (TexAT2*) fRun -> GetDetector();
    fChannelArray = fRun -> GetBranchA("RawData");
    fEventHeaderArray = fRun -> GetBranchA("RawData");

    //draw_mm(eventID);
    //draw_entry(eventID,eventID);
    draw_entry(0,100);
    //draw_MMSLR(eventID);

    gStyle -> SetPalette(73);
}
