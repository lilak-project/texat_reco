#include "TTEventNumberSelectionTask.h"

ClassImp(TTEventNumberSelectionTask);

TTEventNumberSelectionTask::TTEventNumberSelectionTask(TString eventNumberListFileName)
{
    fName = "TTEventNumberSelectionTask";

    fEventNumberListFileName = eventNumberListFileName;
}

bool TTEventNumberSelectionTask::Init()
{
    lk_info << "Initializing TTEventNumberSelectionTask" << std::endl;

    fFileEventNumberList.open(fEventNumberListFileName);
    if (fFileEventNumberList.is_open()==false) {
        lk_error << "list file " << fEventNumberListFileName << " cannot be opened!" << std::endl;
        return false;
    }
    lk_info << "Assuming event list in file \"" << fEventNumberListFileName << "\" has ordered list of event numbers from earlier to later!" << endl;

    fFileEventNumberList >> fEventNumberFromList;

    fEventHeaderHolder = fRun -> KeepBranchA("EventHeader");

    return true;
}


void TTEventNumberSelectionTask::Exec(Option_t *option)
{
    auto eventHeader = (TTEventHeader*) fEventHeaderHolder -> At(0);

    if (fFileEventNumberList.eof()) {
        eventHeader -> SetIsGoodEvent(false);
        lk_info << "End of list!" << std::endl;
        return;
    }

    int currentEventNumber = eventHeader -> GetEventNumber();
    while (fEventNumberFromList<currentEventNumber)
        if (!(fFileEventNumberList >> fEventNumberFromList))
            break;

    if (currentEventNumber==fEventNumberFromList) {
        lk_info << "Event number " << currentEventNumber << " exist in the list!" << std::endl;
        eventHeader -> SetIsGoodEvent(true);
        return;
    }

    eventHeader -> SetIsGoodEvent(false);
    lk_info << "Event number " << currentEventNumber << " do not exist in the list" << std::endl;
    return;
}
