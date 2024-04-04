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
// $Id: DetectorConstruction.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef APPARATUSDICE_HH
#define APPARATUSDICE_HH

// Forward declarations of classes which arent explicitly included here for some geanty reason, but which will be linked by any library which uses this class
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

class ApparatusDICE
{
public:
	ApparatusDICE();
	~ApparatusDICE();
    
	void Build(G4LogicalVolume*,G4String);
    
	static G4double fFieldStength;

	///////////////////////////////////////////////////////
	////// Controlled by DetectorConstruction class ///////
	///////////////////////////////////////////////////////
	
    G4int fBuildOption;

	G4int    fAdjNumber;
	bool fRemoveShield;
	bool fAddBlocker;
	bool fUniformFieldOn;
	
    void SetParam(G4String,G4double);
	
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	
private:
 	G4LogicalVolume* fDetectorLog;
    
	G4String fMagnetMaterial; 
	G4String fYokeMaterial; 
	G4String fShieldMaterial; 
	G4String fBlockerMaterial; 
	G4String fWaferMaterial; 
	G4String fScintMaterial; 
	G4String fXrayMaterial; 
	G4String fKaptonMaterial; 
	G4String fTargetMaterial; 

    G4VisAttributes *OneVisAtt,*TwoVisAtt,*ThreeVisAtt,*FourVisAtt,*FiveVisAtt;
	G4VisAttributes* vis_att_hid;
    
	G4double 	fBB34PCB_HalfThickness;
	G4double 	fBB34PCB_Shelf;
	G4double 	fBB34PCB_ShelfWidth;
	G4double 	fBB34PCB_HalfWidth;
	G4double 	fBB34PCB_HalfALength;
	G4double 	fBB34PCB_Blength;
	G4double 	fBB34PCB_Angle;
	G4double 	fBB34PCB_Clength;

	G4double 	fBB34Chip_HalfThickness;
	G4double 	fBB34Chip_HalfWidth;
	G4double 	fBB34Chip_HalfLength;
	G4double 	fBB34Chip_GuardWidth;
	G4int 		fBB34Chip_Nseg;
	G4double 	fBB34Chip_Dead;
	G4double 	fBB34Chip_DeadAngle;
	G4double 	fBB34Chip_Al;
	
	G4double 	Orange_MagMidOffset;
	G4double 	Orange_BeamDetY;
	G4double 	Orange_ShieldMidBeamSep;
	G4double 	Orange_MagY;
	G4double 	Orange_MagAng;
	G4double Orange_BeamShieldSep;
	G4double Orange_MagZ;
	G4double Orange_MagHalfThick;
	G4double Orange_MagGapMinHalf;
	G4double PoleSepHalf;
	
	G4double PhiBlockThickness;
	
	std::vector<G4double*> SetPtrLst;
	std::vector<G4String> SetNmLst;
	
private:
	//////////////////////////////////////////////////////
	// internal methods and functions in ApparatusDICE::Build()
	//////////////////////////////////////////////////////

	// methods
	void BuildPlaceBasicTest(G4LogicalVolume*);
	
	void BuildPlaceTargetLadder(G4LogicalVolume*);
	
    G4LogicalVolume* BuildBB34();
    
    static G4int BuildMicronSiN;
    
	void BuildPlaceFlatOrange(G4LogicalVolume*,G4double);

	G4LogicalVolume* BuildISOK160Cross();
};
#endif
