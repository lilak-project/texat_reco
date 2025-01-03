#include "TexAT2.h"
#include "TTMicromegas.h"

ClassImp(TexAT2);

TexAT2::TexAT2()
{
    fName = "TexAT2";
    if (fDetectorPlaneArray==nullptr)
        fDetectorPlaneArray = new TObjArray();

    for (auto type=0; type<fNumPSAType; ++type)
        fChannelAnalyzer[type] = nullptr;
}

TexAT2::TexAT2(TString parName) : TexAT2()
{
    AddPar(parName);
    Init();
}

TexAT2::TexAT2(LKParameterContainer* par) : TexAT2()
{
    AddPar(par);
    Init();
}

bool TexAT2::Init()
{
    LKDetector::Init();

    // Put intialization todos here which are not iterative job though event
    e_info << "Initializing TexAT2" << std::endl;

    fPar -> UpdatePar(fX1,"TexAT2/x1");
    fPar -> UpdatePar(fX2,"TexAT2/x2");
    fPar -> UpdatePar(fY1,"TexAT2/y1");
    fPar -> UpdatePar(fY2,"TexAT2/y2");
    fPar -> UpdatePar(fZ1,"TexAT2/z1");
    fPar -> UpdatePar(fZ2,"TexAT2/z2");

    // SetDetType()
    fmapmmFileName     = fPar -> GetParString("TexAT2/mapmmFileName");
    fmapsiFileName     = fPar -> GetParString("TexAT2/mapsiFileName");
    fmapX6FileName     = fPar -> GetParString("TexAT2/mapX6FileName");
    fmapX6dimFileName  = fPar -> GetParString("TexAT2/mapX6dimFileName");
    fmapCsIFileName    = fPar -> GetParString("TexAT2/mapCsIFileName");

    Int_t mmasad[1024];
    Int_t mmaget[1024];
    Int_t mmdchan[1024];
    Int_t mmx[1024];
    Int_t mmy[1024];

    Int_t siasad;
    Int_t siaget;
    Int_t sichan;
    Int_t six;
    Int_t siy;
    Int_t sipos;

    Int_t X6asad;
    Int_t X6aget;
    Int_t X6chan;
    Int_t X6flag;
    Int_t X6detnum;
    Int_t X6pos;

    Int_t tdet;
    Int_t tstrip;
    Double_t tposx;
    Double_t tposy;
    Double_t tposz;

    Int_t CsIasad;
    Int_t CsIaget;
    Int_t CsIchan;
    Int_t CsICTnum;
    Int_t CsIpinflag;
    Int_t CsItoX6det;

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
        line++;
        if(line==fmmnum) break;
    }
    if(line<fmmnum) lk_info << Form("mmnum check: %d/%d",line-1,fmmnum) << endl;
    mapmm.close();
    mapmm.clear();

    for(Int_t i=0; i<fmmnum; i++)
    {
        fmmpx[mmasad[i]][mmaget[i]][mmdchan[i]]=mmx[i];
        fmmpy[mmasad[i]][mmaget[i]][mmdchan[i]]=mmy[i];
        if(mmx[i]==-1)
        {
            if(mmasad[i]==2)
            {
                fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLeftStrip;
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kLeft;
            }
            else
            {
                fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kRightStrip;
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kRight;
            }
        }
        else
        {
            if(mmx[i]>=0 && mmx[i]<64)
            {
                fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLeftChain;
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kLeft;
            }
            else if(mmx[i]>69)
            {
                fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kRightChain;
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kRight;
            }
            else if(mmx[i]>=65 && mmx[i]<69)
            {
                if(mmasad[i]==0 && mmaget[i]==3) fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kHighCenter;
                else fType[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eType::kLowCenter;
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kCenter;
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
                fDetLoc[0][mmasad[i]][mmaget[i]][mmdchan[i]] = eDetLoc::kCenter;
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
        mapsi >> siasad >> siaget >> sichan >> six >> siy >> sipos;
        //e_info << Form("%d: %d_%d_%d_%d_%d_%d",line,siasad,siaget,sichan,six,siy,sipos) << endl;
        fSipx[siasad][siaget][sichan] = six;
        fSipy[siasad][siaget][sichan] = siy;
        fSistrip[siasad][siaget][sichan] = sipos;
        fSidet[siasad][siaget][sichan] = 2*(4-six)+(1-siy);
        fType[1][siasad][siaget][sichan] = eType::kForwardSi;

        if(six==2)                fDetLoc[1][siasad][siaget][sichan] = eDetLoc::kCenter;
        else if(six==0 || six==1) fDetLoc[1][siasad][siaget][sichan] = eDetLoc::kLeft;
        else if(six==3 || six==4) fDetLoc[1][siasad][siaget][sichan] = eDetLoc::kRight;

        line++;
        if(line==fsinum) break;
    }
    if(line<fsinum) lk_info << Form("sinum check: %d/%d",line-1,fsinum) << endl;
    mapsi.close();
    mapsi.clear();

    // Forward CsI
    fCsIdet[1][0][2] = 0;
    fCsIdet[1][0][7] = 1;
    fCsIdet[1][0][10] = 3;
    fCsIdet[1][0][16] = 2;
    fCsIdet[1][0][19] = 4;
    fCsIdet[1][0][25] = 5;
    fCsIdet[1][0][28] = 7;
    fCsIdet[1][0][33] = 6;
    fCsIdet[1][0][36] = 8;
    fCsIdet[1][0][41] = 9;
    fType[1][1][0][2]  = eType::kForwardCsI;
    fType[1][1][0][7]  = eType::kForwardCsI;
    fType[1][1][0][10] = eType::kForwardCsI;
    fType[1][1][0][16] = eType::kForwardCsI;
    fType[1][1][0][19] = eType::kForwardCsI;
    fType[1][1][0][25] = eType::kForwardCsI;
    fType[1][1][0][28] = eType::kForwardCsI;
    fType[1][1][0][33] = eType::kForwardCsI;
    fType[1][1][0][36] = eType::kForwardCsI;
    fType[1][1][0][41] = eType::kForwardCsI;
    fDetLoc[1][1][0][2]  = eDetLoc::kRight; //0
    fDetLoc[1][1][0][7]  = eDetLoc::kRight; //1
    fDetLoc[1][1][0][10] = eDetLoc::kRight; //3
    fDetLoc[1][1][0][16] = eDetLoc::kRight; //2
    fDetLoc[1][1][0][19] = eDetLoc::kCenter; //4
    fDetLoc[1][1][0][25] = eDetLoc::kCenter; //5
    fDetLoc[1][1][0][28] = eDetLoc::kLeft; //7
    fDetLoc[1][1][0][33] = eDetLoc::kLeft; //6
    fDetLoc[1][1][0][36] = eDetLoc::kLeft; //8
    fDetLoc[1][1][0][41] = eDetLoc::kLeft; //9

    // External inputs
    fType[1][1][2][2]  = eType::kExternal; //SBD
    fType[1][1][2][7]  = eType::kExternal; //BM trigger
    fType[1][1][2][10] = eType::kExternal; //
    fType[1][1][2][16] = eType::kExternal; //
    fType[1][1][2][19] = eType::kExternal; //BM1 shaping
    fType[1][1][2][25] = eType::kExternal; //BM2 shaping
    fType[1][1][2][33] = eType::kExternal; //BM  shaping
    fType[1][1][3][2]  = eType::kExternal; //RF-PPACa
    fType[1][1][3][7]  = eType::kExternal; //PPACa-PPACb
    fType[1][1][3][10] = eType::kExternal; //
    fType[1][1][3][16] = eType::kExternal; //BM-PPACa
    fType[1][1][3][19] = eType::kExternal; //L0

    // CENS X6
    line = 0;
    ifstream mapX6;
    mapX6.open(fmapX6FileName);
    if(mapX6.fail()==true) lk_error << "error: mapchantoX6 " << fmapX6FileName << endl;
    while(mapX6.good())
    {
        mapX6 >> X6asad >> X6aget >> X6chan >> X6flag >> X6detnum >> X6pos;
        fX6det[X6asad][X6aget][X6chan] = X6detnum;
        if(X6flag==0) fX6strip[X6asad][X6aget][X6chan] = (Int_t) (X6pos+1)/2;
        else if(X6flag==1) fX6strip[X6asad][X6aget][X6chan] = X6pos;
        fX6pin[X6asad][X6aget][X6chan] = X6pos%2;
        //e_info << X6asad << " " <<  X6aget << " " <<  X6chan << " " << X6det[X6asad][X6aget][X6chan] << " " << X6strip[X6asad][X6aget][X6chan] << " " << X6pin[X6asad][X6aget][X6chan] << " " << endl;
        fType[2][X6asad][X6aget][X6chan] = eType::kCENSX6;

        if(X6asad==0 && X6aget!=3) fDetLoc[2][X6asad][X6aget][X6chan] = eDetLoc::kRight;
        else if(X6asad==1 && X6aget!=3) fDetLoc[2][X6asad][X6aget][X6chan] = eDetLoc::kLeft;
        else if(X6asad==2 && X6aget!=3) fDetLoc[2][X6asad][X6aget][X6chan] = eDetLoc::kLeft;
        else if(X6asad==3 && X6aget!=3) fDetLoc[2][X6asad][X6aget][X6chan] = eDetLoc::kRight;

        line++;
        if(line==fX6num) break;
    }
    if(line<fX6num) lk_info << Form("X6num check: %d/%d",line-1,fX6num) << endl;
    mapX6.close();
    mapX6.clear();

    // X6 dim
    line = 0;
    ifstream dimX6;
    dimX6.open(fmapX6dimFileName);
    if(dimX6.fail()==true) lk_error << "error: mapdimtoX6" << endl;
    while(dimX6.good())
    {
        dimX6 >> tdet >> tstrip >> tposx >> tposy >> tposz;
        fX6posx[tdet][tstrip] = tposx;
        fX6posy[tdet][tstrip] = tposy;
        fX6posz[tdet][tstrip] = tposz;
        line++;
        if(line==240) break;
    }
    dimX6.close();
    dimX6.clear();

    // CENS CsI
    line = 0;
    ifstream mapCsI;
    mapCsI.open(fmapCsIFileName);
    if(mapCsI.fail()==true) lk_error << "error: mapchantoCsI " << fmapCsIFileName << endl;
    while(mapCsI.good())
    {
        mapCsI >> CsIasad >> CsIaget >> CsIchan >> CsICTnum >> CsIpinflag >> CsItoX6det;
        fCsICT[CsIasad][CsIaget][CsIchan] = CsICTnum;
        fCsIpin[CsIasad][CsIaget][CsIchan] = CsIpinflag;
        fCsIX6det[CsIasad][CsIaget][CsIchan] = CsItoX6det;

        fType[2][CsIasad][CsIaget][CsIchan] = eType::kCENSCsI;
        line++;
        if(line==fCsInum) break;
    }
    if(line<fCsInum) lk_info << Form("CsInum check: %d/%d",line-1,fCsInum) << endl;
    mapCsI.close();
    mapCsI.clear();

    // Calibration Parameters
    fFSiJFileName  = fPar -> GetParString("CalibrationParameter/fFSiJ_CalPar");
    fFCsIFileName  = fPar -> GetParString("CalibrationParameter/fFCsI_CalPar");
    fX6JFileName   = fPar -> GetParString("CalibrationParameter/fX6J_CalPar");
    fX6OFileName   = fPar -> GetParString("CalibrationParameter/fX6O_CalPar");

    int iDet, iStrip;
    Double_t x6jX;
    Double_t x6jY;
    Double_t x6jK;
    Double_t x6jA;
    Double_t x6jB;
    Double_t x6oe0;
    Double_t x6oe1;

    line = 0;
    ifstream sical;
    sical.open(fFSiJFileName);
    if(sical.fail()==true) lk_error << "error: fSiJcalpar " << fFSiJFileName << endl;
    while(sical.good())
    {
        sical >> fSiJpar0[line] >> fSiJpar1[line];
        if(fSiJpar0[line]!=0)
        {
            fSiJdet0[fSidet[0][0][line]][fSistrip[0][0][line]-1] = fSiJpar0[line];
            fSiJdet1[fSidet[0][0][line]][fSistrip[0][0][line]-1] = fSiJpar1[line];
        }
        line++;
        if(line==42) break;
    }
    sical.close();
    sical.clear();

    line = 0;
    ifstream csical;
    csical.open(fFCsIFileName);
    if(csical.fail()==true) lk_error << "error: fCsIcalpar " << fFCsIFileName << endl;
    while(csical.good())
    {
        csical >> fCsIpar0[line] >> fCsIpar1[line];
        line++;
        if(line==10) break;
    }
    csical.close();
    csical.clear();

    line = 0;
    ifstream x6jcal;
    x6jcal.open(fX6JFileName);
    if(x6jcal.fail()==true) lk_error << "error: fX6Jcalpar " << fX6JFileName << endl;
    while(x6jcal.good())
    {
        x6jcal >> iDet >> iStrip >> x6jY >> x6jX >> x6jK >> x6jB >> x6jA;
        fX6JparX[(int)line/8][line%8] = x6jX;
        fX6JparY[(int)line/8][line%8] = x6jY;
        fX6JparK[(int)line/8][line%8] = x6jK;
        fX6JparA[(int)line/8][line%8] = x6jA;
        fX6JparB[(int)line/8][line%8] = x6jB;

        line++;
        if(line==240) break;
    }
    x6jcal.close();
    x6jcal.clear();

    line = 0;
    ifstream x6ocal;
    x6ocal.open(fX6OFileName);
    if(x6ocal.fail()==true) lk_error << "error: fX6Ocalpar " << fX6OFileName << endl;
    while(x6ocal.good())
    {
        x6ocal >> x6oe0 >> x6oe1;
        fX6OEpar0[(int)line/4][line%4] = x6oe0;
        fX6OEpar1[(int)line/4][line%4] = x6oe1;

        line++;
        if(line==120) break;
    }
    x6ocal.close();
    x6ocal.clear();

    InitChannelAnalyzer();

    return true;
}

bool TexAT2::InitChannelAnalyzer()
{
    if (fChannelAnalyzer[0]==nullptr)
        for (auto type=0; type<fNumPSAType; ++type)
        {
            fChannelAnalyzer[type] = new LKChannelAnalyzer();
            TString parName1 = Form("TexAT2/pulseFile/%s",fTypeNames[type].Data());
            TString parName2 = Form("TexAT2/analysis/%s",fTypeNames[type].Data());
            TString parName3 = Form("TexAT2/inverted/%s",fTypeNames[type].Data());
            TString pulseFileName = fPar -> GetParString(parName1);
            int dynamicRange = 4096;
            int threshold = 50;
            int tbStart = 1;
            int tbMax = 350;
            int iterMax = 15;
            double tbStepCut = 0.01;
            int tbStartCut = 330;
            double scaleTbStep = 0.2;
            int thresholdOneStep = 2;
            int numTbAcendingCut = 5;
            bool dataIsInverted = false;
            fPar -> UpdatePar(dynamicRange,parName2,0);
            fPar -> UpdatePar(threshold,parName2,1);
            fPar -> UpdatePar(tbStart,parName2,2);
            fPar -> UpdatePar(tbMax,parName2,3);
            fPar -> UpdatePar(iterMax,parName2,4);
            fPar -> UpdatePar(tbStepCut,parName2,5);
            fPar -> UpdatePar(tbStartCut,parName2,6);
            fPar -> UpdatePar(scaleTbStep,parName2,7);
            fPar -> UpdatePar(thresholdOneStep,parName2,8);
            fPar -> UpdatePar(numTbAcendingCut,parName2,9);
            fPar -> UpdatePar(dataIsInverted,parName3);
            fChannelAnalyzer[type] -> SetPulse(pulseFileName);
            fChannelAnalyzer[type] -> SetDynamicRange(dynamicRange);
            fChannelAnalyzer[type] -> SetThreshold(threshold);
            fChannelAnalyzer[type] -> SetTbStart(tbStart);
            fChannelAnalyzer[type] -> SetTbMax(tbMax);
            fChannelAnalyzer[type] -> SetIterMax(iterMax);
            fChannelAnalyzer[type] -> SetTbStepCut(tbStepCut);
            fChannelAnalyzer[type] -> SetTbStartCut(tbStartCut);
            fChannelAnalyzer[type] -> SetScaleTbStep(scaleTbStep);
            fChannelAnalyzer[type] -> SetThresholdOneStep(thresholdOneStep);
            fChannelAnalyzer[type] -> SetNumTbAcendingCut(numTbAcendingCut);
            fChannelAnalyzer[type] -> SetDataIsInverted(dataIsInverted);
        }

    return true;
}

void TexAT2::Print(Option_t *option) const
{
    // You will probability need to modify here
    e_info << "TexAT2" << std::endl;
}

bool TexAT2::BuildGeometry()
{
    return true;
}

bool TexAT2::BuildDetectorPlane()
{
    // example plane
    // AddPlane(new MyPlane);
    auto mm = new TTMicromegas;
    AddPlane(mm);
    return true;
}

bool TexAT2::IsInBoundary(Double_t x, Double_t y, Double_t z)
{
    // example (x,y,z) is inside the plane boundary
    //if (x>-10 and x<10)
    //    return true;
    //return false;
    return true;
}

bool TexAT2::GetEffectiveDimension(Double_t &x1, Double_t &y1, Double_t &z1, Double_t &x2, Double_t &y2, Double_t &z2)
{
    x1 = -175;
    x2 = +175;
    y1 = -100;
    y2 = 100;
    z1 = 150;
    z2 = 500;
    return true;
}

int TexAT2::GetElectronicsID(int caac) {
    auto caac0 = caac;
    auto cobo = int(caac0/10000); caac0 -= cobo*10000;
    auto asad = int(caac0/1000); caac0 -= asad*1000;
    auto aget = int(caac0/100); caac0 -= aget*100;
    auto chan = caac0;
    return GetElectronicsID(cobo, asad, aget, chan);
}

int TexAT2::GetElectronicsID(int cobo, int asad, int aget, int chan)
{
    if(chan==11 || chan==22 || chan==45 || chan==56) return -1;

    if (cobo==0 && (asad==0 || asad==1)) {
        int mchannel = chan;
             if(chan<11) mchannel = chan;
        else if(chan<22) mchannel = chan - 1;
        else if(chan<45) mchannel = chan - 2;
        else if(chan<56) mchannel = chan - 3;
        else             mchannel = chan - 4;

        if ((int(mchannel+1)/2)%2==0)
        {
                 if (aget==0) return eMMCenterSideA0;
            else if (aget==1) return eMMCenterSideA1;
            else if (aget==2) return eMMCenterSideA2;
            else if (aget==3) return eMMCenterSideA3;
        }
        else
        {
                 if (aget==0) return eMMCenterCenterA0;
            else if (aget==1) return eMMCenterCenterA1;
            else if (aget==2) return eMMCenterCenterA2;
            else if (aget==3) return eMMCenterCenterA3;
        }
    }
    if (cobo==0 && asad==2 && (aget==0 || aget==1)) return eMMLeftSide;
    if (cobo==0 && asad==2 && (aget==2 || aget==3)) return eMMLeftCenter;
    if (cobo==0 && asad==3 && (aget==0 || aget==1)) return eMMRightSide;
    if (cobo==0 && asad==3 && (aget==2 || aget==3)) return eMMRightCenter;
    if (cobo==1 && asad==0 && aget==0) return efSiJunction;
    if (cobo==1 && asad==0 && aget==1) return efSiOhmic;
    if (cobo==1 && asad==1 && aget==0
        && (chan==2
         || chan==7
         || chan==10
         || chan==16
         || chan==19
         || chan==25
         || chan==28
         || chan==33
         || chan==36
         || chan==41)
       )
        return efCsI;
    if (cobo==2 && aget==0) return eX6Ohmic;
    if (cobo==2 && (aget==1 || aget==2)) return eX6Junction;
    if (cobo==2 && asad==1 && aget==3) return eCsICT;

    return -1;
}

void TexAT2::CAACToGlobalPosition(Int_t caac,
        Double_t &posx, Double_t &posy, Double_t &posz,
        Double_t &errx, Double_t &erry, Double_t &errz)
{
    auto caac0 = caac;
    auto cobo = int(caac0/10000); caac0 -= cobo*10000;
    auto asad = int(caac0/1000); caac0 -= asad*1000;
    auto aget = int(caac0/100); caac0 -= aget*100;
    auto chan = caac0;
    CAACToGlobalPosition(cobo,asad,aget,chan,posx,posy,posz,errx,erry,errz);
}

void TexAT2::CAACToGlobalPosition(Int_t Cobo, Int_t Asad, Int_t Aget, Int_t Chan,
        Double_t &posx, Double_t &posy, Double_t &posz,
        Double_t &errx, Double_t &erry, Double_t &errz)
{
    //[mm]
    posx = 0;
    posy = 0;
    posz = 0;
    errx = 0;
    erry = 0;
    errz = 0;

    if(Chan==11 || Chan==22 || Chan==45 || Chan==56) return;
    if(Cobo==0)
    {
        if(Chan<11) Chan = Chan;
        else if(Chan>11 && Chan<22) Chan = Chan -1;
        else if(Chan>22 && Chan<45) Chan = Chan -2;
        else if(Chan>45 && Chan<56) Chan = Chan -3;
        else if(Chan>56) Chan = Chan -4;
    }
    auto fDetector = ((TexAT2*) LKRun::GetRun() -> GetDetector());
    auto type = fDetector -> GetType(Cobo,Asad,Aget,Chan);

    Double_t MMAllX = 240;
    Double_t MMAllZ = 224;
    Double_t minMMZ = 191.7;
    Double_t MMBeamAllLong = 3.43;
    Double_t MMBeamAllShort = 1.75;
    Double_t MMBeamActLong = 3.42;
    Double_t MMBeamActShort = 1.67;
    Double_t MMChainAllShort = 1.714;
    Double_t MMStripX = (MMAllX - MMBeamAllLong*6)/2;
    Double_t MMChainZ = MMAllZ;

    Double_t maxForwardSiZ = 468.29;
    //Double_t maxForwardSiZ = 476.9;

    Double_t X6AllLong = 93.1;
    Double_t X6AllShort = 45.2;
    Double_t X6ActLong = 75;
    Double_t X6ActShort = 40.3;

    Double_t maxSideX6X = fDetector->GetX6posx(21, 0);
    Double_t maxBottomX6X = fDetector->GetX6posx(201, 0) + X6ActLong / 2;
    Double_t minBottomX6Y = fDetector->GetX6posy(201, 0);
    Double_t minX6Z = fDetector->GetX6posz(208, 0);

    if(type==TexAT2::eType::kLeftStrip ||
       type==TexAT2::eType::kRightStrip ||
       type==TexAT2::eType::kLeftChain ||
       type==TexAT2::eType::kRightChain ||
       type==TexAT2::eType::kLowCenter ||
       type==TexAT2::eType::kHighCenter) //MM
    {
        Int_t mmpx = fDetector -> Getmmpx(Asad,Aget,Chan);
        Int_t mmpy = fDetector -> Getmmpy(Asad,Aget,Chan);
        if(type==TexAT2::eType::kLowCenter || type==TexAT2::eType::kHighCenter) //beam
        {
            posx = (mmpx-66.5)*MMBeamAllLong;
            posz = minMMZ + (mmpy+0.5)*MMBeamAllShort;
            errx = MMBeamActLong/2;
            errz = MMBeamActShort/2;
        }
        else //side
        {
            if(mmpx<0) //strip
            {
                if(Asad==2) posx = -(MMStripX/2+MMBeamAllLong*3);
                else if(Asad==3) posx = (MMStripX/2+MMBeamAllLong*3);
                posz = minMMZ + (mmpy+0.5)*MMBeamAllShort;
                errx = MMStripX/2;
                errz = MMBeamActShort/2;
            }
            else if(mmpy<0) //chain
            {
                if(Asad==2) posx = -(MMBeamAllLong*3+(63.5-mmpx)*MMChainAllShort);
                if(Asad==3) posx = MMBeamAllLong*3+(mmpx-69.5)*MMChainAllShort;
                posz = minMMZ + MMChainZ/2;
                errx = MMBeamActShort/2;
                errz = MMChainZ/2;
            }
        }
        posy = 75/2; //should be checked
        erry = 0; //Detector on xz-plane
    }
    else if(type==TexAT2::eType::kForwardSi) //Forward Si
    {
        Int_t sipx = fDetector -> GetSipx(Asad,Aget,Chan);
        Int_t sipy = fDetector -> GetSipy(Asad,Aget,Chan);
        Int_t sistrip = fDetector -> GetSistrip(Asad,Aget,Chan);
        if(Aget==0) //Junction
        {
            if(sipx==2)
            {
                posx = 0;
                if(sipy==0) posy = -12.5;
                else if(sipy==1) posy = 51.8;
            }
            else
            {
                if(sipx==0) { posx = -124; posy = 35.8; }
                else if(sipx==1) { posx = -62.2; posy = 30.8; }
                else if(sipx==3) { posx = 62.2; posy = 30.8; }
                else if(sipx==4) { posx = 124; posy = 35.8; }
                if(sipy==0) posy *= -1;
            }
            //should be checked below here
            if(sistrip==1) { posx -= 12.5; posy += 12.5; }
            else if(sistrip==2) { posx += 12.5; posy += 12.5; }
            else if(sistrip==3) { posx -= 12.5; posy -= 12.5; }
            else if(sistrip==4) { posx += 12.5; posy -= 12.5; }
            posz = maxForwardSiZ;
            errx = 12.5;
            erry = 12.5;
            errz = 0; //Detector on xy-plane
        }
        else if(Aget==1) //Ohmic
        {
            if(sipx==2)
            {
                posx = 0;
                if(sipy==0) posy = -12.5;
                else if(sipy==1) posy = 51.8;
            }
            else
            {
                if(sipx==0) { posx = -124; posy = 35.8; }
                else if(sipx==1) { posx = -62.2; posy = 30.8; }
                else if(sipx==3) { posx = 62.2; posy = 30.8; }
                else if(sipx==4) { posx = 124; posy = 35.8; }
                if(sipy==0) posy *= -1;
            }
            posz = maxForwardSiZ;
            errx = 25;
            erry = 25;
            errz = 0; //Detector on xy-plane
        }
    }
    else if(type==TexAT2::eType::kCENSX6) //X6
    {
        Int_t det = fDetector -> GetX6det(Asad,Aget,Chan);
        Int_t strip = fDetector -> GetX6strip(Asad,Aget,Chan);
        if(Aget==1 || Aget==2) //Junction
        {
            posx = fDetector -> GetX6posx(det,strip-1);
            posy = fDetector -> GetX6posy(det,strip-1);
            posz = fDetector -> GetX6posz(det,strip-1);
            if(det<100) //Side
            {
                if(det<19) posz += 2.5;
                else if(det<29) posz -= 2.5;
                errx = 0; //Detector on yz-plane
                erry = X6ActLong/2;
                //erry = 1;
                errz = 2.5;
            }
            else //Bottom
            {
                if(det==101 || det==204)
                {
                    posx += 2.5;
                    errx = 2.5;
                    erry = 0; //Detector on xz-plane
                    errz = X6ActLong/2;
                    //errz = 1;
                }
                else
                {
                    if(det<109) posz -= 2.5;
                    else if(det<209) posz += 2.5;
                    errx = X6ActLong/2;
                    //errx = 1;
                    erry = 0; //Detector on xz-plane
                    errz = 2.5;
                }
            }
        }
        else if(Aget==0) //Ohmic
        {
            if(det<100)
            {
                Int_t map;
                if(det<15) map = det-8;
                else if(det<19) map = det-15;
                else map = 27-det;

                if(det<19) posx = -maxSideX6X;
                else if(det<29) posx = maxSideX6X;
                posy = (strip-2.5)*X6ActLong/4;
                posz = minX6Z + map*X6AllShort + X6ActShort/2;
                errx = 0; //Detector on yz-plane
                erry = X6ActLong/8;
                errz = X6ActShort/2;
            }
            else
            {
                if(det==101 || det==204)
                {
                    if(det==101) posx = -X6AllShort/2;
                    else posx = X6AllShort/2;
                    posy = minBottomX6Y;
                    posz = 422 + (strip-2.5)*X6ActLong/4;
                    errx = X6ActShort/2;
                    erry = 0; //Detector on xz-plane
                    errz = X6ActLong/8;
                }
                else
                {
                    Int_t map;
                    if(det<105) map = det-98;
                    else if(det<109) map = det-105;
                    else if(det<204) map = 207-det;
                    else map = 208-det;

                    if(det<109) posx = - (maxBottomX6X - X6ActLong/2 - (strip-2.5)*X6ActLong/4);
                    else posx = maxBottomX6X - X6ActLong/2 - (strip-2.5)*X6ActLong/4;
                    posy = minBottomX6Y;
                    posz = minX6Z + map*X6AllShort + X6ActShort/2;
                    errx = X6ActLong/8;
                    erry = 0; //Detector on xz-plane
                    errz = X6ActShort/2;
                }
            }
        }
    }
    else //Forward CsI, CENS CsI
    {
        posx = 0;
        posy = 0;
        posz = 0;
        errx = 0;
        erry = 0;
        errz = 0;
    }
}
