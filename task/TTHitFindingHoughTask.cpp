#include "TTHitFindingHoughTask.h"
#include "MMChannel.h"
#include "LKHit.h"

ClassImp(TTHitFindingHoughTask);

TTHitFindingHoughTask::TTHitFindingHoughTask()
{
    fName = "TTHitFindingHoughTask";
}

bool TTHitFindingHoughTask::Init()
{
    lx_info << "Initializing TTHitFindingHoughTask" << std::endl;

    fDetector = (TexAT2 *) fRun -> GetDetector();

    fEventHeader = (TTEventHeader *) fRun -> GetBranch("EventHeader");
    fChannelArray = fRun -> GetBranchA("RawData");
    fHitArray = new TClonesArray("LKHit",200);
    fRun -> RegisterBranch("HoughHit",fHitArray);

    timing_dt_xy = new TH2D("timing_dt_xy","Y(vertical) vs Chain location;mmpx;dT(Beam-Chain)",134,0,134,300,-150,150);
    timing_dt_zy = new TH2D("timing_dt_zy","Y(vertical) vs Strip location;mmpy;dT(Beam-Strip)",128,0,128,300,-150,150);
    timing_dt_xz = new TH2D("timing_dt_xz","Chain location vs Strip location;mmpx;mmpy",134,0,134,64,0,128);

    for(Int_t i=0; i<6; i++)
        LCWaveFormbyPixel[i] = new TH1D(Form("LCenter%d",i),"",2*Beam_er,Bi,Bf);

    for(Int_t i=0; i<64; i++)
        HWaveFormbyPixel[i] = new TH1D(Form("HWaveFormbyPixel%d",i),"",512,0,512);

    HWaveFormbyPixel_temp = new TH1D("HWaveFormbyPixel_temp","",512,0,512);
    Hough_xt = new TH2D("Hough_xt","Hough_xt;theta;radius",1800,0,180,268,-134,134);
    Hough_zt = new TH2D("Hough_zt","Hough_zt;theta;radius",1800,0,180,256,-128,128);

    return true;
}

