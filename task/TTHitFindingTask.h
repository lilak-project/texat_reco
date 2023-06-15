#ifndef TTHITFINDINGTASK_HH
#define TTHITFINDINGTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"

/*
 * Remove this comment block after reading it through
 * Or use print_example_comments=False option to omit printing
 *
 * # Example LILAK task class
 *
 * - Write Init() method.
 * - Write Exec() or/and EndOfRun() method.
 */

class TTHitFindingTask : public LKTask
{
    public:
        TTHitFindingTask();
        virtual ~TTHitFindingTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:
        TClonesArray *fChannelArray = nullptr;
        TClonesArray *fHitArray = nullptr;

    ClassDef(TTHitFindingTask,1);
};

#endif
