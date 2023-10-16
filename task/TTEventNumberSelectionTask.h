#ifndef TTEVENTNUMBERSELECTIONTASK_HH
#define TTEVENTNUMBERSELECTIONTASK_HH

#include "LKLogger.h"
#include "LKRun.h"
#include "LKTask.h"
#include "TTEventHeader.h"
#include <iostream>
#include <fstream>

class TTEventNumberSelectionTask : public LKTask
{
    public:
        TTEventNumberSelectionTask(TString eventNumberListFileName);
        virtual ~TTEventNumberSelectionTask() {}

        bool Init();
        void Exec(Option_t *option="");

    private:
        TClonesArray* fEventHeaderHolder = nullptr;

        TString fEventNumberListFileName;
        std::ifstream fFileEventNumberList;
        int fEventNumberFromList;

    ClassDef(TTEventNumberSelectionTask,1);
};

#endif
