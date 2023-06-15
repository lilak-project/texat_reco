#include "TTHitFindingTask.h"

ClassImp(TTHitFindingTask);

TTHitFindingTask::TTHitFindingTask()
{
    ;
}

bool TTHitFindingTask::Init()
{
    // Put intialization todos here which are not iterative job though event
    lx_info << "Initializing TTHitFindingTask" << std::endl;

    fTrackArray = fRun -> GetBranchA("RawData");
    fHitArray = new TClonesArray("LKHit",200);
    fRun -> RegisterBranch("Hit", fHitArray);

    timing_dt_xy = new TH2D("timing_dt_xy","Y(vertical) vs Chain location;mmpx;dT(Beam-Chain)",134,0,134,300,-150,150);
    timing_dt_zy = new TH2D("timing_dt_zy","Y(vertical) vs Strip location;mmpy;dT(Beam-Strip)",128,0,128,300,-150,150);
    timing_dt_xz = new TH2D("timing_dt_xz","Chain location vs Strip location;mmpx;mmpy",134,0,134,64,0,128);

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

    return true;
}

void TTHitFindingTask::Exec(Option_t *option)
{
    /*
    // Call channel from fChannelArray and get data value
    int numRawData = fChannelArray -> GetEntriesFast();
    for (int iRawData = 0; iRawData < numRawData; ++iRawData)
    {
        auto *channel = (MMChannel *) fChannelArray -> At(iRawData);
        //auto value = channel -> GetDataValue(); ...
    }

    // Construct (new) hit from fHitArray and set data value
    for (int iHit = 0; iHit < numHit; ++iHit)
    {
        auto hit = (LKHit *) fHitArray -> ConstructedAt(iHit);
    }
    lx_info << "TTHitFindingTask container" << std::endl;
    */

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
            if(type[0][mmAsad[hit]][mmAget[hit]][dchan]==4) 
            {
                if(mmpy[mmAsad[hit]][mmAget[hit]][dchan]<64)
                {
                    if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==64)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[0] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==65)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[1] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==66)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[2] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==67)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[3] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==68)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[4] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                    else if(mmpx[mmAsad[hit]][mmAget[hit]][dchan]==69)
                    {
                        for(Int_t buck=Bi; buck<Bf; buck++) LCWaveFormbyPixel[5] -> Fill(buck,mmWaveformY[hit][buck]);
                    }
                }
            }
        }
        strcpy(updown, "Down"); //just for temp.
                                //if(mmCobo[hit]==1 && !(mmChan[hit]==11 || mmChan[hit]==22 || mmChan[hit]==45 || mmChan[hit]==56))
                                //{
                                //    if(type[1][mmAsad[hit]][mmAget[hit]][mmChan[hit]]==6) 
                                //    {
                                //	if(mmAsad[hit]==0 && mmAget[hit]==0) fchan = mmChan[hit];
                                //	if(mmAsad[hit]==0 && mmAget[hit]==1)
                                //	{
                                //	    if((mmChan[hit]%2)==0) strcpy(updown, "Up");
                                //	    else if((mmChan[hit]%2)==1) strcpy(updown, "Down");
                                //	}
                                //    }
                                //}
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
    cout << evt << ": " << Timing[whereisbeam]-ChainT[whereisbeam] << " at " << loc_chain << " BT" << endl;
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
        cout << evt << ": " << BeamT-ChainT_all[i] << " at " << loc_chain_all[i] << endl;
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
    cout << thetaxt << " " << radxt << endl;
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
        cout << evt << ": " << BeamT-StripT_all[i] << " at " << loc_strip_all[i] << endl;
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
    cout << thetazt << " " << radzt << endl;
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

    TCanvas* cvs_dt = new TCanvas("cvs_dt","cvs_dt",1800,1500);
    cvs_dt -> Divide(3,3);
    cvs_dt -> cd(1);
    timing_dt_xy -> Draw("colz");
    cvs_dt -> cd(2);
    timing_dt_zy -> Draw("colz");
    cvs_dt -> cd(3);
    timing_dt_xz -> Draw("colz");
    cvs_dt -> cd(4);
    //Hough_xt -> Draw("colz"); 
    fhough_xt -> Draw("colz");
    cvs_dt -> cd(5);
    //Hough_zt -> Draw("colz"); 
    fhough_zt -> Draw("colz");
    cvs_dt -> cd(6);
    fhough_xz -> Draw("colz");
    cvs_dt -> cd(7);
    Hough_xt -> Draw("colz"); 
    cvs_dt -> cd(8);
    Hough_zt -> Draw("colz"); 
    cvs_dt -> cd(9);
    fhough_xz_check -> Draw("colz"); 
    cvs_dt -> SaveAs(Form("./drawing/evt%d_track.jpg",evt));

}

bool TTHitFindingTask::EndOfRun()
{
    return true;
}
