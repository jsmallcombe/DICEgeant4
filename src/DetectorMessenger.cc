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
// $Id: DetectorMessenger.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"

#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
:fDetector(Det)
{
	fDetSysDir = new G4UIdirectory("/DetSys/");
	fDetSysDir->SetGuidance("UI commands of this example");

	fDetDir = new G4UIdirectory("/DetSys/det/");
	fDetDir->SetGuidance("detector control");

	fAppDir = new G4UIdirectory("/DetSys/app/");
	fAppDir->SetGuidance("apparatus control");

	fWorldDir = new G4UIdirectory("/DetSys/world/");
	fWorldDir->SetGuidance("world control");

	fWorldMaterialCmd = new G4UIcmdWithAString("/DetSys/world/material",this);
	fWorldMaterialCmd->SetGuidance("Select material for the world.");
	fWorldMaterialCmd->SetParameterName("choice",false);
	fWorldMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fWorldDimensionsCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/world/dimensions",this);
	fWorldDimensionsCmd->SetGuidance("Set world dimensions - x y z unit.");
	fWorldDimensionsCmd->SetUnitCategory("Length");
	fWorldDimensionsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fWorldStepLimit = new G4UIcmdWithADoubleAndUnit("/DetSys/world/StepLimit",this);
	fWorldStepLimit->SetGuidance("Set user step limit for the world volume.");
	fWorldStepLimit->SetUnitCategory("Length");
	fWorldStepLimit->AvailableForStates(G4State_PreInit,G4State_Idle);


	fUpdateCmd = new G4UIcmdWithoutParameter("/DetSys/det/update",this);
	fUpdateCmd->SetGuidance("Update geometry.");
	fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
	fUpdateCmd->AvailableForStates(G4State_Idle);

	fTabMagneticFieldCmd = new G4UIcmdWithAString("/DetSys/world/TabMagneticField",this); ///19/7
	fTabMagneticFieldCmd->SetGuidance("Set tabulated magnetic field.");
	fTabMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
    fTabMagneticFieldMirrorPointCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/world/SetFieldMirrorPos",this);
	fTabMagneticFieldMirrorPointCmd->SetGuidance("Field mirror coord.");
	fTabMagneticFieldMirrorPointCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
    fTabMagneticFieldOffsetCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/world/SetFieldOffset",this);
	fTabMagneticFieldOffsetCmd->SetGuidance("Field offset vector");
	fTabMagneticFieldOffsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
    fTabMagneticFieldMirrorCmd = new G4UIcmdWithAnInteger("/DetSys/world/SetFieldMirror",this);
	fTabMagneticFieldMirrorCmd->SetGuidance("Field xyz 123 mirror coords.");
	fTabMagneticFieldMirrorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
    fTabMagneticFieldAntiMirrorCmd = new G4UIcmdWithAnInteger("/DetSys/world/SetFieldAntiMirror",this);
	fTabMagneticFieldAntiMirrorCmd->SetGuidance("Field xyz 123 field anti mirror.");
	fTabMagneticFieldAntiMirrorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    
	fAddApparatusDiceDetectorCmd = new G4UIcmdWithAString("/DetSys/app/addDiceDetector",this);
	fAddApparatusDiceDetectorCmd->SetGuidance("Add DICE geometry");
	fAddApparatusDiceDetectorCmd->AvailableForStates(G4State_Idle);
	
	fApparatusDiceFieldCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/World/ScaleField",this);
	fApparatusDiceFieldCmd->SetGuidance("Scale the magentic field");
	fApparatusDiceFieldCmd->SetUnitCategory("Magnetic flux density");
	fApparatusDiceFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fApparatusDiceSetCmd = new G4UIcmdWithAString("/DetSys/app/SetDiceLength",this);
	fApparatusDiceSetCmd->SetGuidance("Choose DICE detector length to set");
	fApparatusDiceSetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fApparatusDiceIntCmd = new G4UIcmdWithAString("/DetSys/app/SetDiceN",this);
	fApparatusDiceIntCmd->SetGuidance("Set DICE Int N");
	fApparatusDiceIntCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fApparatusDiceBoolCmd = new G4UIcmdWithAString("/DetSys/app/SetDiceBool",this);
	fApparatusDiceBoolCmd->SetGuidance("Set DICE bool by name");
	fApparatusDiceBoolCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{	
	delete fDetSysDir;
	delete fDetDir;
	delete fAppDir;
	delete fWorldDir;
	delete fWorldMaterialCmd;
	delete fWorldDimensionsCmd;
	delete fWorldStepLimit;
	delete fUpdateCmd;
	
	delete fTabMagneticFieldCmd;
	delete fTabMagneticFieldMirrorPointCmd;
	delete fTabMagneticFieldOffsetCmd;
	delete fTabMagneticFieldMirrorCmd;
	delete fTabMagneticFieldAntiMirrorCmd;
	
	delete fAddApparatusDiceDetectorCmd;
	delete fApparatusDiceFieldCmd;
	delete fApparatusDiceSetCmd;
	delete fApparatusDiceIntCmd;
	delete fApparatusDiceBoolCmd;
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
	if(command == fWorldMaterialCmd) {
		fDetector->SetWorldMaterial(newValue);
		return;
	}
	if(command == fWorldDimensionsCmd) {
		fDetector->SetWorldDimensions(fWorldDimensionsCmd->GetNew3VectorValue(newValue));
	}
	if(command == fUpdateCmd) {
		fDetector->UpdateGeometry();
	}
	if(command == fWorldStepLimit) {
		fDetector->SetWorldStepLimit(fWorldStepLimit->GetNewDoubleValue(newValue));
	}

	if(command == fTabMagneticFieldCmd) {
		G4String PathAndTableName;
		std::istringstream is(newValue);///string
		is>>PathAndTableName;
		fDetector->SetTabMagneticField(PathAndTableName);
	}
	
	if(command == fTabMagneticFieldMirrorPointCmd) {
		fDetector->SetFieldMirrorPoint(fTabMagneticFieldMirrorPointCmd->GetNew3VectorValue(newValue));
	}
	
	if(command == fTabMagneticFieldOffsetCmd) {
		fDetector->SetFieldOffsetVec(fTabMagneticFieldOffsetCmd->GetNew3VectorValue(newValue));
	}
	
	if(command == fTabMagneticFieldMirrorCmd) {
		fDetector->SetFieldMirror(fTabMagneticFieldMirrorCmd->GetNewIntValue(newValue),false);
	}
	
	if(command == fTabMagneticFieldAntiMirrorCmd) {
		fDetector->SetFieldMirror(fTabMagneticFieldAntiMirrorCmd->GetNewIntValue(newValue),true);
	}
	

	if(command == fAddApparatusDiceDetectorCmd) {
		fDetector->AddApparatusDiceDetector(newValue);
	}
	if(command == fApparatusDiceFieldCmd) {
		fDetector->SetDiceFieldStrength(fApparatusDiceFieldCmd->GetNewDoubleValue(newValue));
	}
	
	if(command == fApparatusDiceSetCmd) {
		G4String Name,Val,Unit;
		std::istringstream is(newValue);
		is>>Name>>Val>>Unit;
		Val+=" "+Unit;
		fDetector->DiceSetParam(Name,G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(Val));
	}
	
	if(command == fApparatusDiceIntCmd) {
		G4String Name,Val;
		std::istringstream is(newValue);
		is>>Name>>Val;
		fDetector->DiceSetParam(Name,G4UIcmdWithAnInteger::GetNewIntValue(Val));
	}
	
	if(command == fApparatusDiceBoolCmd) {
		G4String Name,Val;
		std::istringstream is(newValue);
		is>>Name>>Val;
		fDetector->DiceSetParam(Name,G4UIcmdWithABool::GetNewBoolValue(Val));
	}
	
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
