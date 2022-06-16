#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "PrimaryGeneratorMessenger.hh"//PGM

#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"

#include "G4Material.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4Trd.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"
#include "G4VSolid.hh"
#include "G4Trap.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <math.h>

#include "HistoManager.hh"
#include "ApparatusDICE.hh"

////////////////////////////////////////////////
// building and placing the different parts   //
// of the DICE system                         //
////////////////////////////////////////////////
#define SN_COL 1.0, 1.0, 1.0
#define CU_COL 1.0, 1.0, 0.0
#define PB_COL 0.6, 0.1, 0.1

G4double ApparatusDICE::fFieldStength = 0.065*CLHEP::tesla;	
//

ApparatusDICE::ApparatusDICE()//parameter chooses which lens is in place.
{
	// Materials
	fBlockerMaterial = "G4_C"; 
	fMagnetMaterial = "Iron"; 
	fWaferMaterial   = "Silicon";
    
	fFieldOn=true;
    fRemoveShield=false;
	
	// Shoe box measurments
	fMagnetHeight= 104*CLHEP::mm;
	fMagnetWidth= 300*CLHEP::mm;
	fMagnetDepth= 150*CLHEP::mm;
	fMagnetThickness= 10*CLHEP::mm;
	fMagnetLining= 0.5*CLHEP::mm;
	
	fTargetMagnetDistance= -1*CLHEP::mm;
	
    fDetectorThickness= 4*CLHEP::mm;
	fDetectorWidth= 100*CLHEP::mm;
	fDetectorHeight= 80*CLHEP::mm;
	fSegX= 20;
	fSegY= 8;
	fGuardRingWidth= 4*CLHEP::mm;
    
	fDetPlaceAngle1 = 85*CLHEP::deg;
	fDetPlaceAngle2 = 345*CLHEP::deg;
	fDetPlaceRadius= 40*CLHEP::mm;
	fDetPlaceRadius+=fDetectorWidth/2.;

	// Helios measurments
	fHMagnetOuterR= 90*CLHEP::mm;
	fHMagnetInnerR= 80*CLHEP::mm;
	fHMagnetDepth= 500*CLHEP::mm;
	fHMagTargetOffset= 0*CLHEP::mm;
	
	fHTargDetOffset= 45*CLHEP::mm;
	fHDetRad= 10*CLHEP::mm;
	fHDetRInner= 5*CLHEP::mm;
	fHDetLength= 300*CLHEP::mm;
	fHSegZ=150;

	
    // SuperTube
	fSTMagnetOuterR= 20*CLHEP::mm;
	fSTMagnetInnerR= 15*CLHEP::mm;
	fSTMagnetDepth= 300*CLHEP::mm;
	fSTMagTargetOffset= 10*CLHEP::mm;
	
	fSTDetOffset= 30*CLHEP::mm;
	fSTDetInnerR= 20*CLHEP::mm;
	fSTDetOuterR= 25*CLHEP::mm;
	fSTDetLength= 150*CLHEP::mm;

    // Recoil Shadow
	fRSOffset= 10*CLHEP::mm;
	fRSBlock0= 3*CLHEP::mm;
	fRSBlock1= 2*CLHEP::mm;
	fRSDetR= 15*CLHEP::mm;
	fRSInnerR= 6*CLHEP::mm;
    
	fRSShieldRIn= 3*CLHEP::mm;
	fRSDetDepth= 4*CLHEP::mm;
    fRSSegZ=10;
	
    
    // Shield Common
    
    fShieldOffset = 2.*CLHEP::mm;
    OneVisAtt = new G4VisAttributes(G4Colour(PB_COL));
    TwoVisAtt = new G4VisAttributes(G4Colour(SN_COL));
    ThreeVisAtt = new G4VisAttributes(G4Colour(CU_COL));

    fPhotonShieldLayerOneMaterial = "WTa";
    fPhotonShieldLayerTwoMaterial = "Tin";
    fPhotonShieldLayerThreeMaterial = "Copper";
    
    fPhotonShieldLength = 30.*CLHEP::mm;
    fPhotonShieldLayerOneThickness = 25*CLHEP::mm;
    fPhotonShieldLayerTwoThickness = 4*CLHEP::mm;
    fPhotonShieldLayerThreeThickness = fPhotonShieldLength-fPhotonShieldLayerOneThickness-fPhotonShieldLayerTwoThickness;
        
    // RMS recoilshaddow measurments
	fRMSGuard= 3*CLHEP::mm;
	fRMSDetHalfLength = 40*CLHEP::mm+fRMSGuard;
	fRMSDetHalfWidth= 20*CLHEP::mm+fRMSGuard;
	fRMSDetHalfDepth= 1*CLHEP::mm;
    fRMSSegZ=16;

    
    
	///////////////////////////////////////////////////////
	////// Controlled by DetectorConstruction class ///////
	///////////////////////////////////////////////////////
	
	fAdjLength= 10*CLHEP::mm;
	fAdjNumber=4;
	
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	
} // end ApparatusDICE constructor

//////////////////////////////////////////////////
// delete ApparatusDICE
/////////////////////////////////////////////////

ApparatusDICE::~ApparatusDICE()
{
} // end ~ApparatusDICE

///////////////////////////////////////
// build and place components        //
// ///////////////////////////////////////

// Main build function called by DetectorConstruction.cc
void ApparatusDICE::Build(G4LogicalVolume* expHallLog,G4String Options)
{	
	std::stringstream ss;
	ss<<Options;
	int Opt;
	ss>>Opt;

	switch(Opt) {
		case 1: BuildPlaceShoeBox(expHallLog); break;
		case 2: BuildPlaceHelios(expHallLog); break;
		case 3: BuildPlaceSuperTube(expHallLog); break;
		case 4: BuildPlaceRecoilShadow(expHallLog); break;
		case 5: BuildPlaceCyclone(expHallLog); break;
		case 6: BuildPlacePhotonShieldTest(expHallLog); break;
		case 7: BuildPlaceNewRecoilShaddow(expHallLog); break;
		case 8: BuildPlaceRMSFocal(expHallLog); break;
        
        
		default: BuildPlaceBasicTest(expHallLog);
	}
	
// 	BuildPlaceHPGe(expHallLog);
} 

////////////////////////////////////////////////////
// methods used to build and place the components:
////////////////////////////////////////////////////


