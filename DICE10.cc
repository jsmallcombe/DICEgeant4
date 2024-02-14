//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file analysis/AnaEx01/AnaEx01.cc
/// \brief Main program of the analysis/AnaEx01 example
//
//
// $Id: AnaEx01.cc 73919 2013-09-17 07:38:47Z gcosmo $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#include "Randomize.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
	 // G4long is at least 32 bits. 
    G4long seed = time(NULL);
    G4Random::setTheSeed(seed);

	G4RunManager* runManager = new G4RunManager;

	 // Set mandatory initialization classes
	 DetectorConstruction* detector = new DetectorConstruction;
	 runManager->SetUserInitialization(detector);
	 runManager->SetUserInitialization(new PhysicsList);
	 runManager->SetUserInitialization(new ActionInitialization(detector));

	 // We don't initialize the G4 kernel at run time so the physics list can be changed!

	 // Get the pointer to the User Interface manager
	 G4UImanager* UImanager = G4UImanager::GetUIpointer();

	 G4VisManager* visManager = new G4VisExecutive;
	 visManager->Initialize();

	G4String command = "/control/execute ";
	G4String fileName="vis.mac";
	if(argc==2)fileName = argv[1];
	else if(argc>2)fileName = argv[2];
		
	 if(argc!=2){
        // local interactive mode : define visualization and UI terminal
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand(command+fileName);
        ui->SessionStart();
        delete ui;	 
    }else{
        UImanager->ApplyCommand(command+fileName);
	}
	 
// 	 else { // remote interactive mode : define visualization and UI terminal
// 		G4UIExecutive* ui = new G4UIExecutive(argc, argv, "tcsh");
// 		UImanager->ApplyCommand("/control/execute remote_vis.mac");
// 		ui->SessionStart();
// 		delete ui;	 
// 	 }
	 
	 delete visManager;

	 // Job termination
	 delete runManager;

	 return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
