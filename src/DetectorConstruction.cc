
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
/// \file analysis/shared/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.cc 77256 2013-11-22 10:10:23Z gcosmo $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include <iostream>
#include <sstream>

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"

#include "G4Threading.hh"

#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "NonUniformMagneticField.hh"

#include "ApparatusDICE.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool operator==(const DetectorProperties& lhs, const DetectorProperties& rhs)
{
	return (lhs.systemID == rhs.systemID && lhs.detectorNumber == rhs.detectorNumber && lhs.crystalNumber == rhs.crystalNumber);
}

DetectorConstruction::DetectorConstruction() :
	fSolidWorld(nullptr),
	fLogicWorld(nullptr),
	fPhysiWorld(nullptr)
{
	fWorldSizeX  = fWorldSizeY = fWorldSizeZ = 10.0*m;

	// materials
	DefineMaterials();

	//  builtDetectors = false;

	fMatWorldName = "G4_AIR";


	// create commands for interactive definition

	fDetectorMessenger = new DetectorMessenger(this);

	// ensure the global field is initialized
	//(void)GlobalField::getObject();

	//expHallMagField = new MagneticField(); // Global field is set to zero
	
	fAppDICE= new ApparatusDICE();

    fRecordGun = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {	
	delete fDetectorMessenger;
	delete fAppDICE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {

	// Replaced by ConstructDetectionSystems

	// Experimental hall (world volume)
	// search the world material by its name

	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();

	G4Material* matWorld = G4Material::GetMaterial(fMatWorldName);

	if(!matWorld) {
		G4cout<<" ----> Material "<<fMatWorldName<<" not found, cannot build world! "<<G4endl;
		return 0;
	}

	fSolidWorld = new G4Box("World", fWorldSizeX/2,fWorldSizeY/2,fWorldSizeZ/2);

	fLogicWorld = new G4LogicalVolume(fSolidWorld,		//its solid
			matWorld,	//its material
			"World");		//its name

	fPhysiWorld = new G4PVPlacement(0,                  //no rotation
			G4ThreeVector(),	//at (0,0,0)
			fLogicWorld,         //its logical volume
			"World",            //its name
			0,                  //its mother  volume
			false,              //no boolean operation
			0);                 //copy number

	// Visualization Attributes

	fLogicWorld->SetVisAttributes (G4VisAttributes::Invisible); // The following block of code works too.

	//  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	//  worldVisAtt->SetForceWireframe(true);
	//  worldVisAtt->SetVisibility(worldVis);
	//  fLogicWorld->SetVisAttributes(worldVisAtt);
	//  fLogicWorld = fLogicWorld;

	return fPhysiWorld;
}

void DetectorConstruction::SetWorldMaterial(G4String name) {
	fMatWorldName = name;
	UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldDimensions(G4ThreeVector vec) {
	fWorldSizeX = vec.x() ;
	fWorldSizeY = vec.y() ;
	fWorldSizeZ = vec.z() ;
	UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldStepLimit(G4double step) {
	if(fLogicWorld == nullptr) {
		Construct();
	}
	fLogicWorld->SetUserLimits(new G4UserLimits(step));
}

void DetectorConstruction::SetTabMagneticField(G4String PathAndTableName, G4double z_offset, G4double z_rotation)
{
	//const char * c = PathAndTableName.c_str();///conversion attempt using .c_str() (a string member function)
	new NonUniformMagneticField(PathAndTableName,z_offset,z_rotation); /// addition of field for DICE
}

void DetectorConstruction::UpdateGeometry() {
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

/////////////////////////////////////////////////////////////////////////
/////////////////// Detector "Properties" from GRSI /////////////////////
/////////////////////////////////////////////////////////////////////////

void DetectorConstruction::SetProperties() {
	// loop over all existing daughters of the world volume
	// check if their properties are set and if not, set them
	if(fLogicWorld == nullptr) return;
	// thread id is -1 for master, -2 in sequential mode
	// so this only outputs the number of volumes once
	if(G4Threading::G4GetThreadId() < 0) {
		G4cout<<fLogicWorld->GetNoDaughters()<<" daughter volumes"<<std::endl;
	}
    SetPropertiesRecursive(fLogicWorld);
}


void DetectorConstruction::SetPropertiesRecursive(G4LogicalVolume* vol) {
    for(unsigned int i = 0; i < (unsigned)vol->GetNoDaughters(); ++i) {
		if(!HasProperties(vol->GetDaughter(i)) && CheckVolumeName(vol->GetDaughter(i)->GetName())) {
			fPropertiesMap[vol->GetDaughter(i)] = ParseVolumeName(vol->GetDaughter(i)->GetName());
		}
		SetPropertiesRecursive(vol->GetDaughter(i)->GetLogicalVolume());
	}
    
}

void DetectorConstruction::Print() {
	if(fLogicWorld == nullptr) {
		std::cout<<"World volume does not exist yet!"<<std::endl;
		return;
	}

	SetProperties();

    PrintRecursive(fLogicWorld);
}

void DetectorConstruction::PrintRecursive(G4LogicalVolume* vol){
    
    for(unsigned int i = 0; i < (unsigned)vol->GetNoDaughters(); ++i) {
		std::cout<<i<<": "<<vol->GetDaughter(i)<<" - "<<vol->GetDaughter(i)->GetName();
		if(HasProperties(vol->GetDaughter(i))) {
			auto prop = GetProperties(vol->GetDaughter(i));
			std::cout<<" - "<<prop.detectorNumber<<", "<<prop.crystalNumber<<", "<<prop.systemID;
		}
		std::cout<<std::endl;
        PrintRecursive(vol->GetDaughter(i)->GetLogicalVolume());
	}
}

bool DetectorConstruction::CheckVolumeName(G4String volumeName) {
	if(volumeName.find("SiSegmentPhys") != G4String::npos) return true;
	if(volumeName.find("TrificGasCell") != G4String::npos) return true;
	return false;
}

DetectorProperties DetectorConstruction::ParseVolumeName(G4String volumeName) {
	DetectorProperties result;
    

	// Dice detectors also have the number in their name
	if(volumeName.find("SiSegmentPhys") != G4String::npos) {
		// strip "SiSegmentPhys_" (13 characters) and everything before from the string
		std::string tmpString = volumeName.substr(volumeName.find("SiSegmentPhys")+13);
		// replace all '_' with spaces so we can just use istringstream::operator>>
		std::replace(tmpString.begin(), tmpString.end(), '_', ' ');
		// create istringstream from the stripped and converted stream, and read detector and crystal number
		std::istringstream is(tmpString);
		is>>result.detectorNumber>>result.crystalNumber;
		result.systemID = 10;
		return result;
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void DetectorConstruction::AddApparatusDiceDetector(G4String Options){
	fAppDICE->Build(fLogicWorld,Options);
}

void DetectorConstruction::SetDiceFieldStrength(G4double Field){
	ApparatusDICE::fFieldStength=Field;
}

void DetectorConstruction::SetDiceLength(G4int Nsel,G4double Length){
    switch (Nsel) {
    case 1:
        fAppDICE->fAdjLengthA=Length;
		fAppDICE->fUseA=true;
        break;
    case 2:
        fAppDICE->fAdjLengthB=Length;
		fAppDICE->fUseB=true;
        break;
    case 3:
        fAppDICE->fAdjLengthC=Length;
		fAppDICE->fUseC=true;
        break;
    case 4:
        fAppDICE->fAdjLengthD=Length;
		fAppDICE->fUseD=true;
        break;
    case 5:
        fAppDICE->fAdjLengthE=Length;
		fAppDICE->fUseE=true;
        break;
    default:
        fAppDICE->fAdjLength=Length;
    }
    
}
void DetectorConstruction::SetDiceInt(G4int N){
	fAppDICE->fAdjNumber=N;
}
void DetectorConstruction::RemoveShield(G4bool input){
	fAppDICE->fRemoveShield=input;
}
void DetectorConstruction::AddBlocker(G4bool input){
	fAppDICE->fAddBlocker=input;
}
ApparatusDICE* DetectorConstruction::GetDice(){
	return fAppDICE;
}

