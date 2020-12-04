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

	fWorldVisCmd = new G4UIcmdWithABool("/DetSys/world/vis",this);
	fWorldVisCmd->SetGuidance("Set the visulization of the world");
	fWorldVisCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fWorldMagneticFieldCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/world/magneticField",this);
	fWorldMagneticFieldCmd->SetGuidance("Set world magnetic field - x y z unit.");
	fWorldMagneticFieldCmd->SetUnitCategory("Magnetic flux density");
	fWorldMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fWorldStepLimit = new G4UIcmdWithADoubleAndUnit("/DetSys/world/StepLimit",this);
	fWorldStepLimit->SetGuidance("Set user step limit for the world volume.");
	fWorldStepLimit->SetUnitCategory("Length");
	fWorldStepLimit->AvailableForStates(G4State_PreInit,G4State_Idle);
 
	fFieldBoxMaterialCmd = new G4UIcmdWithAString("/DetSys/app/fieldBoxMaterial",this);
	fFieldBoxMaterialCmd->SetGuidance("Select material of the target.");
	fFieldBoxMaterialCmd->SetParameterName("choice",false);
	fFieldBoxMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fFieldBoxDimensionsCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/app/fieldBoxDimensions",this);
	fFieldBoxDimensionsCmd->SetGuidance("Set target dimensions - x y z unit.");
	fFieldBoxDimensionsCmd->SetUnitCategory("Length");
	fFieldBoxDimensionsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fFieldBoxPositionCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/app/fieldBoxPosition",this);
	fFieldBoxPositionCmd->SetGuidance("Set target position - x y z unit.");
	fFieldBoxPositionCmd->SetUnitCategory("Length");
	fFieldBoxPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fFieldBoxMagneticFieldCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/app/fieldBoxMagneticField",this);
	fFieldBoxMagneticFieldCmd->SetGuidance("Set magnetic field - x y z unit.");
	fFieldBoxMagneticFieldCmd->SetUnitCategory("Magnetic flux density");
	fFieldBoxMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fTabMagneticFieldCmd = new G4UIcmdWithAString("/DetSys/world/TabMagneticField",this); ///19/7
	fTabMagneticFieldCmd->SetGuidance("Set tabulated magnetic field.");
	fTabMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	// Box Stuff
	fAddBoxMatCmd = new G4UIcmdWithAString("/DetSys/det/boxMat",this);
	fAddBoxMatCmd->SetGuidance("Set box material.");
	fAddBoxMatCmd->SetParameterName("choice",false);
	fAddBoxMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fAddBoxThicknessCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/det/boxThickness",this);
	fAddBoxThicknessCmd->SetGuidance("Set box thickness.");
	fAddBoxThicknessCmd->SetUnitCategory("Length");
	fAddBoxThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fAddBoxInnerDimensionsCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/det/boxInnerDimensions",this);
	fAddBoxInnerDimensionsCmd->SetGuidance("Set box inner dimensions.");
	fAddBoxInnerDimensionsCmd->SetUnitCategory("Length");
	fAddBoxInnerDimensionsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fAddBoxColourCmd = new G4UIcmdWith3Vector("/DetSys/det/boxColour",this);
	fAddBoxColourCmd->SetGuidance("Set box colour.");
	fAddBoxColourCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fAddBoxCmd = new G4UIcmdWithoutParameter("/DetSys/det/addBox",this);
	fAddBoxCmd->SetGuidance("Add a box.");
	fAddBoxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fAddApparatusDiceDetectorCmd = new G4UIcmdWithAString("/DetSys/app/addDiceDetector",this);
	fAddApparatusDiceDetectorCmd->SetGuidance("Add DICE geometry");
	fAddApparatusDiceDetectorCmd->AvailableForStates(G4State_Idle);

	fUpdateCmd = new G4UIcmdWithoutParameter("/DetSys/det/update",this);
	fUpdateCmd->SetGuidance("Update geometry.");
	fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
	fUpdateCmd->AvailableForStates(G4State_Idle);

   	fRecordGunCmd = new G4UIcmdWithABool("/DetSys/det/RecordGun",this);
	fRecordGunCmd->SetGuidance("Record the particle for each event in the tree");
	fRecordGunCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{	
	delete fDetDir;
	delete fAppDir;
	delete fWorldDir;
	delete fWorldMaterialCmd;
	delete fWorldDimensionsCmd;
	delete fWorldVisCmd;
	delete fWorldMagneticFieldCmd;
	delete fWorldStepLimit;
	delete fDetSysDir;
	delete fUpdateCmd;

	delete fFieldBoxMaterialCmd;
	delete fFieldBoxDimensionsCmd;
	delete fFieldBoxPositionCmd;
	delete fFieldBoxMagneticFieldCmd;
	delete fTabMagneticFieldCmd;
	delete fAddBoxMatCmd;
	delete fAddBoxThicknessCmd;
	delete fAddBoxInnerDimensionsCmd;
	delete fAddBoxColourCmd;
	delete fAddBoxCmd;

	delete fAddApparatusDiceDetectorCmd;
	
	delete fRecordGunCmd;
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
	if(command == fWorldVisCmd) {
		fDetector->SetWorldVis(fWorldVisCmd->GetNewBoolValue(newValue));
	}
	if(command == fUpdateCmd) {
		fDetector->UpdateGeometry();
	}
	if(command == fWorldStepLimit) {
		fDetector->SetWorldStepLimit(fWorldStepLimit->GetNewDoubleValue(newValue));
	}

	if(command == fTabMagneticFieldCmd) {
		G4String PathAndTableName;
		G4double z_offset, z_rotation;
		std::istringstream is(newValue);///string
		is>>PathAndTableName>>z_offset>>z_rotation;
		fDetector->SetTabMagneticField(PathAndTableName, z_offset, z_rotation); // z in mm, angle in degree  
	}
	if(command == fAddApparatusDiceDetectorCmd) {
		fDetector->AddApparatusDiceDetector(newValue);
	}
	
	if(command == fRecordGunCmd) {
		fDetector->RecordGun(fRecordGunCmd->GetNewBoolValue(newValue));
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
