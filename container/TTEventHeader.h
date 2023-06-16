#ifndef TTEVENTHEADER_HH
#define TTEVENTHEADER_HH

#include "TClonesArray.h"
#include "LKContainer.h"
#include "LKLogger.h"

/*
 * Remove this comment block after reading it through
 * Or use print_example_comments=False option to omit printing
 *
 * # Example LILAK container class
 *
 * ## Must
 * - Write Clear() method
 * : Clear() method clears and intialize the data class.
 * This is "Must Write Method" because containers are not recreated each event,
 * but their memories are reused after Clear method is called.
 * and they are filled up and written to tree each event.
 * See: https://root.cern/doc/master/classTClonesArray.html, https://opentutorials.org/module/2860/19477
 *
 * - Version number (2nd par. in ClassDef of source file) should be changed if the class has been modified.
 * This notifiy users that the container has been update in the new LILAK (or side project version).
 *
 * ## Recommended
 * - Documentaion like this!
 * - Write Print() to see what is inside the container;
 *
 * ## If you have time
 * - Write Copy() for copying object
 */

/**
 * TexAT event header
 */
class TTEventHeader : public LKContainer
{
    public:
        TTEventHeader();
        virtual ~TTEventHeader() { ; }

        void Clear(Option_t *option="");
        void Print(Option_t *option="") const;
        void Copy(TObject &object) const;

        bool GetIsGoodEvent() const  { return fIsGoodEvent; }
        Int_t GetSiBLR() const  { return fSiBLR; }
        Int_t GetSiLhit() const  { return fsiLhit; }
        Int_t GetSiRhit() const  { return fsiRhit; }
        Int_t GetSiChit() const  { return fsiChit; }
        Int_t GetX6Lhit() const  { return fX6Lhit; }
        Int_t GetX6Rhit() const  { return fX6Rhit; }

        void SetIsGoodEvent(bool isGoodEvent) { fIsGoodEvent = isGoodEvent; }
        void SetSiBLR(Int_t SiBLR) { fSiBLR = SiBLR; }
        void SetSiLhit(Int_t siLhit) { fsiLhit = siLhit; }
        void SetSiRhit(Int_t siRhit) { fsiRhit = siRhit; }
        void SetSiChit(Int_t siChit) { fsiChit = siChit; }
        void SetX6Lhit(Int_t X6Lhit) { fX6Lhit = X6Lhit; }
        void SetX6Rhit(Int_t X6Rhit) { fX6Rhit = X6Rhit; }

        bool         fIsGoodEvent = true;
        Int_t        fSiBLR = -1;
        Int_t        fsiLhit = -1;
        Int_t        fsiRhit = -1;
        Int_t        fsiChit = -1;
        Int_t        fX6Lhit = -1;
        Int_t        fX6Rhit = -1;

    ClassDef(TTEventHeader,1);
};

#endif