void ApparatusDICE::BuildPlaceBasicTest(G4LogicalVolume* expHallLog){
	
	// Define a G4Material to load and store the information about a certain material that we will assign to an object later
	// In this case we load properties for variable fWaferMaterial, which was set at the initilisation of the ApparatusDICE class
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
	
	// Create a G4Box, a "Solid" type object, defines geometry but does not have material properies or exist inside this simulated world. Only a geometric blueprint.
	// Inputs: name, 3 half lengths of the cuboid 
	G4Box* fDetectorSolid = new G4Box("fDetectorSolid", 15*mm, 15*mm, 2*mm);

	// Create a G4LogicalVolume a "logical", combines a "solid" with material properies and other options (not used here) describing how it should be treated during the simulation. Still only a blueprint, does not exist in the simulated world
	// Inputs : Solid (pointer), Material, name, 3 options not used here (0,0,0)
	G4LogicalVolume* fDetectorLogical = new G4LogicalVolume(fDetectorSolid, material, "fDetectorLogical", 0, 0, 0);
	
	// Create a G4PVPlacement, a type of "physical" which means placing a copy of our logical blueprints inside another logical volume. When a logical volume is placed inside the "world volume" that volume (and any contained within it) exist in the simulated world
	// Note : The actual instance of the object is not used further in this function, we only need it to be created. It now exists in memory and at the moment of creation information was sent to it's "mother volume".
	// Inputs : Rotation (pointer), Rotation (local), Material (pointer), name, mothervolume, 2 options not used here (false,0)
	// Mother volume is the "logical" within which this physical should occur, for instance we can but segments inside a detector
	// All physicals with name "SiSegmentPhys#_#" where # are number will be treated as detecors for the purposes of output, where the numbers # are used to specify the volumes
	// The exact definition of the special names can befound in DetectorConstruction::ParseVolumeName(G4String volumeName) of DetectorConstruction.cc
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(0,0,02*mm), fDetectorLogical, "SiSegmentPhys0_0",  expHallLog, false, 0);
	
}


void ApparatusDICE::BuildPlaceShoeBox(G4LogicalVolume* expHallLog){
	
	//// fFieldVolume is the logical volume over which the field is defined
	//// The magnet yoke and detector are placed in fFieldVolume
	//// fFieldVolume is the placed in world volume
    
    G4Box* fFieldBox = new G4Box("FieldBox", 0.1*mm+ fMagnetWidth/2.,0.1*mm+ fMagnetHeight/2.,0.1*mm+ fMagnetDepth/2.);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	fFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", 0, 0, 0);
    fFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 
	
    if(fFieldOn)CreateShoeBoxField(); // Add field to fFieldVolume
	
	BuildShoeBoxYoke(); // Build fMagnetLog
    new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(),fMagnetLog,"MagnetPhys",fFieldVolume,false,false,0); // Place fMagnetLog centrally in fFieldVolume
	
	BuildRadialDetector(fDetPlaceAngle1); // Build fDetectorLog
	BuildRadialDetector(fDetPlaceAngle2); // Build fDetectorLog
	
	// Finally place fFieldVolume in expHallLog

	G4ThreeVector movef(0, 0, fTargetMagnetDistance+fMagnetDepth/2.);

    // Do we even need to store the phys?
	fDicePhys = new G4PVPlacement(new G4RotationMatrix(),                       //rotation
			movef,                    //at position
			fFieldVolume,             //its logical volume
			"MagnetFieldPhys",             //its name
			expHallLog,         //its mother  volume
			false,        //no boolean operation
			false,       //copy number
			0);          //overlaps checking

}

/// The logical volume fFieldVolume already exists, this just adds a specific field in that box
void ApparatusDICE::CreateShoeBoxField(){
	
    G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(0.,fFieldStength,0.));
    G4FieldManager* localFieldManager=new G4FieldManager(magField);
    localFieldManager->CreateChordFinder(magField);
    fFieldVolume->SetFieldManager(localFieldManager, true);
    
    // Global
//    G4FieldManager* fieldMgr= G4TransportationManager::GetTransportationManager()->GetFieldManager();
//    fieldMgr->SetDetectorField(magField);
//    fieldMgr->CreateChordFinder(magField);
}


void ApparatusDICE::BuildShoeBoxYoke() {
	
	// Visualisation
	G4VisAttributes* sVisAtt = new G4VisAttributes(G4Colour(0.0, 0.9, 0.1));
	sVisAtt->SetVisibility(true);
    
	G4Box* MagnetYoke = new G4Box("BoxM", fMagnetWidth/2., fMagnetHeight/2., fMagnetDepth/2.);
	G4Box* MagnetHole = new G4Box("CutBoxH", fMagnetWidth/2. -fMagnetThickness, fMagnetHeight/2. -fMagnetThickness, 0.1*mm+fMagnetDepth/2.);
	G4VSolid* MagFull = new G4SubtractionSolid("Magnet", MagnetYoke, MagnetHole,0,G4ThreeVector(0,0,0));
    
	// Logical
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	fMagnetLog = new G4LogicalVolume(MagFull, mMaterial, "MagnetLog", 0, 0, 0);
	fMagnetLog->SetVisAttributes(sVisAtt);
	
    G4Box* MagnetLining = new G4Box("CutBoxH", fMagnetWidth/2. -fMagnetThickness-fMagnetLining, fMagnetHeight/2. -fMagnetThickness-fMagnetLining, 0.2*mm+fMagnetDepth/2.);
    G4VSolid* MagLin = new G4SubtractionSolid("Magnet", MagnetHole, MagnetLining,0,G4ThreeVector(0,0,0));

    mMaterial = G4Material::GetMaterial(fBlockerMaterial);
	G4LogicalVolume* fMagnetLinLog = new G4LogicalVolume(MagLin, mMaterial, "MagnetLinLog", 0, 0, 0);
	fMagnetLinLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.9, 0.7, 0.4)));

    new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(0,0,0),fMagnetLinLog,"MagnetLining",fMagnetLog,false,false,0); 
    
}

G4int ApparatusDICE::ShoeDetN=0;

