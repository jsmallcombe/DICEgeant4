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
//
// $Id: PrimaryGeneratorMessenger.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* Gun)
:fAction(Gun)
{

	fEfficiencyEnergyCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/efficiencyEnergy",this);
	fEfficiencyEnergyCmd->SetGuidance("Set gamma efficiency energy");
	fEfficiencyEnergyCmd->SetUnitCategory("Energy");
	fEfficiencyEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fEfficiencyDirectionCmd = new G4UIcmdWith3Vector("/DetSys/gun/direction",this);
	fEfficiencyDirectionCmd->SetGuidance("Set momentum direction.");
	fEfficiencyDirectionCmd->SetGuidance("Direction needs not to be a unit vector.");
	fEfficiencyDirectionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fEfficiencyPositionCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/gun/position",this);
	fEfficiencyPositionCmd->SetGuidance("Set position.");
	fEfficiencyPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fEfficiencyParticleCmd = new G4UIcmdWithAString("/DetSys/gun/particle",this);
	fEfficiencyParticleCmd->SetGuidance("Set particle.");
	fEfficiencyParticleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fConeAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMaxAngle",this);
	fConeAngleCmd->SetGuidance("Set cone value for outer theta - use deg (0-90)");
	fConeAngleCmd->SetUnitCategory("Angle");
	fConeAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fConeMinAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMinAngle",this);
	fConeMinAngleCmd->SetGuidance("Set cone value for inner theta - use deg (0-90) - default is 0 if none specified");
	fConeMinAngleCmd->SetUnitCategory("Angle");
	fConeMinAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
	fPhiMinAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMinPhi",this);
	fPhiMinAngleCmd->SetUnitCategory("Angle");
	fPhiMinAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fPhiMaxAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMaxPhi",this);
	fPhiMaxAngleCmd->SetUnitCategory("Angle");
	fPhiMaxAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fBeamSpotSigmaCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/BeamSpot",this);//Beam spot sigma
	fBeamSpotSigmaCmd->SetGuidance("Set sigma for a realistic beamspot");
	fBeamSpotSigmaCmd->SetUnitCategory("Length");
	fBeamSpotSigmaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   

	fSetGunBetaCmd = new G4UIcmdWithADouble("/DetSys/gun/SetBeta",this);//Beam spot sigma
	fSetGunBetaCmd->SetGuidance("Set Z beta for gun");
	fSetGunBetaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   	

	fSetGunLifetimeCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/SetLifetime",this);//Beam spot sigma
	fSetGunLifetimeCmd->SetGuidance("Set lifetime for gun emission");
	fSetGunLifetimeCmd->SetUnitCategory("Time");
	fSetGunLifetimeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   
	
	fStoreHits = new G4UIcmdWithoutParameter("/DetSys/SiHits/VisStore",this);
	fStoreHits->AvailableForStates(G4State_Idle);
    
    fSetStoreMult = new G4UIcmdWithAnInteger("/DetSys/SiHits/StoreMultiplicity",this);
	fSetStoreMult->AvailableForStates(G4State_Idle);
	
	fSetPrettyHitsOnly = new G4UIcmdWithoutParameter("/DetSys/SiHits/PrettyOnly",this);
	fSetPrettyHitsOnly->AvailableForStates(G4State_Idle);
	
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
	delete fEfficiencyEnergyCmd;
	delete fEfficiencyDirectionCmd;
	delete fConeAngleCmd;
	delete fConeMinAngleCmd;
	delete fPhiMinAngleCmd;
	delete fPhiMaxAngleCmd;
	delete fBeamSpotSigmaCmd;
	delete fSetGunBetaCmd;
	delete fSetGunLifetimeCmd;
	delete fStoreHits;
	delete fSetStoreMult;
	delete fSetPrettyHitsOnly;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
	if(command == fEfficiencyEnergyCmd) {
		fAction->SetEfficiencyEnergy(fEfficiencyEnergyCmd->GetNewDoubleValue(newValue));
		return;
	}
	if(command == fEfficiencyDirectionCmd) {
		fAction->SetEfficiencyDirection(fEfficiencyDirectionCmd->GetNew3VectorValue(newValue));
		return;
	}
	if(command == fEfficiencyPositionCmd) {
		fAction->SetEfficiencyPosition(fEfficiencyPositionCmd->GetNew3VectorValue(newValue));
		return;
	}
	if(command == fEfficiencyParticleCmd) {
		fAction->SetEfficiencyParticle(newValue);
		return;
	}
	if(command == fConeAngleCmd) {
		fAction->SetConeMaxAngle(fConeAngleCmd->GetNewDoubleValue(newValue));
		G4cout<<"Cone Beam via Angle selected"<<G4endl;
		return;
	}
	if(command == fConeMinAngleCmd) {
		fAction->SetConeMinAngle(fConeMinAngleCmd->GetNewDoubleValue(newValue));
		G4cout<<"Cone Beam minimum angle supplied"<<G4endl;
		return;
	}
	if(command == fPhiMinAngleCmd) {
		fAction->SetConeMinPhi(fPhiMinAngleCmd->GetNewDoubleValue(newValue));
		return;
	}
	if(command == fPhiMaxAngleCmd) {
		fAction->SetConeMaxPhi(fPhiMaxAngleCmd->GetNewDoubleValue(newValue));
		return;
	}
	if(command == fBeamSpotSigmaCmd) {
		fAction->SetBeamSpotSigma(fBeamSpotSigmaCmd->GetNewDoubleValue(newValue));
		G4cout<<"Beam Spot sigma supplied"<<G4endl;
		return;
	}  
	if(command == fSetGunBetaCmd) {
		fAction->SetGunBeta(fSetGunBetaCmd->GetNewDoubleValue(newValue));
		G4cout<<"Gun Beta Z supplied"<<G4endl;
		return;
	}  
	
	if(command == fSetGunLifetimeCmd) {
		fAction->SetGunLifetime(fSetGunLifetimeCmd->GetNewDoubleValue(newValue));
		G4cout<<"Gun Lifetime supplied"<<G4endl;
		return;
	}  
	
	if(command == fStoreHits) {
		fAction->StoreHits();
		G4cout<<"Vis Store Set"<<G4endl;
		return;
	}  
		
	if(command == fSetStoreMult) {
		fAction->SetStoreMult(fSetStoreMult->GetNewIntValue(newValue));
		G4cout<<"Vis Store Mult Set"<<G4endl;
		return;
	}  
	
	if(command == fSetPrettyHitsOnly) {
		fAction->SetPrettyHitsOnly();
		G4cout<<"Vis Store Pretty Only Set"<<G4endl;
		return;
	}  
	
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