void TTHitFindingHoughTask::Exec(Option_t *option)
{
    if (fEventHeader->GetIsGoodEvent()==false)
        return;

    timing_dt_xy -> Reset("ICES");
    timing_dt_zy -> Reset("ICES");
    timing_dt_xz -> Reset("ICES");
    for(Int_t i=0; i<512; i++) track_px[i] = 9999;
    for(Int_t i=0; i<512; i++) track_py[i] = 9999;
    for(Int_t i=0; i<6; i++) {
        LCWaveFormbyPixel[i] -> Reset("ICES");
    }
    for(Int_t i=0; i<64; i++) {
        HWaveFormbyPixel[i] -> Reset("ICES");
    }

    auto SiBLR = fEventHeader -> GetSiBLR();

    //char side[16];
    Int_t whichtype[2];
    if(SiBLR==0)
    {
        whichtype[0] = 0;
        whichtype[1] = 2;
    }
    else if(SiBLR==1)
    {
        whichtype[0] = 1;
        whichtype[1] = 3;
    }

    char updown[10];
    Int_t Timing[6];

    Int_t mmMult = fChannelArray -> GetEntries();

    for(Int_t iChannel=0; iChannel<mmMult; iChannel++)
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
            auto type = fDetector -> GetType(0,mmAsad,mmAget,dchan);
            auto mmpx = fDetector -> Getmmpx(mmAsad,mmAget,dchan);
            auto mmpy = fDetector -> Getmmpy(mmAsad,mmAget,dchan);
            if(type==TexAT2::eType::kLowCenter)
            {
                if(mmpy<64)
                {
                    if(mmpx==64)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[0] -> Fill(buck,mmWaveformY[buck]);
                    }
                    else if(mmpx==65)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[1] -> Fill(buck,mmWaveformY[buck]);
                    }
                    else if(mmpx==66)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[2] -> Fill(buck,mmWaveformY[buck]);
                    }
                    else if(mmpx==67)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[3] -> Fill(buck,mmWaveformY[buck]);
                    }
                    else if(mmpx==68)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[4] -> Fill(buck,mmWaveformY[buck]);
                    }
                    else if(mmpx==69)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[5] -> Fill(buck,mmWaveformY[buck]);
                    }
                }
            }
        }
        strcpy(updown, "Down");
    }

    // ================================================== Beam candidates timing
    for(Int_t i=0; i<6; i++)
    {
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
    if(whichtype[1]==2) // XXX why not just use SiBLR ? and what is the use of whichtype[0]?
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
        if(num_chain==6 || whereisx==0 || whereisx==133)
            break; // XXX how can num_chain be ???
        else
        {
            for(Int_t iChannel=0; iChannel<mmMult; iChannel++)
            {
                auto channel = (MMChannel *) fChannelArray -> At(iChannel);
                Int_t chan = channel -> GetChan();
                Int_t dchan = channel -> GetDChan();
                Int_t mmCobo = channel -> GetCobo();
                Int_t mmAsad = channel -> GetAsad();
                Int_t mmAget = channel -> GetAget();
                Int_t *mmWaveformY = channel -> GetWaveformY();

                auto mmpx = fDetector -> Getmmpx(mmAsad,mmAget,dchan);
                if(mmpx==whereisx && !(chan==11 || chan==22 || chan==45 || chan==56))
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_chain] -> Fill(buck,mmWaveformY[buck]);
                    if(num_chain==0) loc_chain = mmpx;
                    num_chain++;
                    continue;
                }
            }
            whereisx = whereisx + wheretox;
            continue;
        }
    }
    if(num_chain==0) return;

    HWaveFormbyPixel_temp -> Reset("ICES");
    for(Int_t j=0; j<num_chain; j++) //chain
    {
        HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone();
        for(Int_t i=0; i<6; i++) //timing
        {
                 if(strcmp(updown,"Up"  )==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(Timing[i]-BwT,Timing[i]);
            else if(strcmp(updown,"Down")==0) HWaveFormbyPixel_temp -> GetXaxis() -> SetRange(Timing[i],Timing[i]+BwT);

            ChainT[i] = ChainT[i] + HWaveFormbyPixel_temp -> GetMaximumBin();
            if(Timing[i]==0) ChainT[i] = 0;
        }
        HWaveFormbyPixel_temp->Reset();
    }
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
        if(whereisx==0 || whereisx==133) break; // XXX this make error?
        else
        {
            for(Int_t iChannel=0; iChannel<mmMult; iChannel++)
            {
                auto channel = (MMChannel *) fChannelArray -> At(iChannel);
                Int_t chan = channel -> GetChan();
                Int_t dchan = channel -> GetDChan();
                Int_t mmCobo = channel -> GetCobo();
                Int_t mmAsad = channel -> GetAsad();
                Int_t mmAget = channel -> GetAget();
                Int_t *mmWaveformY = channel -> GetWaveformY();

                auto mmpx = fDetector -> Getmmpx(mmAsad,mmAget,dchan);
                if(mmpx==whereisx && !(chan==11 || chan==22 || chan==45 || chan==56))
                {
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_chain] -> Fill(buck,mmWaveformY[buck]);
                    loc_chain_all[num_chain] = mmpx;
                    num_chain++;
                    continue;
                }
            }
            whereisx = whereisx + wheretox;
            continue;
        }
    }
    if(num_chain==0) return;

    Int_t evt = fRun -> GetCurrentEventID();
    for(Int_t j=0; j<num_chain; j++) //chain
    {
        //HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone(Form("H_temp_%d_%d",j,evt));
             if(strcmp(updown,"Up"  )==0) HWaveFormbyPixel[j] -> GetXaxis() -> SetRange(BeamT-Bw,BeamT);
        else if(strcmp(updown,"Down")==0) HWaveFormbyPixel[j] -> GetXaxis() -> SetRange(BeamT,BeamT+Bw);

        ChainT_all[j] = HWaveFormbyPixel[j] -> GetMaximumBin();
        if(BeamT==0)
        {
            ChainT_all[j] = 0;
        }
        //HWaveFormbyPixel_temp->Reset();
    }
    for(Int_t i=0; i<num_chain; i++)
    {
        timing_dt_xy -> Fill(loc_chain_all[i],(BeamT-ChainT_all[i]));
        track_px[BeamT-ChainT_all[i]+256] = loc_chain_all[i];
    }
    for(Int_t i=0; i<64; i++) HWaveFormbyPixel[i] -> Reset();

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
    Hough_xt -> Reset("ICES");
    for(Int_t i=0; i<num_chain; i++)
    {
        if(ChainT_all[i]!=9999)
        {
            for(Double_t theta=0; theta<180; theta+=0.1)
            {
                Double_t costheta = TMath::Cos(theta*TMath::DegToRad());
                Double_t sintheta = TMath::Sin(theta*TMath::DegToRad());
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
            maxbinxt = Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximum();
            thetaxt =  Hough_xt -> GetXaxis() -> GetBinCenter(i);
            radxt =  Hough_xt -> GetYaxis() -> GetBinCenter(Hough_xt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin());
        }
    }

    Double_t tanxt = TMath::Tan(thetaxt*TMath::DegToRad());
    Double_t sinxt = TMath::Sin(thetaxt*TMath::DegToRad());
    Double_t cosxt = TMath::Cos(thetaxt*TMath::DegToRad());

    Int_t StripT_all[128]; // [strip]
    for(Int_t j=0; j<128; j++) StripT_all[j] = 0;
    Int_t loc_strip_all[128];
    Int_t num_strip = 0;
    Int_t whereisz = 0;
    Int_t wheretoz = 2;

    while(1)
    {
        if(whereisz==128) break; //should be changed depend on L/R since R is soooo noisy
        else
        {
            for(Int_t iChannel=0; iChannel<mmMult; iChannel++)
            {
                auto channel = (MMChannel *) fChannelArray -> At(iChannel);
                Int_t chan = channel -> GetChan();
                Int_t dchan = channel -> GetDChan();
                Int_t mmCobo = channel -> GetCobo();
                Int_t mmAsad = channel -> GetAsad();
                Int_t mmAget = channel -> GetAget();
                Int_t *mmWaveformY = channel -> GetWaveformY();

                auto mmpx = fDetector -> Getmmpx(mmAsad,mmAget,dchan);
                auto mmpy = fDetector -> Getmmpy(mmAsad,mmAget,dchan);
                if(mmpx==-1 && mmpy==whereisz
                        && mmAsad==whichtype[1] && !(chan==11 || chan==22 || chan==45 || chan==56))
                {
                    HWaveFormbyPixel[num_strip] -> Reset();
                    for(Int_t buck=0; buck<MaxBuck; buck++) HWaveFormbyPixel[num_strip] -> Fill(buck,mmWaveformY[buck]);
                    loc_strip_all[num_strip] = mmpy;
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
        //HWaveFormbyPixel_temp = (TH1D*) HWaveFormbyPixel[j] -> Clone();
             if(strcmp(updown,"Up"  )==0) HWaveFormbyPixel[j] -> GetXaxis() -> SetRange(BeamT-Bw,BeamT);
        else if(strcmp(updown,"Down")==0) HWaveFormbyPixel[j] -> GetXaxis() -> SetRange(BeamT,BeamT+Bw);

        StripT_all[j] = HWaveFormbyPixel[j] -> GetMaximumBin();
        if(BeamT==0) StripT_all[j] = 0;
        //HWaveFormbyPixel_temp->Reset();
    }
    for(Int_t i=0; i<num_strip; i++)
    {
        timing_dt_zy -> Fill(loc_strip_all[i],(BeamT-StripT_all[i]));
        track_py[BeamT-StripT_all[i]+256] = loc_strip_all[i];
    }

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
    Hough_zt -> Reset("ICES");
    for(Int_t i=0; i<num_strip; i++)
    {
        if(StripT_all[i]!=9999)
        {
            for(Double_t theta=0; theta<180; theta+=0.1)
            {
                Double_t costheta = TMath::Cos(theta*TMath::DegToRad());
                Double_t sintheta = TMath::Sin(theta*TMath::DegToRad());
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
            maxbinzt = Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximum();
            thetazt =  Hough_zt -> GetXaxis() -> GetBinCenter(i);
            radzt =  Hough_zt -> GetYaxis() -> GetBinCenter(Hough_zt -> ProjectionY("pjy",i,i+1) -> GetMaximumBin());
        }
    }

    Double_t tanzt = TMath::Tan(thetazt*TMath::DegToRad());
    Double_t sinzt = TMath::Sin(thetazt*TMath::DegToRad());
    Double_t coszt = TMath::Cos(thetazt*TMath::DegToRad());

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

    int countHits = 0;
    for(Double_t i=-150; i<150; i+=0.1) {
        if(  (-i*tanxt+radxt*(cosxt+sinxt*tanxt)>0 && -i*tanxt+radxt*(cosxt+sinxt*tanxt)<134)
           &&(-i*tanzt+radzt*(coszt+sinzt*tanzt)>0 && -i*tanzt+radzt*(coszt+sinzt*tanzt)<128))
        {
            auto hit = (LKHit *) fHitArray -> ConstructedAt(countHits++);
            //lk_debug << -i*tanxt+radxt*(cosxt+sinxt*tanxt) << " " << i << " " << -i*tanzt+radzt*(coszt+sinzt*tanzt) << endl;
            hit -> SetPosition(-i*tanxt+radxt*(cosxt+sinxt*tanxt),i,-i*tanzt+radzt*(coszt+sinzt*tanzt));
        }
    }

    lx_info << "TTHitFindingHoughTask" << std::endl;
}

bool TTHitFindingHoughTask::EndOfRun()
{
    return true;
}
