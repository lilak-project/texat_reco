#ifndef TEXAT2_HH
#define TEXAT2_HH

#include "LKDetector.h"
#include "LKLogger.h"
#include "LKChannelAnalyzer.h"
#include "MMChannel.h"

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example LILAK detector class
 *
 */

class TexAT2 : public LKDetector
{
    public:
        TexAT2();
        virtual ~TexAT2() { ; }

        TexAT2(TString parName); ///< Init with parName
        TexAT2(LKParameterContainer* par); ///< Init with par

        enum class eType
        {
            kNon,           
            kLeftStrip,     // 0
            kRightStrip,    // 1
            kLeftChain,     // 2
            kRightChain,    // 3
            kLowCenter,     // 4
            kHighCenter,    // 5
            kForwardSi,     // 6
            kForwardCsI,    // 7
            kCENSX6,        // 10
            kCENSCsI,       // 11
            kExternal,      // 100
        };

        enum class eDetLoc
        {
            kNon,
            kLeft,          // 0
            kRight,         // 1
            kCenter,   // 2
        };

        void Print(Option_t *option="") const;
        bool Init();
        bool InitChannelAnalyzer();
        bool BuildGeometry();
        bool BuildDetectorPlane();
        bool IsInBoundary(Double_t x, Double_t y, Double_t z);
        bool GetEffectiveDimension(Double_t &x1, Double_t &y1, Double_t &z1, Double_t &x2, Double_t &y2, Double_t &z2);

        int GetTypeNumber(eType type)
        {
            if (type==eType::kLeftStrip) return 0;
            if (type==eType::kRightStrip) return 1;
            if (type==eType::kLeftChain) return 2;
            if (type==eType::kRightChain) return 3;
            if (type==eType::kLowCenter) return 4;
            if (type==eType::kHighCenter) return 5;
            if (type==eType::kForwardSi) return 6;
            if (type==eType::kForwardCsI) return 7;
            if (type==eType::kCENSX6) return 10;
            if (type==eType::kCENSCsI) return 11;
            if (type==eType::kExternal) return 100;
            return -1;
        }

        int GetTypeNumber(int cobo, int asad, int aget, int chan) {
            auto type = fType[cobo][asad][aget][chan];
            return GetTypeNumber(type);
        }

        eType   GetType  (int cobo, int asad, int aget, int chan) { return fType  [cobo][asad][aget][chan]; }
        eDetLoc GetDetLoc(int cobo, int asad, int aget, int chan) { return fDetLoc[cobo][asad][aget][chan]; }
        Int_t   Getmmpx           (int asad, int aget, int dchan) { return fmmpx       [asad][aget][dchan]; }
        Int_t   Getmmpy           (int asad, int aget, int dchan) { return fmmpy       [asad][aget][dchan]; }
        Int_t   GetSipx            (int asad, int aget, int chan) { return fSipx        [asad][aget][chan]; }
        Int_t   GetSipy            (int asad, int aget, int chan) { return fSipy        [asad][aget][chan]; }
        Int_t   GetSistrip         (int asad, int aget, int chan) { return fSistrip     [asad][aget][chan]; }
        Int_t   GetSidet           (int asad, int aget, int chan) { return fSidet       [asad][aget][chan]; }
        Int_t   GetCsIdet          (int asad, int aget, int chan) { return fCsIdet      [asad][aget][chan]; }
        Int_t   GetX6det           (int asad, int aget, int chan) { return fX6det       [asad][aget][chan]; }
        Int_t   GetX6strip         (int asad, int aget, int chan) { return fX6strip     [asad][aget][chan]; }
        Int_t   GetX6pin           (int asad, int aget, int chan) { return fX6pin       [asad][aget][chan]; }
        Int_t   GetCsICT           (int asad, int aget, int chan) { return fCsICT       [asad][aget][chan]; }
        Int_t   GetCsIpin          (int asad, int aget, int chan) { return fCsIpin      [asad][aget][chan]; }
        Int_t   GetCsIX6det        (int asad, int aget, int chan) { return fCsIX6det    [asad][aget][chan]; }

        Double_t GetX6posx               (int x6det, int x6strip) { return fX6posx        [x6det][x6strip]; }
        Double_t GetX6posy               (int x6det, int x6strip) { return fX6posy        [x6det][x6strip]; } 
        Double_t GetX6posz               (int x6det, int x6strip) { return fX6posz        [x6det][x6strip]; }

        Double_t GetSiJpar0(int chan)            { return fSiJpar0[chan]; }
        Double_t GetSiJpar1(int chan)            { return fSiJpar1[chan]; }
        Double_t GetCsIpar0(int chan)            { return fCsIpar0[chan]; }
        Double_t GetCsIpar1(int chan)            { return fCsIpar1[chan]; }

        Double_t GetX6JEpar0(int det, int strip) { return fX6JEpar0[det][strip]; }
        Double_t GetX6JEpar1(int det, int strip) { return fX6JEpar1[det][strip]; }
        Double_t GetX6JPpar0(int det, int strip) { return fX6JPpar0[det][strip]; }
        Double_t GetX6JPpar1(int det, int strip) { return fX6JPpar1[det][strip]; }
        Double_t GetX6OEpar0(int det, int strip) { return fX6OEpar0[det][strip]; }
        Double_t GetX6OEpar1(int det, int strip) { return fX6OEpar1[det][strip]; }

