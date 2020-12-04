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

class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

// define custom colours for visualisation
#define SN_COL 1.0, 1.0, 1.0
#define CU_COL 1.0, 1.0, 0.0
#define PEEK_COL 0.5, 0.5, 0.0
#define KAPTON_COL 0.2, 0.7, 0.1
#define PB_COL 0.6, 0.1, 0.1
#define NDFEB_COL 0.7,0.3,0.3
#define DELRIN_COL 0.0, 0.0, 1.0
#ifndef AL_COL
#define AL_COL 0.5, 0.5, 0.5
#endif

#define TARGET_CHAMBER_COL 0.15,0.15,0.15
#define ELECTROBOX_COL 0.15,0.15,0.15

class ApparatusDICE
{
public:
	ApparatusDICE();
	~ApparatusDICE();
    
	void Build(G4LogicalVolume*,G4String);
    
	static G4double fFieldStength;

private:
    G4LogicalVolume* fFieldVolume; 
 	G4LogicalVolume* fDetectorLog;
 	G4LogicalVolume* fMagnetLog;
	
	G4VPhysicalVolume* fDicePhys;
    
	G4String fMagnetMaterial; 
	G4String fBlockerMaterial; 
	G4String fWaferMaterial; 
	
	bool fFieldOn;
	
	/////

	G4double fMagnetHeight;
	G4double fMagnetWidth;
	G4double fMagnetDepth;
	G4double fMagnetThickness;
    
	G4double fTargetMagnetDistance;
    
	G4double fDetectorThickness;
	G4double fDetectorWidth;
	G4double fDetectorHeight;
	G4int fSegX;
	G4int fSegY;
	G4double fGuardRingWidth;
    
	G4double fDetPlaceAngle;
	G4double fDetPlaceRadius;
    
	//////
	
	G4double fHMagnetOuterR;
	G4double fHMagnetInnerR;
	G4double fHMagnetDepth;
	G4double fHMagTargetOffset;
	
	G4double fHTargDetOffset;
	G4double fHDetRad;
	G4double fHDetLength;
	G4int fHSegZ;
	
	////
	
	G4double fSTMagnetOuterR;
	G4double fSTMagnetInnerR;
	G4double fSTMagnetDepth;
	G4double fSTMagTargetOffset;
	
	G4double fSTDetOffset;
	G4double fSTDetInnerR;
	G4double fSTDetOuterR;
	G4double fSTDetLength;
	
	////

	G4double fRSOffset;
	G4double fRSBlock0;
	G4double fRSDetR;
	G4double fRSInnerR;

	
private:
	//////////////////////////////////////////////////////
	// internal methods and functions in ApparatusDICE::Build()
	//////////////////////////////////////////////////////

	// methods
	void BuildPlaceShoeBox(G4LogicalVolume*);
	void CreateShoeBoxField();
	void BuildShoeBoxYoke();
	void BuildRadialDetector();
	
	
	void BuildPlaceHelios(G4LogicalVolume*);
	void CreateHeliosField();
	void BuildHeliosYoke();
	void BuildHeliosDetector();
	
	void BuildPlaceSuperTube(G4LogicalVolume*);
	void CreateSuperTubeField();
    void BuildSuperTubeYoke();
	void BuildSuperTubeDetector();
	
	void BuildPlaceRecoilShadow(G4LogicalVolume*);
	void BuildRecoilShadowUnit();

};
#endif