void ApparatusDICE::BuildRadialDetector(G4double PlaceAngle){
	G4RotationMatrix* rotate = new G4RotationMatrix;

	// Detector crystal mother
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);
    G4Box* siDetMother = new G4Box("siDetMother", 0.1*mm +fDetPlaceRadius, 0.1*mm+fDetectorHeight/2., 0.1*mm+fDetectorThickness/2.);
	fDetectorLog = new G4LogicalVolume(siDetMother, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);

    
	// Silicon G4VisAttributes
	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);

	// Place Guard Ring in fDetectorLog
	
    G4Box* siGuardOuter = new G4Box("siGuardOuter", (fDetectorWidth)/2. -0.1*mm, (fDetectorHeight)/2. -0.1*mm, fDetectorThickness/2. -0.1*mm);
	G4double halfwidth=fDetectorWidth/2. -fGuardRingWidth;
	G4double halfheight=fDetectorHeight/2. -fGuardRingWidth;
    G4Box* siGuardInner = new G4Box("siGuardOuter", halfwidth +0.1*mm, halfheight +0.1*mm, fDetectorThickness/2.);
	G4VSolid* siGuard = new G4SubtractionSolid("siGuard", siGuardOuter, siGuardInner,0,G4ThreeVector(0,0,0));
    G4LogicalVolume* SiGuardLog =new G4LogicalVolume(siGuard, material, "SiGuardLog", 0,0,0);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<ShoeDetN;
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0), SiGuardLog,tt.str(),  fDetectorLog, false, 0);
	
	// Place Detector Elements in fDetectorLog  
    
    G4Box* siDetSegment = new G4Box("siSeg", halfwidth/fSegX -0.01*mm, halfheight/fSegY -0.01*mm, fDetectorThickness/2. -0.1*mm);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	for(int x=0;x<fSegX;x++){
        G4double xx=-halfwidth+(halfwidth*2/fSegX)*(0.5+x);
        
        for(int y=0;y<fSegY;y++){
            G4double yy=-halfheight +(halfheight*2/fSegY)*(0.5+y);
            std::stringstream temp;
			temp<<"SiSegmentPhys_"<<1+x*fSegY+y<<"_"<<ShoeDetN;
			new G4PVPlacement(rotate,G4ThreeVector(xx,yy,0), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);
		}	
	}
	
	ShoeDetN++;
		
	// Place Shield in fDetectorLog
    
    G4double ShieldAngle=atan((fDetectorThickness*.5)/(fDetPlaceRadius-(0.5*fDetectorWidth)));
    G4double ShieldOneZ=(fDetPlaceRadius-0.5*fDetectorWidth)-fPhotonShieldLength+fPhotonShieldLayerOneThickness*.5;
    G4double ShieldTwoZ=ShieldOneZ+(fPhotonShieldLayerOneThickness+fPhotonShieldLayerTwoThickness)*.5;
    G4double ShieldThreeZ=ShieldTwoZ+(fPhotonShieldLayerTwoThickness+fPhotonShieldLayerThreeThickness)*.5;
    
    G4Box* ShieldBoxOne = new G4Box("WaTaBox", fDetectorWidth/2., fDetectorHeight/2., fPhotonShieldLayerOneThickness*0.5-0.1*mm);
    G4Box* ShieldBoxTwo = new G4Box("TinBox", fDetectorWidth/2., fDetectorHeight/2., fPhotonShieldLayerTwoThickness*0.5-0.1*mm -0.01*mm);
    G4Box* ShieldBoxThree = new G4Box("CopperBox", fDetectorWidth/2., fDetectorHeight/2., fPhotonShieldLayerThreeThickness*0.5-0.1*mm);
    G4Box* CutBox = new G4Box("CutBox", fDetectorWidth, fDetectorHeight, fPhotonShieldLength);
    
    rotate = new G4RotationMatrix;
    rotate->rotateY(-ShieldAngle);
    
	G4SubtractionSolid* ShieldSolidOne = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxOne,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldOneZ+fDetectorWidth/cos(ShieldAngle),0,0));
 	G4SubtractionSolid* ShieldSolidTwo = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxTwo,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldTwoZ+fDetectorWidth/cos(ShieldAngle),0,0));
    G4SubtractionSolid* ShieldSolidThree = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxThree,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldThreeZ+fDetectorWidth/cos(ShieldAngle),0,0));
    
    rotate = new G4RotationMatrix;
    rotate->rotateY(ShieldAngle);
    
	ShieldSolidOne = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidOne,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldOneZ+fDetectorWidth/cos(ShieldAngle)),0,0));
 	ShieldSolidTwo = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidTwo,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldTwoZ+fDetectorWidth/cos(ShieldAngle)),0,0));
    ShieldSolidThree = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidThree,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldThreeZ+fDetectorWidth/cos(ShieldAngle)),0,0));   
    
    
    G4Material* shieldmat = G4Material::GetMaterial(fPhotonShieldLayerOneMaterial);
	G4LogicalVolume *ShieldOne = new G4LogicalVolume(ShieldSolidOne, shieldmat,"PhotonShieldLayerOneLog",0,0,0);
	ShieldOne->SetVisAttributes(OneVisAtt);
    
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerTwoMaterial);
	G4LogicalVolume *ShieldTwo = new G4LogicalVolume(ShieldSolidTwo, shieldmat,"PhotonShieldLayerTwoLog",0,0,0);
	ShieldTwo->SetVisAttributes(TwoVisAtt);
    
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerThreeMaterial);
	G4LogicalVolume *ShieldThree = new G4LogicalVolume(ShieldSolidThree, shieldmat,"PhotonShieldLayerThreeLog",0,0,0);
	ShieldThree->SetVisAttributes(ThreeVisAtt);
    
	rotate = new G4RotationMatrix;
    rotate->rotateY(-90*CLHEP::deg); 
    
    if(!fRemoveShield){
        new G4PVPlacement(rotate,G4ThreeVector(-(fDetPlaceRadius-ShieldOneZ+fShieldOffset),0,0), ShieldOne,"ShieldOne", fDetectorLog,false,0);
        new G4PVPlacement(rotate,G4ThreeVector(-(fDetPlaceRadius-ShieldTwoZ+fShieldOffset),0,0), ShieldTwo,"ShieldTwo", fDetectorLog,false,0);
        new G4PVPlacement(rotate,G4ThreeVector(-(fDetPlaceRadius-ShieldThreeZ+fShieldOffset),0,0), ShieldThree,"ShieldThree", fDetectorLog,false,0);
    }
    
    // Place fDetectorLog in fFieldVolume
    
    G4double rotY=90*CLHEP::deg-PlaceAngle;
    G4double Xplace=sin(PlaceAngle)*fDetPlaceRadius;
    G4double Zplace=cos(PlaceAngle)*fDetPlaceRadius-(fTargetMagnetDistance+fMagnetDepth/2.);
    
//     Xplace+=cos(PlaceAngle)*(fDetectorThickness*.5);
//     Zplace+=-sin(PlaceAngle)*(fDetectorThickness*.5);
	
	G4ThreeVector move(Xplace, 0, Zplace);   
	rotate = new G4RotationMatrix;
    rotate->rotateY(rotY); 
    
	new G4PVPlacement(rotate,move, fDetectorLog, "SiLiDetActive",  fFieldVolume, true, 0);
}


void ApparatusDICE::BuildPlaceHelios(G4LogicalVolume* expHallLog){
	
	//// fFieldVolume is the logical volume over which the field is defined
	//// The magnet yoke and detector are placed in fFieldVolume
	//// fFieldVolume is the placed in world volume
    
    G4Tubs* fFieldBox = new G4Tubs("FieldBox", 0,0.1*mm+fHMagnetOuterR,0.1*mm+ fHMagnetDepth/2.,0 , 360*CLHEP::deg);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	fFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", 0, 0, 0);
    fFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 

    if(fFieldOn)CreateHeliosField(); // Add field to fFieldVolume
	
	BuildHeliosYoke(); // Build fMagnetLog
    new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(),fMagnetLog,"MagnetPhys",fFieldVolume,false,false,0); // Place fMagnetLog centrally in fFieldVolume
	
	BuildHeliosDetector(); // Build fDetectorLog

    // Place fDetectorLog in fFieldVolume
    G4double Zplace=(fHTargDetOffset+fHDetLength/2)-(fHMagTargetOffset+fHMagnetDepth/2);
	G4ThreeVector move(0, 0, Zplace);       
	new G4PVPlacement(new G4RotationMatrix(),move, fDetectorLog, "SiLiDetActive",  fFieldVolume, false, 0);	

    // Add Photon Shield into the volume
    BuildPlaceHeliosPhotonShield(fFieldVolume);
    
	// Finally place fFieldVolume in expHallLog
	
	G4ThreeVector movef(0, 0, fHMagTargetOffset+fHMagnetDepth/2.);

    // Do we even need to store the phys?
	fDicePhys = new G4PVPlacement(new G4RotationMatrix(),                       //rotation
			movef,                    //at position
			fFieldVolume,             //its logical volume
			"MagnetFieldPhys",             //its name
			expHallLog,         //its mother  volume
			false,        //no boolean operation
			false,       //copy number
			0);          //overlaps checking

}

