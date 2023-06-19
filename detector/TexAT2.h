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
            kMMJr,          // 8
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

        eType   GetType  (int cobo, int asad, int aget, int dchan) { return fType  [cobo][asad][aget][dchan]; }
        eDetLoc GetDetLoc(int cobo, int asad, int aget, int dchan) { return fDetLoc[cobo][asad][aget][dchan]; }
        Int_t   Getmmpx            (int asad, int aget, int dchan) { return fmmpx        [asad][aget][dchan]; }
        Int_t   Getmmpy            (int asad, int aget, int dchan) { return fmmpy        [asad][aget][dchan]; }

    private:
        const Int_t  fmmnum = 1024;       ///<  # of all channels
        const Int_t  fsinum = 45;         ///<  quadrant*9
        const Int_t  fX6num = 600;        ///<  20chan*30det
        const Int_t  fCsInum = 64;        ///<  1chan*64det
        TString      fmapmmFileName;
        TString      fmapsiFileName;
        TString      fmapX6FileName;
        TString      fmapCsIFileName;

        eType        fType[3][4][4][68];
        eDetLoc      fDetLoc[3][4][4][68];
        Int_t        fmmpx[4][4][64];
        Int_t        fmmpy[4][4][64];

    ClassDef(TexAT2,1);
};

#endif
