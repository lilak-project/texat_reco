#ifndef TTBOTTOMARRAY_HH
#define TTBOTTOMARRAY_HH


#include "LKDetectorPlane.h"
#include "LKLogger.h"


/*
 * Remove this comment block after reading it through
 * Or use print_example_comments=False option to omit printing
 * 
 * # Example LILAK detector plane class
 * 
 * # Given members in LKDetectorPlane class
 * 
 * ## public:
 * - void SetDetector(LKDetector *detector);
 * - void SetPlaneID(Int_t id);
 * - Int_t GetPlaneID() const;
 * - void AddChannel(LKChannel *channel);
 * - LKChannel *GetChannelFast(Int_t idx);
 * - LKChannel *GetChannel(Int_t idx);
 * - Int_t GetNChannels();
 * - TObjArray *GetChannelArray();
 * - LKVector3::Axis GetAxis1();
 * - LKVector3::Axis GetAxis2();
 * 
 * ## protected:
 * - TObjArray *fChannelArray = nullptr;
 * - Int_t fPlaneID = -1;
 * - TCanvas *fCanvas = nullptr;
 * - TH2 *fH2Plane = nullptr;
 * - LKVector3::Axis fAxis1 = LKVector3::kX;
 * - LKVector3::Axis fAxis2 = LKVector3::kY;
 * - LKDetector *fDetector = nullptr;
 * 
 */



class TTBottomArray : public LKDetectorPlane
{
    public:
        TTBottomArray();
        virtual ~TTBottomArray() { ; }


        void Print(Option_t *option="") const;
        bool Init();
        bool IsInBoundary(Double_t i, Double_t j);
        Int_t FindChannelID(Double_t i, Double_t j);
        TCanvas* GetCanvas(Option_t *option="");
        TH2* GetHist(Option_t *option="");
        bool DrawEvent(Option_t *option="");
        bool SetDataFromBranch();
        void DrawHist();
        void DrawFrame(Option_t *option="");

        void MouseClickEvent(int iPlane);
        void ClickedAtPosition(Double_t x, Double_t y);
        
        

    ClassDef(TTBottomArray,1);
};

#endif
