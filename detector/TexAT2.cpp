#include "TexAT2.h"

ClassImp(TexAT2);

TexAT2::TexAT2()
{
    fName = "TexAT2";
    if (fDetectorPlaneArray==nullptr)
        fDetectorPlaneArray = new TObjArray();
}

bool TexAT2::Init()
{
    // Put intialization todos here which are not iterative job though event
    e_info << "Initializing TexAT2" << std::endl;

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

    Double_t x6je0;
    Double_t x6je1;
    Double_t x6jp0;
    Double_t x6jp1;
    Double_t x6oe0;
    Double_t x6oe1;

    line = 0;
    ifstream sical;
    sical.open(fFSiJFileName);
    if(sical.fail()==true) lk_error << "error: fSiJcalpar " << fFSiJFileName << endl;
    while(sical.good())
    {
        sical >> fSiJpar0[line] >> fSiJpar1[line];
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
        x6jcal >> x6je0 >> x6je1 >> x6jp0 >> x6jp1;
        fX6JEpar0[(int)line/8][line%8] = x6je0;
        fX6JEpar1[(int)line/8][line%8] = x6je1;
        fX6JPpar0[(int)line/8][line%8] = x6jp0;
        fX6JPpar1[(int)line/8][line%8] = x6jp1;

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