        int GetElectronicsID(int caac);
        int GetElectronicsID(int cobo, int asad, int aget, int chan);
        LKChannelAnalyzer* GetChannelAnalyzer(int id) { return fChannelAnalyzer[id]; }
        LKChannelAnalyzer* GetChannelAnalyzer(MMChannel* channel) {
            auto caac = channel -> GetCAAC();
            auto electronicsID = GetElectronicsID(caac);
            if (electronicsID>=0)
                return GetChannelAnalyzer(electronicsID);
            return (LKChannelAnalyzer*) nullptr;
        }

        void CAACToGlobalPosition(Int_t caac,
                Double_t &posx, Double_t &posy, Double_t &posz,
                Double_t &errx, Double_t &erry, Double_t &errz);
        void CAACToGlobalPosition(Int_t cobo, Int_t asad, Int_t aget, Int_t chan,
                Double_t &posx, Double_t &posy, Double_t &posz,
                Double_t &errx, Double_t &erry, Double_t &errz);

    private:
        const Int_t  fmmnum = 1024;       ///<  # of all channels
        const Int_t  fsinum = 45;         ///<  quadrant*9
        const Int_t  fX6num = 600;        ///<  20chan*30det
        const Int_t  fCsInum = 64;        ///<  1chan*64det
        TString      fmapmmFileName;
        TString      fmapsiFileName;
        TString      fmapX6FileName;
        TString      fmapX6dimFileName;
        TString      fmapCsIFileName;

        eType        fType[3][4][4][68];
        eDetLoc      fDetLoc[3][4][4][68];

        Int_t        fmmpx[4][4][64];
        Int_t        fmmpy[4][4][64];

        Int_t        fSipx[4][4][68];        ///<  0(left) - 4(right)
        Int_t        fSipy[4][4][68];        ///<  0(down) , 1(up)
        Int_t        fSistrip[4][4][68];     ///<  1,2,3,4 (in circle)
        Int_t        fSidet[4][4][68];       ///<  0-9 (matched with Ohmic channel)

        Int_t        fCsIdet[4][4][68];

        Int_t        fX6det[4][4][68];    ///<  11-17: LS | 21-27: RS | 101-108: LB | 201-208: RB
        Int_t        fX6strip[4][4][68];  ///<  0-7(Junction), 0-3(Ohmic)
        Int_t        fX6pin[4][4][68];    ///<  1 for X6 pin side
        Double_t     fX6posx[300][8];
        Double_t     fX6posy[300][8];
        Double_t     fX6posz[300][8];

        Int_t        fCsICT[4][4][68];          ///<  Charge Terminator numbering: 1-64
        Int_t        fCsIpin[4][4][68];         ///<  1 for X6 pin side
        Int_t        fCsIX6det[4][4][68];       ///<  matched X6 num

        TString fFSiJFileName;
        TString fFCsIFileName;
        TString fX6JFileName; 
        TString fX6OFileName; 

        Double_t fSiJpar0[68];
        Double_t fSiJpar1[68];
        Double_t fCsIpar0[10];
        Double_t fCsIpar1[10];

        Double_t fX6JEpar0[30][8];
        Double_t fX6JEpar1[30][8];
        Double_t fX6JPpar0[30][8];
        Double_t fX6JPpar1[30][8];
        Double_t fX6OEpar0[30][4];
        Double_t fX6OEpar1[30][4];

        const int fNumPSAType = 18;
        const int eMMCenterSideA0   = 0;
        const int eMMCenterSideA1   = 1;
        const int eMMCenterSideA2   = 2;
        const int eMMCenterSideA3   = 3;
        const int eMMCenterCenterA0 = 4;
        const int eMMCenterCenterA1 = 5;
        const int eMMCenterCenterA2 = 6;
        const int eMMCenterCenterA3 = 7;
        const int eMMLeftSide       = 8;
        const int eMMLeftCenter     = 9;
        const int eMMRightSide      = 10;
        const int eMMRightCenter    = 11;
        const int efSiJunction      = 12;
        const int efSiOhmic         = 13;
        const int efCsI             = 14;
        const int eX6Ohmic          = 15;
        const int eX6Junction       = 16;
        const int eCsICT            = 17;

        TString fTypeNames[18] = {
            "MMCenterSideA0",
            "MMCenterSideA1",
            "MMCenterSideA2",
            "MMCenterSideA3",
            "MMCenterCenterA0",
            "MMCenterCenterA1",
            "MMCenterCenterA2",
            "MMCenterCenterA3",
            "MMLeftSide",
            "MMLeftCenter",
            "MMRightSide",
            "MMRightCenter",
            "fSiJunction",
            "fSiOhmic",
            "fCsI",
            "X6Ohmic",
            "X6Junction",
            "CsICT"
        };

        LKChannelAnalyzer* fChannelAnalyzer[18];

    ClassDef(TexAT2,1);
};

#endif