/// The logical volume fFieldVolume already exists, this just adds a specific field in that box
void ApparatusDICE::CreateHeliosField(){
	
    G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(0.,0.,fFieldStength));
    G4FieldManager* localFieldManager=new G4FieldManager(magField);
    localFieldManager->CreateChordFinder(magField);
    fFieldVolume->SetFieldManager(localFieldManager, true);
}

void ApparatusDICE::BuildHeliosYoke() {
	
	// Visualisation
	G4VisAttributes* sVisAtt = new G4VisAttributes(G4Colour(0.0, 0.9, 0.1));
// 	sVisAtt->SetVisibility(false);
    
	G4Tubs* Magnet = new G4Tubs("Magnet", fHMagnetInnerR,fHMagnetOuterR,fHMagnetDepth/2.0,0,360*CLHEP::deg);

	// Logical
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	fMagnetLog = new G4LogicalVolume(Magnet, mMaterial, "MagnetLog", 0, 0, 0);
	fMagnetLog->SetVisAttributes(sVisAtt);
}

void ApparatusDICE::BuildHeliosDetector(){
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);

	// Define rotation and movement objects
	G4RotationMatrix* rotate = new G4RotationMatrix;
	

	// Detector crystal mother
    G4Tubs* siDetMother = new G4Tubs("siDetMother", 0,0.1*mm+fHDetRad,fHDetLength/2.0 , 0,360*CLHEP::deg);
	fDetectorLog = new G4LogicalVolume(siDetMother, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);
	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true); 
    
    G4Tubs* TmpCore = new G4Tubs("TmpCore", 0,fHDetRInner-0.1*mm,fHDetLength*0.5 -0.1*mm, 0,360*CLHEP::deg);
    G4LogicalVolume* TmpCoreLog = new G4LogicalVolume(TmpCore, G4Material::GetMaterial("WTa"), "TmpCoreLog", 0,0,0);  
    new G4PVPlacement(rotate,G4ThreeVector(0,0,0), TmpCoreLog, "TCore",  fDetectorLog, false, 0);	
    
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
    G4Tubs* siDetSegment = new G4Tubs("siSeg",fHDetRInner, fHDetRad, fHDetLength/(fHSegZ*2.) -0.1*mm, 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	G4double zz=fHDetLength/(fHSegZ*2.)-fHDetLength/2.;
	for(int x=0;x<fHSegZ;x++){
		std::stringstream temp;
		temp<<"SiSegmentPhys_"<<x<<"_"<<0;
		new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);	
		zz+=fHDetLength/fHSegZ;
	}
}

void ApparatusDICE::BuildPlaceHeliosPhotonShield(G4LogicalVolume* FieldVolume){
        
    if(fRemoveShield) return;
    
//     G4Cons* PhotonShield = new G4Cons("PhotonShield", InnerRadius, BackOuterRadius, InnerRadius, FrontOuterRadius,	HalfLength, 0, 2*CLHEP::pi);
    
    G4double Zplace=fHTargDetOffset-(fHMagTargetOffset+fHMagnetDepth/2); 
    G4double ShieldOneZ=Zplace-fPhotonShieldLength+fPhotonShieldLayerOneThickness*.5;
    G4double ShieldTwoZ=ShieldOneZ+(fPhotonShieldLayerOneThickness+fPhotonShieldLayerTwoThickness)*.5;
    G4double ShieldThreeZ=ShieldTwoZ+(fPhotonShieldLayerTwoThickness+fPhotonShieldLayerThreeThickness)*.5;
    
    G4double ShieldATan=fHDetRad/fHTargDetOffset;
    G4double R1=ShieldATan*(fHTargDetOffset-fPhotonShieldLength);
    G4double R2=ShieldATan*(fHTargDetOffset-fPhotonShieldLength+fPhotonShieldLayerOneThickness);
    G4Cons* ShieldBoxOne = new G4Cons("WaTaCone", 0, R1, 
			0, R2,fPhotonShieldLayerOneThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);
    R1=R2;
    R2=ShieldATan*(fHTargDetOffset-fPhotonShieldLayerThreeThickness);
    G4Cons* ShieldBoxTwo = new G4Cons("TinBox", 0, R1, 
			0, R2,fPhotonShieldLayerTwoThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);
    
    R1=R2;
    R2=ShieldATan*fHTargDetOffset;
    G4Cons* ShieldBoxThree = new G4Cons("CopperBox", 0, R1, 
			0, R2,fPhotonShieldLayerThreeThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);

    
    G4Material* shieldmat = G4Material::GetMaterial(fPhotonShieldLayerOneMaterial);
	G4LogicalVolume *ShieldOne = new G4LogicalVolume(ShieldBoxOne, shieldmat,"PhotonShieldLayerOneLog",0,0,0);
	ShieldOne->SetVisAttributes(OneVisAtt);
    
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerTwoMaterial);
	G4LogicalVolume *ShieldTwo = new G4LogicalVolume(ShieldBoxTwo, shieldmat,"PhotonShieldLayerTwoLog",0,0,0);
	ShieldTwo->SetVisAttributes(TwoVisAtt);
//     
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerThreeMaterial);
	G4LogicalVolume *ShieldThree = new G4LogicalVolume(ShieldBoxThree, shieldmat,"PhotonShieldLayerThreeLog",0,0,0);
	ShieldThree->SetVisAttributes(ThreeVisAtt);
    
    G4RotationMatrix* rotate = new G4RotationMatrix;
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldOneZ-fShieldOffset), ShieldOne,"ShieldOne", FieldVolume,false,0);
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldTwoZ-fShieldOffset), ShieldTwo,"ShieldTwo", FieldVolume,false,0);
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldThreeZ-fShieldOffset), ShieldThree,"ShieldThree", FieldVolume,false,0);
}


void ApparatusDICE::BuildPlaceSuperTube(G4LogicalVolume* expHallLog){
		//// fFieldVolume is the logical volume over which the field is defined
	//// The magnet yoke and detector are placed in fFieldVolume
	//// fFieldVolume is the placed in world volume
    
    G4Tubs* fFieldBox = new G4Tubs("FieldBox", 0,0.1*mm+fSTMagnetOuterR,fSTMagnetDepth,0 , 360*CLHEP::deg);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	fFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", 0, 0, 0);
//     fFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 

    if(fFieldOn)CreateSuperTubeField(); // Add field to fFieldVolume
	
	BuildSuperTubeYoke(); // Build fMagnetLog
    new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(),fMagnetLog,"MagnetPhys",fFieldVolume,false,false,0); // Place fMagnetLog centrally in fFieldVolume
	
	BuildSuperTubeDetector(); // Build fDetectorLog
// 
//     // Place fDetectorLog in fFieldVolume
    G4double Zplace=(fSTMagnetDepth+fSTDetLength)*0.5+fSTDetOffset;
	G4ThreeVector move(0, 0, Zplace);       
	new G4PVPlacement(new G4RotationMatrix(),move, fDetectorLog, "SiLiDetActive",  fFieldVolume, false, 0);	

	// Finally place fFieldVolume in expHallLog
	
	G4ThreeVector movef(0, 0, fSTMagTargetOffset+fSTMagnetDepth/2.);

    // Do we even need to store the phys?
	fDicePhys = new G4PVPlacement(new G4RotationMatrix(),                       //rotation
			movef,                    //at position
			fFieldVolume,             //its logical volume
			"MagnetFieldPhys",             //its name
			expHallLog,         //its mother  volume
			false,        //no boolean operation
			false,       //copy number
			0);          //overlaps checking
	
}

