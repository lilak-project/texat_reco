#ifndef TEXAT2_HH
#define TEXAT2_HH

#include "LKDetector.h"
#include "LKLogger.h"

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
            kCenterFront,   // 2
            kBottomLeftX6,  // 10
            kBottomRightX6, // 11
            kCsI,           // -1
        };

        void Print(Option_t *option="") const;
        bool Init();
        bool BuildGeometry();
        bool BuildDetectorPlane();
        bool IsInBoundary(Double_t x, Double_t y, Double_t z);

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

    ClassDef(TexAT2,1);
};

#endif
