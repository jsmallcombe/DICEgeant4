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
/// \file analysis/shared/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.hh 77256 2013-11-22 10:10:23Z gcosmo $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include <unordered_map>

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4MagneticField.hh"
#include "NonUniformMagneticField.hh"
#include "TabulatedMagneticField.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

class ApparatusDICE;

//class MagneticField;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

struct DetectorProperties {
	G4int systemID;
	G4int detectorNumber;
	G4int crystalNumber;
	void Clear()
	{
		systemID = 0;
		detectorNumber = 0;
		crystalNumber = 0;
	}
};

bool operator==(const DetectorProperties& lhs, const DetectorProperties& rhs);

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
	DetectorConstruction();
	~DetectorConstruction();

	void PassEfficiencyPosition( G4ThreeVector num ) {fDetEffPosition = num;}

	void SetWorldMaterial( G4String );
	void SetWorldDimensions( G4ThreeVector );
	void SetWorldStepLimit( G4double );

	void SetTabMagneticField(G4String);

	G4VPhysicalVolume* Construct();

	void UpdateGeometry();

	void RecordGun( G4bool input ){fRecordGun = input;};
	G4bool RecordingGun(){return fRecordGun;};
	
	bool HasProperties(G4VPhysicalVolume* vol) { return fPropertiesMap.find(vol) != fPropertiesMap.end(); }
	DetectorProperties GetProperties(G4VPhysicalVolume* vol) { return fPropertiesMap.at(vol); }
	void SetProperties();
	void SetPropertiesRecursive(G4LogicalVolume* vol);

	void Print();
	void PrintRecursive(G4LogicalVolume* vol);
	
	void RemoveShield( G4bool input );
	void AddBlocker( G4bool input );
    
	void AddApparatusDiceDetector(G4String Options);
	void SetDiceFieldStrength(G4double Field);
	
	void SetDiceLength(G4int Nsel, G4double Length);
	void SetDiceInt(G4int N);
    
    ApparatusDICE* GetDice();
	
	
    TabulatedMagneticField* GetField(){
		if(fGlobalField){
			return (TabulatedMagneticField*)fGlobalField->GetField();
		}else{
			G4cout<<"NO FIELD SET. CANNOT GET FIELD."<<G4endl;
			return nullptr;
		}
	};
	
    void UpdateField(){
		if(fGlobalField){
			fGlobalField->UpdateField();
		}else{
			G4cout<<"NO FIELD SET. NOT UPDATED."<<G4endl;
		}
	};
	
	void SetFieldMirror(int,bool);
	void SetFieldMirrorPoint(G4ThreeVector);
	
private:
	bool CheckVolumeName(G4String volumeName);
	DetectorProperties ParseVolumeName(G4String volumeName);

	G4double  fWorldSizeX;
	G4double  fWorldSizeY;
	G4double  fWorldSizeZ;

    G4bool    fRecordGun;  
    G4bool    fRemoveShield;
    G4bool    fAddBlocker;

	// Box
	G4String           fBoxMat;
	G4double           fBoxThickness;
	G4ThreeVector      fBoxInnerDimensions;
	G4ThreeVector      fBoxColour;

	G4Box*             fSolidWorld;    //pointer to the solid World
	G4LogicalVolume*   fLogicWorld;    //pointer to the logical World
	G4VPhysicalVolume* fPhysiWorld;    //pointer to the physical World

	// Grid
	G4String           fGridMat;
	G4double           fGridSize;
	G4ThreeVector      fGridDimensions;
	G4ThreeVector      fGridColour;
	G4ThreeVector      fGridOffset;

	void DefineMaterials();

	G4String fMatWorldName;

	DetectorMessenger* fDetectorMessenger;

	G4ThreeVector fDetEffPosition;

	ApparatusDICE *fAppDICE;
	
	NonUniformMagneticField *fGlobalField; 
	
	//unordered maps which hold properties of the physical volumes created
	std::unordered_map<G4VPhysicalVolume*, DetectorProperties> fPropertiesMap;
	
	
};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