/// The logical volume fFieldVolume already exists, this just adds a specific field in that box
void ApparatusDICE::CreateSuperTubeField(){
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");

	G4Tubs* fFieldCore = new G4Tubs("FieldBoxCore", 0,-0.1*mm+fSTMagnetInnerR,5*mm+fSTMagnetDepth/2.0,0 , 360*CLHEP::deg);
	G4LogicalVolume*	fFieldCoreLog = new G4LogicalVolume(fFieldCore, matWorld, "FieldCoreLog", 0, 0, 0);
	fFieldCoreLog->SetVisAttributes(G4VisAttributes::Invisible); 

	G4Tubs* fFieldEnd = new G4Tubs("FieldBoxEnd", 0,-0.1*mm+fSTDetInnerR,-5.01*mm+fSTMagnetDepth/4.0,0 , 360*CLHEP::deg);
	G4LogicalVolume* fFieldEndLog = new G4LogicalVolume(fFieldEnd, matWorld, "FieldEndLog", 0, 0, 0);
	fFieldEndLog->SetVisAttributes(G4VisAttributes::Invisible); 

    G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(0.,0.,fFieldStength*10));
    G4FieldManager* localFieldManager=new G4FieldManager(magField);
    localFieldManager->CreateChordFinder(magField);
    fFieldCoreLog->SetFieldManager(localFieldManager, true);
	
	
    G4UniformMagField* magFieldSmall = new G4UniformMagField(G4ThreeVector(0.,0.,fFieldStength*2));
    G4FieldManager* localFieldManagerSmall=new G4FieldManager(magFieldSmall);
    localFieldManagerSmall->CreateChordFinder(magFieldSmall);
    fFieldEndLog->SetFieldManager(localFieldManagerSmall, true);	
	
	
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(),fFieldCoreLog,"MagnetFieldCore",fFieldVolume,false,false,0);
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(0,0,fSTMagnetDepth*.75),fFieldEndLog,"MagnetFieldEnd1",fFieldVolume,false,false,0);
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(0,0,-fSTMagnetDepth*.75),fFieldEndLog,"MagnetFieldEnd2",fFieldVolume,false,false,0);
}

void ApparatusDICE::BuildSuperTubeYoke() {
	
	// Visualisation
	G4VisAttributes* sVisAtt = new G4VisAttributes(G4Colour(0.0, 0.9, 0.1));
	sVisAtt->SetVisibility(true);
    
	G4Tubs* Magnet = new G4Tubs("Magnet", fSTMagnetInnerR,fSTMagnetOuterR,fSTMagnetDepth/2.0,0,360*CLHEP::deg);

	// Logical
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	fMagnetLog = new G4LogicalVolume(Magnet, mMaterial, "MagnetLog", 0, 0, 0);
	fMagnetLog->SetVisAttributes(sVisAtt);
}


void ApparatusDICE::BuildSuperTubeDetector(){
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);

	// Define rotation and movement objects
	G4RotationMatrix* rotate = new G4RotationMatrix;
	
	// Detector crystal mother
    G4Tubs* siDetMother = new G4Tubs("siDetMother", 0,0.1*mm+fSTDetOuterR,fSTDetLength/2.0 , 0,360*CLHEP::deg);
	fDetectorLog = new G4LogicalVolume(siDetMother, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);

	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
	
    G4Tubs* siDetSegment = new G4Tubs("siSeg",fSTDetInnerR, fSTDetOuterR, fSTDetLength/(fHSegZ*2.) -0.1*mm, 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
	fSiSegmentLog->SetVisAttributes(vis_att);

  
	G4double zz=fSTDetLength/(fHSegZ*2.)-fSTDetLength/2.;
	for(int x=0;x<fHSegZ;x++){
		std::stringstream temp;
		temp<<"SiSegmentPhys_"<<x<<"_"<<0;
		new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);	
		zz+=fSTDetLength/fHSegZ;
	}
}

void ApparatusDICE::BuildPlaceRecoilShadow(G4LogicalVolume* expHallLog){

    G4double EffectiveRad=sqrt(2)*(fRSDetDepth+fRSDetR);
    G4double ShieldATan=EffectiveRad/(fPhotonShieldLength+fRSOffset);
    G4double ShieldOneZ=fRSOffset+fPhotonShieldLayerOneThickness*.5;
    G4double ShieldTwoZ=ShieldOneZ+(fPhotonShieldLayerOneThickness+fPhotonShieldLayerTwoThickness)*.5;
    G4double ShieldThreeZ=ShieldTwoZ+(fPhotonShieldLayerTwoThickness+fPhotonShieldLayerThreeThickness)*.5;
    G4double R1=ShieldATan*fRSOffset;
    G4double R2=ShieldATan*(fRSOffset+fPhotonShieldLayerOneThickness);
    G4Cons* ShieldBoxOne = new G4Cons("WaTaCone", fRSShieldRIn, R1, 
            fRSShieldRIn, R2,fPhotonShieldLayerOneThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);
    R1=R2;
    R2=ShieldATan*(fRSOffset+fPhotonShieldLength-fPhotonShieldLayerThreeThickness);
    G4Cons* ShieldBoxTwo = new G4Cons("TinBox", fRSShieldRIn, R1, 
            fRSShieldRIn, R2,fPhotonShieldLayerTwoThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);
    R1=R2;
    R2=ShieldATan*(fRSOffset+fPhotonShieldLength);
    G4Cons* ShieldBoxThree = new G4Cons("CopperBox", fRSShieldRIn, R1, 
            fRSShieldRIn, R2,fPhotonShieldLayerThreeThickness*0.5-0.1*mm, 0, 2*CLHEP::pi);


    G4Material* shieldmat = G4Material::GetMaterial(fPhotonShieldLayerOneMaterial);
    G4LogicalVolume *ShieldOne = new G4LogicalVolume(ShieldBoxOne, shieldmat,"PhotonShieldLayerOneLog",0,0,0);
    ShieldOne->SetVisAttributes(OneVisAtt);

    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerTwoMaterial);
    G4LogicalVolume *ShieldTwo = new G4LogicalVolume(ShieldBoxTwo, shieldmat,"PhotonShieldLayerTwoLog",0,0,0);
    ShieldTwo->SetVisAttributes(TwoVisAtt);
