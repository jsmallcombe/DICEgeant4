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
/// \file analysis/shared/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
// $Id: SteppingAction.cc 68015 2013-03-13 13:27:27Z gcosmo $
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4HadronicProcess.hh"
#include "G4ProcessType.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* detcon,
		EventAction* evt)
: G4UserSteppingAction(),
	fDetector(detcon), fEventAction(evt)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction() { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep) {
	
	
	if(fEventAction->fHistoManager->PrettyHitsOnly){
// 		if (aStep->GetTrack()->GetParticleDefinition() == G4Gamma::Gamma()){
		if (aStep->GetTrack()->GetParticleDefinition() != fDetector->fParticle){
			// When we are using our fake pretty setting, kill all gammas immediatly.
			// avoids registering an "exit volume" when doing pairs
			aStep->GetTrack()->SetTrackStatus(fStopAndKill);
			return;
		}
	}
		
	
	G4bool trackSteps   = false;
	G4int processType   = -1;
	G4int evntNb        = 0;

	// Get volume of the current step
	G4VPhysicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

	// collect energy and track length step by step
	// As it's called more than once, get the Track and assign to variable
	G4double edep = aStep->GetTotalEnergyDeposit();
	G4double ekin = aStep->GetPreStepPoint()->GetKineticEnergy();

	G4Track* theTrack = aStep->GetTrack();
	G4int stepNumber = theTrack->GetCurrentStepNumber();

	// Track particle type in EVERY step
	G4int particleType  = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

	const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();
	G4int targetZ = -1;
	if(process != nullptr && process->GetProcessType() == fHadronic) {
		G4HadronicProcess* hadrProcess = (G4HadronicProcess*) process;
		const G4Isotope* target = nullptr;
		target = hadrProcess->GetTargetIsotope();
		if(target != nullptr) {
			targetZ = target->GetZ();
		}
	}

	// this can be modified to add more processes
	if(theTrack->GetCreatorProcess() != nullptr) {
		G4String processName = theTrack->GetCreatorProcess()->GetProcessName();
		if(processName == "RadioactiveDecay")      processType = 1;
		else if(processName == "eIoni")            processType = 2;
		else if(processName == "msc")              processType = 3;
		else if(processName == "Scintillation")    processType = 4;
		else if(processName == "Cerenkov")         processType = 5;
		else                                       processType = 0;
	}

	evntNb =  fEventAction->GetEventNumber();

	// Get initial momentum direction & energy of particle
	G4int trackID = theTrack->GetTrackID();
	G4int parentID = theTrack->GetParentID();
	

	//G4StepPoint* prePoint = aStep->GetPreStepPoint();
	G4StepPoint* postPoint = aStep->GetPostStepPoint();
	G4ThreeVector postPos = postPoint->GetPosition();
	G4double postTime = postPoint->GetGlobalTime();

	// check if this volume has its properties set, i.e. it's an active detector
	if((edep > 0 || (ekin > 0)) && fDetector->HasProperties(volume)) {
		DetectorProperties prop = fDetector->GetProperties(volume);

		// check edep again in case we use the grid cell but haven't hit it
		if(edep <= 0) return;

		fEventAction->AddHitTracker(prop, evntNb, trackID, parentID, stepNumber, particleType, processType, edep, postPos, postTime, targetZ);

		if(trackSteps) {
			fEventAction->AddStepTracker(prop, evntNb, trackID, parentID, stepNumber, particleType, processType, edep, postPos, postTime, targetZ);
		}
		
		// At least this track ends in active volume, so hopefully is a full energy event
		// This is for making pretty pictures not real simulations
		G4int trackStatus = theTrack->GetTrackStatus();
		if(trackStatus==fStopAndKill){
			fEventAction->GoodTrackEnd();
		}
		
	}// if(fDetector->HasProperties(volume))
	
	
	
	if(!theTrack->GetNextVolume()){
		
		// No next volume means exit world
		// Getting rid of these is good if you want a pretty picture
		fEventAction->TrackExitsWorld();
		
	}else{
		//There is a next volume 
		
		G4String NextLogName=theTrack->GetNextVolume()->GetLogicalVolume()->GetName();
		
        if(NextLogName.contains("FORBID")){//Parts of the DICE Geometry we have labelled as bad events
            if(fEventAction->fHistoManager->PrettyHitsOnly){
				fEventAction->TrackExitsWorld(); //This just tells our own variable that one of the tracks in the even "exited the world" i.e. did something forbidden.
				theTrack->SetTrackStatus(fStopAndKill); //Actually kill it, because why track further, when set to "pretty only" mode
			}
        }
        
        // If entering the BB34
		if(NextLogName.contains("ChipMetalLog")){
			if(NextLogName!=volume->GetLogicalVolume()->GetName()){
				G4ThreeVector preDir=aStep->GetPreStepPoint()->GetMomentumDirection();
				fEventAction->fHistoManager->SetBB34Penetrate(preDir, postPos);
			}
		}
    }
}

