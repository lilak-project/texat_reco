#include "LKLogger.h"

#ifdef LILAK_GEANT4
#include "LKParameterContainer.h"
#include "LKPrimaryGeneratorAction.h"
#include "LKEventAction.h"
#include "LKTrackingAction.h"
#include "LKSteppingAction.h"
#include "QGSP_BERT.hh"
#include "LKG4RunManager.h"
#include "G4StepLimiterPhysics.hh"
#include "TTDetectorConstruction.h"
#endif

int main(int argc, char** argv)
{
    lk_logger("data/log");

#ifdef LILAK_GEANT4
    auto runManager = new LKG4RunManager();

    G4VModularPhysicsList* physicsList = new QGSP_BERT;
    physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
    runManager -> SetUserInitialization(physicsList);
    runManager -> AddParameterContainer(argv[1]);
    runManager -> GetPar() -> Print();
    runManager -> SetUserInitialization(new TTDetectorConstruction());
    runManager -> Initialize();
    runManager -> Run(argc, argv);

    delete runManager;
#endif

    return 0;
}