//     
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerThreeMaterial);
    G4LogicalVolume *ShieldThree = new G4LogicalVolume(ShieldBoxThree, shieldmat,"PhotonShieldLayerThreeLog",0,0,0);
    ShieldThree->SetVisAttributes(ThreeVisAtt);

    G4RotationMatrix* rotate = new G4RotationMatrix;
    if(!fRemoveShield){
        new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldOneZ-fShieldOffset), ShieldOne,"ShieldOne", expHallLog,false,0);
        new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldTwoZ-fShieldOffset), ShieldTwo,"ShieldTwo", expHallLog,false,0);
        new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldThreeZ-fShieldOffset), ShieldThree,"ShieldThree", expHallLog,false,0);
    }
        
        
	G4Material* matblock = G4Material::GetMaterial(fBlockerMaterial);
	G4Tubs* blocktub = new G4Tubs("blocktub",fRSInnerR,fRSDetR, fRSBlock0*.5, 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSubBlock =new G4LogicalVolume(blocktub, matblock, "BlockRingLog", 0,0,0);
    
    int Nblock=floor((fRSDetR*2)/(fRSBlock0+fRSBlock1));
    double StepBlock=(fRSDetR*2)/Nblock;
        
    G4double zz=fRSOffset+fPhotonShieldLength+fRSDetR*2;
    for(int x=0;x<fRSSegZ;x++){
        double Z=zz+x*2*fRSDetR-fRSBlock0*.5;
        
        for(int i=0;i<Nblock;i++){
            new G4PVPlacement(rotate,G4ThreeVector(0,0,Z), fSubBlock, "BlockerRing",  expHallLog, false, 0);
            Z-=StepBlock;	            
        }
    }	
    
    
	
    G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);

    G4Tubs* siDetSegment = new G4Tubs("siSeg",0, fRSDetR,fRSDetDepth/2., 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);	
    fSiSegmentLog->SetVisAttributes(vis_att);    
    
    double Z=zz-fRSDetR;
    for(int x=0;x<fRSSegZ;x++){
        
        for(int i=0;i<4;i++){
            std::stringstream temp;
            temp<<"SiSegmentPhys_"<<x<<"_"<<i;
            
            G4ThreeVector place=G4ThreeVector(fRSDetR+fRSDetDepth*.5,0,Z);
            place.rotateZ(i*90*CLHEP::deg);
            G4RotationMatrix* r=new G4RotationMatrix;
            r->rotateY(90*CLHEP::deg); 
            r->rotateX(i*90*CLHEP::deg); 
            new G4PVPlacement(r,place, fSiSegmentLog,temp.str(),  expHallLog, false, 0);
        }
        
         Z+=fRSDetR*2;
    }	   
    

}



void ApparatusDICE::BuildPlaceCyclone(G4LogicalVolume* expHallLog){
	
	//// fFieldVolume is the logical volume over which the field is defined
	//// The magnet yoke and detector are placed in fFieldVolume
	//// fFieldVolume is the placed in world volume
    
    G4Tubs* fFieldBox = new G4Tubs("FieldBox", 0,0.1*mm+fHMagnetOuterR,0.1*mm+ fHMagnetDepth/2.,0 , 360*CLHEP::deg);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	fFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", 0, 0, 0);
    fFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 

    if(fFieldOn)CreateCycloneField(); // Add field to fFieldVolume
	
	BuildCycloneYoke(); // Build fMagnetLog
	
    new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(),fMagnetLog,"MagnetPhys",fFieldVolume,false,false,0); // Place fMagnetLog centrally in fFieldVolume

    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
	
	for(int i=0;i<6;i++){
// 		
		G4Tubs* fDetSolid = new G4Tubs("FieldBox", 45-i*5,50-i*5,2.5,0 , (60-i*5)*CLHEP::deg);
		G4LogicalVolume* fDetectorLogical = new G4LogicalVolume(fDetSolid, material, "fDetectorLogical", 0, 0, 0);
// 		
		for(int j=0;j<i+3;j++){
			G4RotationMatrix* rot=new G4RotationMatrix();
			rot->rotateZ(j*(360/(i+3))*CLHEP::deg);
			rot->rotateX(30*CLHEP::deg);
			G4ThreeVector movef(0, 0, i*50-fSTMagnetDepth/2.+2.5);
			std::stringstream ss;
			ss<<"SiSegmentPhys"<<i<<"_"<<j;
			new G4PVPlacement(rot,movef, fDetectorLogical,ss.str(),  fFieldVolume, false, 0);
		}
	}
	
	
// 	for(int i=0;i<6;i++){
// // 		
// 		G4Tubs* fDetSolid = new G4Tubs("FieldBox", 45-i*5,50-i*5,24,0 , 360*CLHEP::deg);
// 		G4LogicalVolume* fDetectorLogical = new G4LogicalVolume(fDetSolid, material, "fDetectorLogical", 0, 0, 0);
// // 		
// 		G4RotationMatrix* rot=new G4RotationMatrix();
// 		G4ThreeVector movef(0, 0, i*50-fSTMagnetDepth/2.+24);
// 		std::stringstream ss;
// 		ss<<"SiSegmentPhys"<<i<<"_"<<0;
// 		new G4PVPlacement(rot,movef, fDetectorLogical,ss.str(),  fFieldVolume, false, 0);
// 	}
	
// 	
// 	// Finally place fFieldVolume in expHallLog
// 
	G4ThreeVector movem(0, 0, fSTMagTargetOffset+fSTMagnetDepth/2.);
	
    // Do we even need to store the phys?
	fDicePhys = new G4PVPlacement(new G4RotationMatrix(),                       //rotation
			movem,                    //at position
			fFieldVolume,             //its logical volume
			"MagnetFieldPhys",             //its name
			expHallLog,         //its mother  volume
			false,        //no boolean operation
			false,       //copy number
			0);          //overlaps checking

}

void ApparatusDICE::CreateCycloneField(){
	CreateHeliosField();
}

void ApparatusDICE::BuildCycloneYoke(){
	BuildHeliosYoke();
}




