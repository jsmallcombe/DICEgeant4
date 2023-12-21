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
	
   	fRecordGunCmd = new G4UIcmdWithABool("/DetSys/det/RecordGun",this);
	fRecordGunCmd->SetGuidance("Record the particle for each event in the tree");
	fRecordGunCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
    
   	fRemoveShieldCmd = new G4UIcmdWithABool("/DetSys/det/RemoveShield",this);
	fRemoveShieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
    
   	fAddBlockerCmd = new G4UIcmdWithABool("/DetSys/det/AddBlocker",this);
	fAddBlockerCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
	
	fAddApparatusDiceDetectorCmd = new G4UIcmdWithAString("/DetSys/app/addDiceDetector",this);
	fAddApparatusDiceDetectorCmd->SetGuidance("Add DICE geometry");
	fAddApparatusDiceDetectorCmd->AvailableForStates(G4State_Idle);
	
	fApparatusDiceFieldCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceField",this);
	fApparatusDiceFieldCmd->SetGuidance("Set magentic field strength of DICE.");
	fApparatusDiceFieldCmd->SetUnitCategory("Magnetic flux density");
	fApparatusDiceFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fApparatusDiceLengthCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceLength",this);
	fApparatusDiceLengthCmd->SetGuidance("Set DICE detector length.");
	fApparatusDiceLengthCmd->SetUnitCategory("Length");
	fApparatusDiceLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fApparatusDiceIntCmd = new G4UIcmdWithAnInteger("/DetSys/app/SetDiceN",this);
	fApparatusDiceIntCmd->SetGuidance("Set DICE Int N");
	fApparatusDiceIntCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fApparatusDiceArbACmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceArbA",this);
	fApparatusDiceArbACmd->SetGuidance("Set DICE detector length.");
// 	fApparatusDiceArbACmd->SetUnitCategory("Length");
	fApparatusDiceArbACmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fApparatusDiceArbBCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceArbB",this);
	fApparatusDiceArbBCmd->SetGuidance("Set DICE detector length.");
// 	fApparatusDiceArbBCmd->SetUnitCategory("Length");
	fApparatusDiceArbBCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fApparatusDiceArbCCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceArbC",this);
	fApparatusDiceArbCCmd->SetGuidance("Set DICE detector length.");
// 	fApparatusDiceArbCCmd->SetUnitCategory("Length");
	fApparatusDiceArbCCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fApparatusDiceArbDCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceArbD",this);
	fApparatusDiceArbDCmd->SetGuidance("Set DICE detector length.");
// 	fApparatusDiceArbDCmd->SetUnitCategory("Length");
	fApparatusDiceArbDCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fApparatusDiceArbECmd = new G4UIcmdWithADoubleAndUnit("/DetSys/app/SetDiceArbE",this);
	fApparatusDiceArbECmd->SetGuidance("Set DICE detector length.");
// 	fConeAngleCmd->SetUnitCategory("Angle");
	fApparatusDiceArbECmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
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

	delete fRecordGunCmd;
    
    delete fRemoveShieldCmd;
    delete fAddBlockerCmd;
	
	delete fAddApparatusDiceDetectorCmd;
	delete fApparatusDiceFieldCmd;
	delete fApparatusDiceLengthCmd;
	delete fApparatusDiceIntCmd;
	delete fApparatusDiceArbACmd;
	delete fApparatusDiceArbBCmd;
	delete fApparatusDiceArbCCmd;
	delete fApparatusDiceArbDCmd;
	delete fApparatusDiceArbECmd;
	
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
		G4double z_offset, z_rotation;
		std::istringstream is(newValue);///string
		is>>PathAndTableName>>z_offset>>z_rotation;
		fDetector->SetTabMagneticField(PathAndTableName, z_offset, z_rotation); // z in mm, angle in degree  
	}
	
	if(command == fRecordGunCmd) {
		fDetector->RecordGun(fRecordGunCmd->GetNewBoolValue(newValue));
	}
	
	if(command == fRemoveShieldCmd) {
		fDetector->RemoveShield(fRemoveShieldCmd->GetNewBoolValue(newValue));
	}
	
	if(command == fAddBlockerCmd) {
		fDetector->AddBlocker(fAddBlockerCmd->GetNewBoolValue(newValue));
	}
	
	if(command == fAddApparatusDiceDetectorCmd) {
		fDetector->AddApparatusDiceDetector(newValue);
	}
	if(command == fApparatusDiceFieldCmd) {
		fDetector->SetDiceFieldStrength(fApparatusDiceFieldCmd->GetNewDoubleValue(newValue));
	}
	
	if(command == fApparatusDiceIntCmd) {
		fDetector->SetDiceInt(fApparatusDiceIntCmd->GetNewIntValue(newValue));
	}
	
	if(command == fApparatusDiceLengthCmd) {
		fDetector->SetDiceLength(0,fApparatusDiceLengthCmd->GetNewDoubleValue(newValue));
	}
	if(command == fApparatusDiceArbACmd) {
		fDetector->SetDiceLength(1,fApparatusDiceArbACmd->GetNewDoubleValue(newValue));
	}
	if(command == fApparatusDiceArbBCmd) {
		fDetector->SetDiceLength(2,fApparatusDiceArbBCmd->GetNewDoubleValue(newValue));
	}
	if(command == fApparatusDiceArbCCmd) {
		fDetector->SetDiceLength(3,fApparatusDiceArbCCmd->GetNewDoubleValue(newValue));
	}	
	if(command == fApparatusDiceArbDCmd) {
		fDetector->SetDiceLength(4,fApparatusDiceArbDCmd->GetNewDoubleValue(newValue));
	}	
	if(command == fApparatusDiceArbECmd) {
		fDetector->SetDiceLength(5,fApparatusDiceArbECmd->GetNewDoubleValue(newValue));
	}	
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
