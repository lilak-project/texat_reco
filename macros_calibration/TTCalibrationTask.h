#ifndef TTCALIBRATIONTASK_HH
#define TTCALIBRATIONTASK_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "MMChannel.h"
#include "TTChannelCalibrationData.h"

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example LILAK task class
 *
 * - Write Init() method.
 * - Write Exec() or/and EndOfRun() method.
 */

/**
 * This include channel pedestal, creating general pulse shape
 */
class TTCalibrationTask : public LKTask
{
    public:
        TTCalibrationTask();
        virtual ~TTCalibrationTask() { ; }

        bool Init();
        void Exec(Option_t *option="");
        bool EndOfRun();

    private:

        TClonesArray *fChannelArray = nullptr;
        TClonesArray *fCalibrationDataArray = nullptr;

        bool fCalibrateChannel = true;
        bool fCalibrateAGET = false;

    ClassDef(TTCalibrationTask,1);
};

#endif