void ApparatusDICE::BuildPlacePhotonShieldTest(G4LogicalVolume* expHallLog)
{
    bool ShieldOff=fRemoveShield;
    
    G4double fDetSide = 50.*CLHEP::mm;
    G4double fDetThick = 5.*CLHEP::mm;
    G4double fDetPlace = 50.*CLHEP::mm;
    
    G4Box* DetBox = new G4Box("DetBox", fDetThick/2., fDetSide/2., fDetSide/2.);
    
	G4Material* detmat = G4Material::GetMaterial(fWaferMaterial);
	G4LogicalVolume *DetLog = new G4LogicalVolume(DetBox, detmat,"TestDetector",0,0,0);

	G4RotationMatrix* rotate = new G4RotationMatrix;
    new G4PVPlacement(rotate,G4ThreeVector(0,0,fDetSide/2.+fDetPlace), DetLog,
			"SiSegmentPhys0_0", expHallLog,false,0);
    
    
   G4Tubs* TestPipeTub = new G4Tubs("TestPipeTub", fDetSide,fDetSide+5*mm,fDetPlace+fDetSide, 0,360*CLHEP::deg);

//     G4Material* TestPipeMat = G4Material::GetMaterial("G4_STAINLESS-STEEL");
    G4Material* TestPipeMat = G4Material::GetMaterial("Aluminum");
	G4LogicalVolume *TestPipeLog = new G4LogicalVolume(TestPipeTub, TestPipeMat,"TestPipeLog",0,0,0);
    new G4PVPlacement(rotate,G4ThreeVector(0,0,0), TestPipeLog,"TestPipe", expHallLog,false,0);
    
    
    if(ShieldOff) return;
    
    G4double ShieldAngle=atan((fDetThick/2.)/fDetPlace);
    G4double ShieldOneZ=fDetPlace-fPhotonShieldLength+fPhotonShieldLayerOneThickness*.5;
    G4double ShieldTwoZ=ShieldOneZ+(fPhotonShieldLayerOneThickness+fPhotonShieldLayerTwoThickness)*.5;
    G4double ShieldThreeZ=ShieldTwoZ+(fPhotonShieldLayerTwoThickness+fPhotonShieldLayerThreeThickness)*.5;
    
    G4Box* ShieldBoxOne = new G4Box("WaTaBox", fDetThick/2., fDetSide/2., fPhotonShieldLayerOneThickness*0.5-0.1*mm);
    G4Box* ShieldBoxTwo = new G4Box("TinBox", fDetThick/2., fDetSide/2., fPhotonShieldLayerTwoThickness*0.5-0.1*mm -0.01*mm);
    G4Box* ShieldBoxThree = new G4Box("CopperBox", fDetThick/2., fDetSide/2., fPhotonShieldLayerThreeThickness*0.5-0.1*mm);
    G4Box* CutBox = new G4Box("CutBox", fDetThick, fDetSide, fPhotonShieldLength);
    
    rotate = new G4RotationMatrix;
    rotate->rotateY(-ShieldAngle);
    
	G4SubtractionSolid* ShieldSolidOne = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxOne,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldOneZ+fDetThick/cos(ShieldAngle),0,0));
 	G4SubtractionSolid* ShieldSolidTwo = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxTwo,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldTwoZ+fDetThick/cos(ShieldAngle),0,0));
    G4SubtractionSolid* ShieldSolidThree = new G4SubtractionSolid("PhotonShieldOneCut",ShieldBoxThree,CutBox,rotate,G4ThreeVector(tan(ShieldAngle)*ShieldThreeZ+fDetThick/cos(ShieldAngle),0,0));
    
    rotate = new G4RotationMatrix;
    rotate->rotateY(ShieldAngle);
    
	ShieldSolidOne = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidOne,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldOneZ+fDetThick/cos(ShieldAngle)),0,0));
 	ShieldSolidTwo = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidTwo,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldTwoZ+fDetThick/cos(ShieldAngle)),0,0));
    ShieldSolidThree = new G4SubtractionSolid("PhotonShieldOneCut",ShieldSolidThree,CutBox,rotate,G4ThreeVector(-(tan(ShieldAngle)*ShieldThreeZ+fDetThick/cos(ShieldAngle)),0,0));   
    
    
    G4Material* shieldmat = G4Material::GetMaterial(fPhotonShieldLayerOneMaterial);
	G4LogicalVolume *ShieldOne = new G4LogicalVolume(ShieldSolidOne, shieldmat,"PhotonShieldLayerOneLog",0,0,0);
	ShieldOne->SetVisAttributes(OneVisAtt);
    
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerTwoMaterial);
	G4LogicalVolume *ShieldTwo = new G4LogicalVolume(ShieldSolidTwo, shieldmat,"PhotonShieldLayerTwoLog",0,0,0);
	ShieldTwo->SetVisAttributes(TwoVisAtt);
    
    shieldmat = G4Material::GetMaterial(fPhotonShieldLayerThreeMaterial);
	G4LogicalVolume *ShieldThree = new G4LogicalVolume(ShieldSolidThree, shieldmat,"PhotonShieldLayerThreeLog",0,0,0);
	ShieldThree->SetVisAttributes(ThreeVisAtt);
    
	rotate = new G4RotationMatrix;
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldOneZ-fShieldOffset), ShieldOne,"ShieldOne", expHallLog,false,0);
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldTwoZ-fShieldOffset), ShieldTwo,"ShieldTwo", expHallLog,false,0);
    new G4PVPlacement(rotate,G4ThreeVector(0,0,ShieldThreeZ-fShieldOffset), ShieldThree,"ShieldThree", expHallLog,false,0);
}


void ApparatusDICE::BuildPlaceHPGe(G4LogicalVolume* expHallLog){
  G4Box* Box1 = new G4Box("Box1", 10*cm, 10*cm, 15*cm  ); 
  G4Box* Box2 = new G4Box("Box1", 10.1*cm, 10.1*cm, 15.1*cm  ); 
  
    G4RotationMatrix* rotate1 = new G4RotationMatrix;
    rotate1->rotateY(45*deg);

    G4VSolid* BoxCut1 = new G4SubtractionSolid("BoxCut1", Box1, Box2,rotate1,G4ThreeVector(15*cm,0,20*cm));

    G4RotationMatrix* rotate2 = new G4RotationMatrix;
    rotate2->rotateY(-45*deg);
    G4VSolid* BoxCut2 = new G4SubtractionSolid("BoxCut2", BoxCut1, Box2,rotate2,G4ThreeVector(-15*cm,0,20*cm));

  
    G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateX(-45*deg);
    G4VSolid* BoxCut3 = new G4SubtractionSolid("BoxCut3", BoxCut2, Box2,rotate3,G4ThreeVector(0,15*cm,20*cm));
  
    G4RotationMatrix* rotate4 = new G4RotationMatrix;
    rotate4->rotateX(45*deg);
    G4VSolid* BoxCut4 = new G4SubtractionSolid("BoxCut3", BoxCut3, Box2,rotate4,G4ThreeVector(0,-15*cm,20*cm));
  
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	G4LogicalVolume *HPGe = new G4LogicalVolume(BoxCut4, mMaterial,"HPGeL",0,0,0);
  
    
    
    for(int i=0;i<4;i++){
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateX(135*deg);
        rotateplace->rotateZ(90*i*deg);
        G4ThreeVector vecplace(0,0,35*cm);
        vecplace*=(*rotateplace);
        
        G4RotationMatrix* rotateface = new G4RotationMatrix;
//         rotateface->rotateX(180*deg);
        rotateface->rotateZ(-90*i*deg);
        rotateface->rotateX(45*deg);
    new G4PVPlacement(rotateface,vecplace, HPGe,"HPGe", expHallLog,false,0);
    }
    
    for(int i=0;i<4;i++){
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateX(45*deg);
        rotateplace->rotateZ(90*i*deg);
        G4ThreeVector vecplace(0,0,35*cm);
        vecplace*=(*rotateplace);
        
        G4RotationMatrix* rotateface = new G4RotationMatrix;
//         rotateface->rotateX(180*deg);
        rotateface->rotateZ(-90*i*deg);
        rotateface->rotateX(135*deg);
    new G4PVPlacement(rotateface,vecplace, HPGe,"HPGe", expHallLog,false,0);
    }
    
        for(int i=0;i<8;i++){
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateX(90*deg);
        rotateplace->rotateZ(45*i*deg);
        G4ThreeVector vecplace(0,0,35*cm);
        vecplace*=(*rotateplace);
        
        G4RotationMatrix* rotateface = new G4RotationMatrix;
//         rotateface->rotateX(180*deg);
        rotateface->rotateX(90*deg);
        rotateface->rotateY(45*i*deg);
    new G4PVPlacement(rotateface,vecplace, HPGe,"HPGe", expHallLog,false,0);
    }
}




