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
// $Id: DetectorMessenger.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
public:
	DetectorMessenger(DetectorConstruction*) ;
	~DetectorMessenger();

	void SetNewValue(G4UIcommand*, G4String);

private:
	DetectorConstruction* fDetector;

	G4UIdirectory*             fDetDir;
	G4UIdirectory*             fAppDir;
	G4UIdirectory*             fWorldDir;
	G4UIdirectory*             fDetSysDir;
	G4UIcmdWithAString*        fWorldMaterialCmd;
	G4UIcmdWith3VectorAndUnit* fWorldDimensionsCmd;
	G4UIcmdWithADoubleAndUnit* fWorldStepLimit;
	G4UIcmdWithoutParameter*   fUpdateCmd;

	//field for DICE
	G4UIcmdWithAString*        fTabMagneticFieldCmd;
	
	G4UIcmdWith3VectorAndUnit* fTabMagneticFieldMirrorPointCmd;
	G4UIcmdWith3VectorAndUnit* fTabMagneticFieldOffsetCmd;
	G4UIcmdWithAnInteger* 	   fTabMagneticFieldMirrorCmd;
	G4UIcmdWithAnInteger*	   fTabMagneticFieldAntiMirrorCmd;

	G4UIcmdWithAString*        fAddApparatusDiceDetectorCmd;
	G4UIcmdWithADoubleAndUnit* fApparatusDiceFieldCmd;
	G4UIcmdWithAString*        fApparatusDiceSetCmd;
	G4UIcmdWithAString*        fApparatusDiceIntCmd;
	G4UIcmdWithAString*        fApparatusDiceBoolCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

