#ifndef TTEVENTHEADER_HH
#define TTEVENTHEADER_HH

#include "TClonesArray.h"
#include "LKContainer.h"
#include "LKLogger.h"

/// Event header for TexAT experiment
class TTEventHeader : public LKContainer
{
    public:
        TTEventHeader();
        virtual ~TTEventHeader() { ; }

        void Clear(Option_t *option="");
        void Print(Option_t *option="") const;
        void Copy(TObject &object) const;

        enum class eGoodSiEvt {
            kNon/*0*/,  kFSiL/*1*/, kFSiC/*2*/, kFSiR/*3*/,
            kX6LS/*4*/, kX6RS/*5*/, kX6LB/*6*/, kX6RB/*6*/,
            kFSiX6/*7*/
        };

        Bool_t     IsGoodEvent()    const  { return fIsGoodEvent; }
        Bool_t     IsMMEvent()      const  { return fIsMMEvent; }
        Bool_t     GetGoodMMEvt()   const  { return fGoodMMEvt; }
        eGoodSiEvt GetGoodSiEvt()   const  { return fGoodSiEvt; }
        Bool_t     IsListed()       const  { return fIsListed; }
        Int_t      GetFiredDet()    const  { return fFiredDet; }
        Int_t      GetFiredStrip()  const  { return fFiredStrip; }
        Int_t      GetEventNumber() const  { return fEventNumber; }
        Int_t      GetSiBLR()  const  { return fSiBLR; }
        Int_t      GetSiLhit() const  { return fSiLhit; }
        Int_t      GetSiRhit() const  { return fSiRhit; }
        Int_t      GetSiChit() const  { return fSiChit; }
        Int_t      GetX6Lhit() const  { return fX6Lhit; }
        Int_t      GetX6Rhit() const  { return fX6Rhit; }

        void SetIsGoodEvent(Bool_t isGoodEvent) { fIsGoodEvent = isGoodEvent; }
        void SetIsMMEvent(Bool_t isMMEvent) { fIsMMEvent = isMMEvent; }
        void SetGoodMMEvt(Bool_t goodMMEvt) { fGoodMMEvt = goodMMEvt; }
        void SetGoodSiEvt(eGoodSiEvt goodSiEvt) { fGoodSiEvt = goodSiEvt; }
        void SetIsListed(Bool_t isListed) { fIsListed = isListed; }
        void SetFiredDet(Int_t firedDet) { fFiredDet = firedDet; }
        void SetFiredStrip(Int_t firedStrip) { fFiredStrip = firedStrip; }
        void SetEventNumber(Int_t eventNumber) { fEventNumber = eventNumber; }
        void SetSiBLR(Int_t siBLR) { fSiBLR = siBLR; }
        void SetSiLhit(Int_t siLhit) { fSiLhit = siLhit; }
        void SetSiRhit(Int_t siRhit) { fSiRhit = siRhit; }
        void SetSiChit(Int_t siChit) { fSiChit = siChit; }
        void SetX6Lhit(Int_t x6Lhit) { fX6Lhit = x6Lhit; }
        void SetX6Rhit(Int_t x6Rhit) { fX6Rhit = x6Rhit; }

    private:
        Bool_t       fIsGoodEvent = false;
        Bool_t       fIsMMEvent = false;
        Bool_t       fGoodMMEvt = false;
        eGoodSiEvt   fGoodSiEvt = eGoodSiEvt::kNon;
        Bool_t       fIsListed = false;
        Int_t        fFiredDet = -1;
        Int_t        fFiredStrip = -1;
        Int_t        fEventNumber = -1;
        Int_t        fSiBLR = -1;
        Int_t        fSiLhit = -1;
        Int_t        fSiRhit = -1;
        Int_t        fSiChit = -1;
        Int_t        fX6Lhit = -1;
        Int_t        fX6Rhit = -1;

    ClassDef(TTEventHeader,1);
};

#endif