void ApparatusDICE::BuildPlaceNewRecoilShaddow(G4LogicalVolume* expHallLog){
    
    // Build and place the photon shield
    
    G4Box* Box1 = new G4Box("Box1", 3*cm, 3*cm, 2*cm  ); 
    G4Box* Box2 = new G4Box("Box2", 3.1*cm, 3.1*cm, 2*cm); 
  
    G4VSolid* BoxCut1 = Box1;
    
    double dxy=(1.4+2*sqrt(2))*cm;
    int sign=1;
    for(int i=0;i<4;i++){
        G4RotationMatrix* rotate1 = new G4RotationMatrix;
        rotate1->rotateY(sign*45*deg*(i%2));
        rotate1->rotateX(-sign*45*deg*((i+1)%2));
        
        
        BoxCut1 = new G4SubtractionSolid("BoxCut1", BoxCut1, Box2,rotate1,G4ThreeVector(sign*dxy*(i%2),sign*dxy*((i+1)%2),-2*cm));
        
        if(i==1)sign=-1;
    }
    
    G4Cons* CutCone = new G4Cons("CutCone", 0,0, 0, 1.4*cm,0.7*cm, 0, 2*CLHEP::pi);
    for(int i=0;i<4;i++){
        
        BoxCut1 = new G4SubtractionSolid("BoxCut2", BoxCut1, CutCone,new G4RotationMatrix,G4ThreeVector(0,0,(i-1.69)*cm));
    }
    
    
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	G4LogicalVolume *ShieldTest = new G4LogicalVolume(BoxCut1, mMaterial,"ShieldTestL",0,0,0);
    
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,2.5*cm), ShieldTest,"ShieldTest", expHallLog,false,0);
    
    
	for(int i=0;i<4;i++){
    
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateZ(90*i*deg);
        G4ThreeVector vecplace(0,3*cm+fRMSDetHalfDepth,4.5*cm+fRMSDetHalfLength-fRMSGuard);
        vecplace*=(*rotateplace);
        
        new G4PVPlacement(rotateplace,vecplace, BuildMicronSi(),"Detector", expHallLog,false,0);
    
    }
}

    
    
G4int ApparatusDICE::BuildMicronSiN=0;
G4LogicalVolume* ApparatusDICE::BuildMicronSi(){
    
     G4RotationMatrix* rotate = new G4RotationMatrix;

    // Detector crystal mother
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);
    G4Box* siDetMother = new G4Box("siDetMother",fRMSDetHalfWidth,fRMSDetHalfDepth,fRMSDetHalfLength);
	fDetectorLog = new G4LogicalVolume(siDetMother, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);

	// Silicon G4VisAttributes
	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);

	// Place Guard Ring in fDetectorLog
	
    G4Box* siGuardOuter = new G4Box("siGuardOuter", fRMSDetHalfWidth -0.1*mm, fRMSDetHalfDepth, fRMSDetHalfLength -0.1*mm);
    G4double HalfLength= fRMSDetHalfLength-fRMSGuard;
    G4Box* siGuardInner = new G4Box("siGuardOuter", fRMSDetHalfWidth-fRMSGuard +0.1*mm, fRMSDetHalfDepth+0.1*mm,HalfLength  +0.1*mm);
	G4VSolid* siGuard = new G4SubtractionSolid("siGuard", siGuardOuter, siGuardInner,0,G4ThreeVector(0,0,0));
    G4LogicalVolume* SiGuardLog =new G4LogicalVolume(siGuard, material, "SiGuardLog", 0,0,0);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0), SiGuardLog,tt.str(),  fDetectorLog, false, 0);
	
	// Place Detector Elements in fDetectorLog  
    
    G4Box* siDetSegment = new G4Box("siSeg",fRMSDetHalfWidth-fRMSGuard-0.01*mm, fRMSDetHalfDepth, HalfLength/fRMSSegZ -0.1*mm);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	for(int z=0;z<fRMSSegZ;z++){
        G4double zz=-HalfLength+(HalfLength*2/fRMSSegZ)*(z+0.5);
    
        std::stringstream temp;
        temp<<"SiSegmentPhys_"<<1+z<<"_"<<BuildMicronSiN;
        new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);
	}
	
    BuildMicronSiN++;
    return fDetectorLog;
}


void ApparatusDICE::BuildPlaceRMSFocal(G4LogicalVolume* expHallLog){
    
    G4Box* Box1 = new G4Box("Box1", 75.633*0.5*mm, 57.306*0.5*mm, 0.1*mm); 
	G4Material *Simaterial = G4Material::GetMaterial(fWaferMaterial);
	G4LogicalVolume *DSSD = new G4LogicalVolume(Box1, Simaterial,"DSSD",0,0,0);
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,0.05*mm), DSSD,"DSSDSolid", expHallLog,false,0);
    
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,40*mm), DSSD,"SiSegmentPhys_0_0", expHallLog,false,0);
    
    double R1=(sqrt(300.)/3.14159);
    double R2=(sqrt(450.)/3.14159);
    double R3=(sqrt(900.)/3.14159);
    double dR=3.3;
    
    double T1=0.5*500*um;
    double T2=0.5*1000*um;
    double T3=0.5*1500*um;
    double T4=0.5*2000*um;
    
//     500
//     1000
//     1500
//     2000
    
    G4Tubs* Si_dE1 = new G4Tubs("Si_dE1", 0,R1,T2, 0,360*CLHEP::deg);
    G4Tubs* Si_dE1out = new G4Tubs("Si_dE1out", R1+0.1*mm,R1+dR,T2, 0,360*CLHEP::deg);
	G4LogicalVolume *Si_dE1log = new G4LogicalVolume(Si_dE1, Simaterial,"Si_dE1log",0,0,0);
	G4LogicalVolume *Si_dE1outlog = new G4LogicalVolume(Si_dE1out, Simaterial,"Si_dE1outlog",0,0,0);
    
    G4Tubs* Si_dE2 = new G4Tubs("Si_dE2", 0,R1,T1, 0,360*CLHEP::deg);
    G4Tubs* Si_dE2out = new G4Tubs("Si_dE2out", R1+0.1*mm,R1+dR,T1, 0,360*CLHEP::deg);
	G4LogicalVolume *Si_dE2log = new G4LogicalVolume(Si_dE2, Simaterial,"Si_dE2log",0,0,0);
	G4LogicalVolume *Si_dE2outlog = new G4LogicalVolume(Si_dE2out, Simaterial,"Si_dE2outlog",0,0,0);
    
    G4Tubs* Si_dE3 = new G4Tubs("Si_dE3", 0,R3,T1, 0,360*CLHEP::deg);
    G4Tubs* Si_dE3out = new G4Tubs("Si_dE3out", R3+0.1*mm,R3+dR,T1, 0,360*CLHEP::deg);
	G4LogicalVolume *Si_dE3log = new G4LogicalVolume(Si_dE3, Simaterial,"Si_dE3log",0,0,0);
	G4LogicalVolume *Si_dE3outlog = new G4LogicalVolume(Si_dE3out, Simaterial,"Si_dE3outlog",0,0,0);
    
    G4RotationMatrix* rotateplace=new G4RotationMatrix;

    
    

    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,5*mm),Si_dE1log,"SiSegmentPhys_0_1", expHallLog,false,0);
    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,5*mm),Si_dE1outlog,"X", expHallLog,false,0);
    
    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,2*mm),Si_dE2log,"SiSegmentPhys_0_2", expHallLog,false,0);
    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,2*mm),Si_dE2outlog,"X", expHallLog,false,0);
    
    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,18*mm),Si_dE3log,"SiSegmentPhys_0_3", expHallLog,false,0);
    new G4PVPlacement(rotateplace,G4ThreeVector(0,0,18*mm),Si_dE3outlog,"X", expHallLog,false,0);
    
}
