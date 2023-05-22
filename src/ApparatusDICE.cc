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

#include "G4TwoVector.hh"
#include "G4ExtrudedSolid.hh"

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
    OneVisAtt = new G4VisAttributes(G4Colour(PB_COL)); // Red
    TwoVisAtt = new G4VisAttributes(G4Colour(SN_COL)); // grey
    ThreeVisAtt = new G4VisAttributes(G4Colour(CU_COL)); // Yellow
    FourVisAtt = new G4VisAttributes(G4Colour(0.2,0.4,0.3)); // Dark Green
    FiveVisAtt = new G4VisAttributes(G4Colour(196,160,0)); //yellow
    SixVisAtt = new G4VisAttributes(G4Colour(0.51,0.42,0.06)); //yellow
    SevenVisAtt = new G4VisAttributes(G4Colour(0.3,0.2,0.3));  //yellow 
//     SevenVisAtt = new G4VisAttributes(G4Colour(207,173,23));  //yellow 
    
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

    
    // BB34 finalised
    fBB34PCB_HalfThickness = 2.05*CLHEP::mm;
	fBB34PCB_Shelf = 1.15*CLHEP::mm;
	fBB34PCB_ShelfWidth = 2.1*CLHEP::mm;
	fBB34PCB_HalfWidth = 47*CLHEP::mm;
	fBB34PCB_HalfALength = 28*CLHEP::mm;
	fBB34PCB_Blength = 21*CLHEP::mm;
	fBB34PCB_Angle = 45*CLHEP::deg;
	fBB34PCB_Clength = fBB34PCB_Blength*tan(fBB34PCB_Angle);
	
	fBB34Chip_HalfThickness = 0.75*CLHEP::mm;
	fBB34Chip_HalfWidth = 43*CLHEP::mm;
	fBB34Chip_HalfLength = 23*CLHEP::mm;
	fBB34Chip_GuardWidth = 3*CLHEP::mm;
	fBB34Chip_Nseg=16;
	fBB34Chip_Dead=500*CLHEP::nm;
	fBB34Chip_DeadAngle = 10*CLHEP::deg; /// An approximation for apparent inter-segment losses
	fBB34Chip_Al=500*CLHEP::nm;
    
    
	///////////////////////////////////////////////////////
	////// Controlled by DetectorConstruction class ///////
	///////////////////////////////////////////////////////
	
	fAdjLength= 10*CLHEP::mm;
	fAdjLengthA= 10*CLHEP::mm;
	fAdjLengthB= 10*CLHEP::mm;
	fAdjLengthC= 10*CLHEP::mm;
	fAdjLengthD= 10*CLHEP::mm;
	fAdjLengthE= 10*CLHEP::mm;
	fAdjNumber=4;
	
	fUseA=false;
	fUseB=false;
	fUseC=false;
	fUseD=false;
	fUseE=false;
	
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
    fBuildOption=Opt;

	switch(Opt) {
		case 1: BuildPlaceShoeBox(expHallLog); break;
		case 2: BuildPlaceHelios(expHallLog); break;
		case 3: BuildPlaceSuperTube(expHallLog); break;
		case 4: BuildPlaceRecoilShadow(expHallLog); break;
		case 5: BuildPlaceCyclone(expHallLog); break;
		case 6: BuildPlacePhotonShieldTest(expHallLog); break;
		case 7: BuildPlaceNewRecoilShaddow(expHallLog); break;
		case 8: BuildPlaceRMSFocal(expHallLog); break;
		case 9: BuildPlaceSimplifiedRecoilShaddow(expHallLog); break;
		case 10: BuildPlaceAdjustableRecoilShaddow(expHallLog); break;
		case 11: BuildPlaceFlatOrange(expHallLog,0); break;
		case 12: BuildPlaceTripleOrange(expHallLog); break;
        
        
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
    G4Material* TestPipeMat = G4Material::GetMaterial("Aluminium");
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
    
	ShieldTest->SetVisAttributes(new G4VisAttributes(G4Colour(0.2,0.4,0.3)));
    
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,2.5*cm), ShieldTest,"ShieldTest", expHallLog,false,0);
    
    
    
    fRMSDetHalfDepth=0.05*mm;
	for(int i=0;i<4;i++){
    
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateZ(90*i*deg);
        G4ThreeVector vecplace(0,3*cm+fRMSDetHalfDepth,4.5*cm+fRMSDetHalfLength-fRMSGuard);
        vecplace*=(*rotateplace);
        
        new G4PVPlacement(rotateplace,vecplace, BuildMicronSi(),"Detector", expHallLog,false,0);
    
    }
    
    fRMSDetHalfDepth=0.5*mm;
	for(int i=0;i<4;i++){
    
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateZ(90*i*deg);
        G4ThreeVector vecplace(0,4*cm+fRMSDetHalfDepth,4.5*cm+fRMSDetHalfLength-fRMSGuard);
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
    
	SiGuardLog->SetVisAttributes(SixVisAtt);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0), SiGuardLog,tt.str(),  fDetectorLog, false, 0);
	
	// Place Detector Elements in fDetectorLog  
    
    G4Box* siDetSegment = new G4Box("siSeg",fRMSDetHalfWidth-fRMSGuard-0.01*mm, fRMSDetHalfDepth, HalfLength/fRMSSegZ -0.1*mm);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	fSiSegmentLog->SetVisAttributes(FiveVisAtt);
    
    
	for(int z=0;z<fRMSSegZ;z++){
        G4double zz=-HalfLength+(HalfLength*2/fRMSSegZ)*(z+0.5);
    
        std::stringstream temp;
        temp<<"SiSegmentPhys_"<<1+z<<"_"<<BuildMicronSiN;
        new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);
	}
	
	
    BuildMicronSiN++;
    return fDetectorLog;
}


G4LogicalVolume* ApparatusDICE::BuildBB34(){
	
	std::vector<G4TwoVector> PCBPolygon(6);
	PCBPolygon[0].set(fBB34PCB_HalfWidth,-fBB34PCB_HalfALength);
	PCBPolygon[1].set(fBB34PCB_HalfWidth,fBB34PCB_HalfALength);
	PCBPolygon[2].set(fBB34PCB_HalfWidth-fBB34PCB_Clength,fBB34PCB_HalfALength+fBB34PCB_Blength);
	PCBPolygon[3].set(-fBB34PCB_HalfWidth+fBB34PCB_Clength,fBB34PCB_HalfALength+fBB34PCB_Blength);
	PCBPolygon[4].set(-fBB34PCB_HalfWidth,fBB34PCB_HalfALength);
	PCBPolygon[5].set(-fBB34PCB_HalfWidth,-fBB34PCB_HalfALength);
	G4VSolid* PCBouter = new G4ExtrudedSolid("PCBouter", PCBPolygon, fBB34PCB_HalfThickness, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
	
	G4RotationMatrix* rotate = new G4RotationMatrix;
    
	// G4VisAttributes
	G4VisAttributes* vis_att_si = new G4VisAttributes(true,G4Colour(0.0,1.0,1.0));
    G4Material* SiliconMat = G4Material::GetMaterial(fWaferMaterial);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	G4VisAttributes* vis_att_hid = new G4VisAttributes(false);
    
    // Detector mother volume
	fDetectorLog = new G4LogicalVolume(PCBouter, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);


	// Make boxes
	G4Box* Chip = new G4Box("Chip", fBB34Chip_HalfWidth, fBB34Chip_HalfLength, fBB34Chip_HalfThickness);
	G4Box* ChipMother = new G4Box("ChipMother", fBB34Chip_HalfWidth, fBB34Chip_HalfLength, fBB34Chip_HalfThickness+fBB34Chip_Al);
	G4Box* ChipAl = new G4Box("ChipAl", fBB34Chip_HalfWidth, fBB34Chip_HalfLength, fBB34Chip_Al*0.5);
	G4Box* ChipPCBHole = new G4Box("ChipPCBHole", fBB34Chip_HalfWidth+0.01*mm, fBB34Chip_HalfLength+0.01*mm, fBB34PCB_HalfThickness+0.01*mm);//Oversized for clearance
	G4Box* InnerChipHole = new G4Box("InnerChipHole", fBB34Chip_HalfWidth-fBB34PCB_ShelfWidth, fBB34Chip_HalfLength-fBB34PCB_ShelfWidth, fBB34PCB_HalfThickness*2);
	G4Box* InnerGuardHole = new G4Box("InnerGuardHole", fBB34Chip_HalfWidth-fBB34Chip_GuardWidth, fBB34Chip_HalfLength-fBB34Chip_GuardWidth, fBB34PCB_HalfThickness*2);

	G4VSolid* siGuard = new G4SubtractionSolid("siGuard", Chip, InnerGuardHole,0,G4ThreeVector(0,0,0));

	// MakePlace PCB in fDetectorLog
	G4VSolid* PCBsolidmid = new G4SubtractionSolid("PCBsolidmid", PCBouter, InnerChipHole,0,G4ThreeVector(0,0,0));
	G4VSolid* PCBsolid = new G4SubtractionSolid("PCBsolid", PCBsolidmid, ChipPCBHole,0,G4ThreeVector(0,0,fBB34PCB_Shelf));
    G4LogicalVolume* PCBLog = new G4LogicalVolume(PCBsolid, G4Material::GetMaterial("FR4"), "PCBLog", 0,0,0);
	PCBLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.3,0.6,0.1)));
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0),PCBLog, "PCBVol",  fDetectorLog, false, false,true);
	
	// Make SiMother
    G4LogicalVolume* ChipMotherLog = new G4LogicalVolume(ChipMother, SiliconMat, "ChipMotherLog", 0,0,0);
	ChipMotherLog->SetVisAttributes(vis_att_hid);

	// MakePlace ChipMetal in ChipMotherLog
    G4LogicalVolume* ChipMetalLog = new G4LogicalVolume(ChipAl, G4Material::GetMaterial("Aluminium"), "ChipMotherLog", 0,0,0);
	ChipMetalLog->SetVisAttributes(vis_att_hid);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,fBB34Chip_HalfThickness+fBB34Chip_Al*0.5),ChipMetalLog, "ChipMetalVol",  ChipMotherLog, false, false,true);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,-fBB34Chip_HalfThickness-fBB34Chip_Al*0.5),ChipMetalLog, "ChipMetalVol",  ChipMotherLog, false, false,true);
	
	
	// Place Guard Ring in ChipMotherLog
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    G4LogicalVolume* SiGuardLog =new G4LogicalVolume(siGuard, SiliconMat, "SiGuardLog", 0,0,0);
	SiGuardLog->SetVisAttributes(vis_att_si);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0),SiGuardLog, tt.str(),  ChipMotherLog, false, false,true);
    
	// Place Detector Elements in ChipMotherLog  
    
	G4double SegHalfHeight=fBB34Chip_HalfThickness-fBB34Chip_Dead;
	G4double SegHalfWidth=(fBB34Chip_HalfWidth-fBB34Chip_GuardWidth)/fBB34Chip_Nseg;
	G4double SegHalfWidthTop=SegHalfWidth-SegHalfHeight*tan(fBB34Chip_DeadAngle);
	
	std::vector<G4TwoVector> SegPolygon(6);
	SegPolygon[0].set(SegHalfHeight,SegHalfWidthTop);
	SegPolygon[1].set(0,SegHalfWidth);
	SegPolygon[2].set(-SegHalfHeight,SegHalfWidthTop);
	SegPolygon[3].set(-SegHalfHeight,-SegHalfWidthTop);
	SegPolygon[4].set(0,-SegHalfWidth);
	SegPolygon[5].set(SegHalfHeight,-SegHalfWidthTop);

	G4VSolid* siDetSegment = new G4ExtrudedSolid("siDetSegment", SegPolygon, fBB34Chip_HalfLength-fBB34Chip_GuardWidth-0.01*mm, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, SiliconMat, "fSiSegmentLog", 0,0,0);
	SiGuardLog->SetVisAttributes(vis_att_si);
	
	G4RotationMatrix* SegRot = new G4RotationMatrix;
	SegRot->rotateX(90*CLHEP::deg);
	SegRot->rotateZ(90*CLHEP::deg);
	
	for(int z=0;z<fBB34Chip_Nseg;z++){
        G4double xx=SegHalfWidth*(z*2+1)-fBB34Chip_HalfWidth+fBB34Chip_GuardWidth;
    
        std::stringstream temp;
        temp<<"SiSegmentPhys_"<<1+z<<"_"<<BuildMicronSiN;
        new G4PVPlacement(SegRot,G4ThreeVector(xx,0,00), fSiSegmentLog, temp.str(),  ChipMotherLog, false, false,true);
	}
	
	// Place Detector ChipMotherLog in fDetectorLog	
	new G4PVPlacement(rotate,G4ThreeVector(0,0,((fBB34PCB_Shelf+fBB34Chip_HalfThickness)+fBB34Chip_Al)-fBB34PCB_HalfThickness+0.01*mm),ChipMotherLog, "SiliChip",  fDetectorLog, false, false,true);

	
    BuildMicronSiN++;
    return fDetectorLog;
}



void ApparatusDICE::BuildPlaceRMSFocal(G4LogicalVolume* expHallLog){
    
//     G4Box* Box1 = new G4Box("Box1", 75.633*0.5*mm, 57.306*0.5*mm, 0.1*mm); 
//     G4Box* Box1 = new G4Box("Box1", 5*0.5*mm, 40*0.5*mm, 1*mm);
// 	G4Material *Simaterial = G4Material::GetMaterial(fWaferMaterial);
// 	G4LogicalVolume *DSSD = new G4LogicalVolume(Box1, Simaterial,"DSSD",0,0,0);
// //     new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,0.05*mm), DSSD,"DSSDSolid", expHallLog,false,0);
//     
//     new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,20*mm), DSSD,"SiSegmentPhys_0_0", expHallLog,false,0);
//     
//     
//     G4Box* Box2 = new G4Box("Box1", 5*0.5*mm, 40*0.5*mm, 0.75*mm);
// 	G4LogicalVolume *DSSD2 = new G4LogicalVolume(Box2, Simaterial,"DSSD",0,0,0);
//     
//     new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,-20*mm), DSSD2,"SiSegmentPhys_0_1", expHallLog,false,0);
    
//     double R1=(sqrt(300.)/3.14159);
//     double R2=(sqrt(450.)/3.14159);
//     double R3=(sqrt(900.)/3.14159);
//     double dR=3.3;
//     
//     double T1=0.5*500*um;
//     double T2=0.5*1000*um;
//     double T3=0.5*1500*um;
//     double T4=0.5*2000*um;
    
//     500
//     1000
//     1500
//     2000
    
//     G4Tubs* Si_dE1 = new G4Tubs("Si_dE1", 0,R1,T1, 0,360*CLHEP::deg);
//     G4Tubs* Si_dE1out = new G4Tubs("Si_dE1out", R1+0.1*mm,R1+dR,T1, 0,360*CLHEP::deg);
// 	G4LogicalVolume *Si_dE1log = new G4LogicalVolume(Si_dE1, Simaterial,"Si_dE1log",0,0,0);
// 	G4LogicalVolume *Si_dE1outlog = new G4LogicalVolume(Si_dE1out, Simaterial,"Si_dE1outlog",0,0,0);
//     
//     G4Tubs* Si_dE2 = new G4Tubs("Si_dE2", 0,R1,T1, 0,360*CLHEP::deg);
//     G4Tubs* Si_dE2out = new G4Tubs("Si_dE2out", R1+0.1*mm,R1+dR,T1, 0,360*CLHEP::deg);
// 	G4LogicalVolume *Si_dE2log = new G4LogicalVolume(Si_dE2, Simaterial,"Si_dE2log",0,0,0);
// 	G4LogicalVolume *Si_dE2outlog = new G4LogicalVolume(Si_dE2out, Simaterial,"Si_dE2outlog",0,0,0);
//     
//     G4Tubs* Si_dE3 = new G4Tubs("Si_dE3", 0,R3,T1, 0,360*CLHEP::deg);
//     G4Tubs* Si_dE3out = new G4Tubs("Si_dE3out", R3+0.1*mm,R3+dR,T1, 0,360*CLHEP::deg);
// 	G4LogicalVolume *Si_dE3log = new G4LogicalVolume(Si_dE3, Simaterial,"Si_dE3log",0,0,0);
// 	G4LogicalVolume *Si_dE3outlog = new G4LogicalVolume(Si_dE3out, Simaterial,"Si_dE3outlog",0,0,0);
//     
//     G4RotationMatrix* rotateplace=new G4RotationMatrix;

    
    

//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,5*mm),Si_dE1log,"SiSegmentPhys_0_1", expHallLog,false,0);
//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,5*mm),Si_dE1outlog,"X", expHallLog,false,0);
    
//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,2*mm),Si_dE2log,"SiSegmentPhys_0_2", expHallLog,false,0);
//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,2*mm),Si_dE2outlog,"X", expHallLog,false,0);
//     
//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,18*mm),Si_dE3log,"SiSegmentPhys_0_3", expHallLog,false,0);
//     new G4PVPlacement(rotateplace,G4ThreeVector(0,0,18*mm),Si_dE3outlog,"X", expHallLog,false,0);
    
// new G4PVPlacement(rotateplace,G4ThreeVector(0,0,15*mm),Si_dE1log,"SiSegmentPhys_0_1", expHallLog,false,0);
// new G4PVPlacement(rotateplace,G4ThreeVector(0,0,15*mm),Si_dE1outlog,"X", expHallLog,false,0);
    
    
    
    
    	new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,0,0), BuildBB34(),"Detector",  expHallLog, false, 0);
    
}





void ApparatusDICE::BuildPlaceSimplifiedRecoilShaddow(G4LogicalVolume* expHallLog){
    
    
    // Build and place the photon shield
    G4double Yaxisoffset=0.5*cm;
    G4double NominalZcut=1.0*cm; //Position we cut the ramp at
    G4double AxisToDetY=4*cm;
    G4double Zblock=0.5*cm; //half
    
    G4double ScintThick=1*mm; //half
    
    
  	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
    G4Box* Box1 = new G4Box("Box1", 3*cm, 3*cm, Zblock); 
	G4LogicalVolume *ShieldFrontL = new G4LogicalVolume(Box1, mMaterial,"ShieldFrontL",0,0,0);
	ShieldFrontL->SetVisAttributes(FourVisAtt);
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,3.0*cm+Yaxisoffset,0.5*cm), ShieldFrontL,"ShieldFront", expHallLog,false,0);  
   
    G4double Zdet=Zblock*2+fBB34Chip_HalfWidth; 
    G4double Ydet=AxisToDetY;
    
    G4double cutdetangle=atan(AxisToDetY/(Zdet+fBB34Chip_HalfWidth-fBB34Chip_GuardWidth-NominalZcut));
    G4double Zramp=NominalZcut+Yaxisoffset/tan(cutdetangle);

    G4double rampangle=atan(Yaxisoffset/Zramp);
    
    G4double lengthramp=8*cm;//half
    G4double heightramp=0.75*cm;
    G4double widthramp=2*cm;
    G4double ScintDep=ScintThick*2;//True not half
    
    G4Box* Box2 = new G4Box("Box2", widthramp, heightramp,lengthramp); 
    
    G4Box* CutBox = new G4Box("CutBox", widthramp*1.1, heightramp,lengthramp); 
    
    
    G4double ramphypo=sqrt(lengthramp*lengthramp+heightramp*heightramp);
    
    G4double projectangle= atan(lengthramp/heightramp)-rampangle;
    
    
    G4RotationMatrix* rotate1 = new G4RotationMatrix;
    rotate1->rotateX(rampangle);
  
//     G4VSolid* Ramp1 = new G4UnionSolid("Ramp1", Box2, CutBox,rotate1,G4ThreeVector(-1*cm,ramphypo*sin(projectangle),ramphypo*cos(projectangle)));
    G4VSolid* Ramp1 = new G4SubtractionSolid("Ramp1", Box2, CutBox,rotate1,G4ThreeVector(0,ramphypo*cos(projectangle)-heightramp,ramphypo*sin(projectangle)-lengthramp));
    
    G4VSolid* Ramp2 = new G4SubtractionSolid("Ramp2", Ramp1, CutBox,new G4RotationMatrix,G4ThreeVector(0,2*heightramp-ScintDep,2*heightramp/tan(rampangle)));  
    
	G4LogicalVolume *ShieldRampL = new G4LogicalVolume(Ramp2, mMaterial,"ShieldRampL",0,0,0);
    ShieldRampL->SetVisAttributes(FourVisAtt);
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,heightramp+Yaxisoffset,lengthramp+Zramp), ShieldRampL,"ShieldRampL", expHallLog,false,0);
 
   
        G4RotationMatrix* rotateplace = new G4RotationMatrix;
        rotateplace->rotateX(90*deg);
        rotateplace->rotateZ(-90*deg);
        G4ThreeVector vecplace(0,Ydet,Zdet);
  
     new G4PVPlacement(rotateplace,vecplace, BuildBB34(),"Detector", expHallLog,false,0);


    G4Material* scinmat = G4Material::GetMaterial(fWaferMaterial);
     
     
    G4Box* ScintBox = new G4Box("Box1",fBB34Chip_HalfLength, ScintThick,fBB34Chip_HalfWidth ); 
    G4LogicalVolume* fScintLog =new G4LogicalVolume(ScintBox, scinmat, "fScintLog", 0,0,0);
    fScintLog->SetVisAttributes(SevenVisAtt);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    
	new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,Ydet+fRMSDetHalfDepth+0.5*mm+ScintThick,Zdet), fScintLog,tt.str(),  expHallLog, false, 0);
    
    std::stringstream TT;
    TT<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    
// 	new G4PVPlacement(rotateplace,G4ThreeVector(0,heightramp/tan(rampangle)+fRMSDetHalfLength+Zramp+1*mm,heightramp+Yaxisoffset+ScintThick-ScintDep+0.1*mm), fScintLog,TT.str(),  expHallLog, false, 0);
    
	new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,heightramp*2+Yaxisoffset+ScintThick-ScintDep+0.1*mm,2*heightramp/tan(rampangle)+fRMSDetHalfLength+Zramp+1*mm), fScintLog,TT.str(),  expHallLog, false, 0);

}



void ApparatusDICE::BuildPlaceAdjustableRecoilShaddow(G4LogicalVolume* expHallLog){
    
    //Intial Design Variables
    G4double ShieldXWidth=6*cm;
    G4double TargetToShield=0.3*cm;
    G4double ShieldMinThick=0.3*cm;
    G4double ShieldToHole=fAdjLengthA;
    G4double ShieldHole=fAdjLengthB;
    G4double ShieldToDet=fAdjLengthC;
    G4double ShieldDetHeight=fAdjLengthD;
    G4double DetDipAngle=fAdjLengthE;
    
    G4double BeamYOffset=0.4*cm;

    // Build and place the photon shield
    G4double ShieldMaxD=ShieldMinThick+ShieldToHole;
    if(ShieldToDet>ShieldToHole)ShieldMaxD=ShieldMinThick+ShieldToDet;
    
  	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
    G4Box* Box1 = new G4Box("Box1", ShieldXWidth/2, ShieldDetHeight/2, ShieldMaxD/2); 
    G4Box* Box2 = new G4Box("Box2", ShieldXWidth, ShieldDetHeight*1000, ShieldMaxD); 
    
    G4double rampangle1=atan((ShieldToHole-ShieldToDet)/ShieldDetHeight);
    G4RotationMatrix* rotate1 = new G4RotationMatrix;
    rotate1->rotateX(rampangle1);
    
    G4VSolid* Ramp1 = new G4SubtractionSolid("Ramp1", Box1, Box2,rotate1,G4ThreeVector(0,0,ShieldMinThick+0.5*(ShieldToDet+ShieldToHole)-ShieldMaxD/2+ShieldMaxD/cos(rampangle1)));
    
	G4LogicalVolume *ShieldFrontL = new G4LogicalVolume(Ramp1, mMaterial,"ShieldFrontL",0,0,0);
	ShieldFrontL->SetVisAttributes(FourVisAtt);
    new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,ShieldDetHeight/2+BeamYOffset,TargetToShield+ShieldMaxD/2), ShieldFrontL,"ShieldFront", expHallLog,false,0);  

    
    // Build and place the back shield
    
    G4double BB34Span=(fBB34Chip_HalfWidth-fBB34Chip_GuardWidth)*2;
    
    G4double BB34EndY=ShieldDetHeight-BB34Span*sin(DetDipAngle);
    G4double BB34EndZ=ShieldMinThick+ShieldToDet+BB34Span*cos(DetDipAngle);
    
    G4double Ramp2PointZ=ShieldMinThick+ShieldToHole+ShieldHole;
    
    G4double Ramp2Thick=BB34EndZ-Ramp2PointZ;
    G4double rampangle2=atan(Ramp2Thick/BB34EndY);
    
//     G4cout<<G4endl<<"BB34Span "<<BB34Span/mm;
//     G4cout<<G4endl<<"Ramp2PointZ "<<Ramp2PointZ/mm;
//     G4cout<<G4endl<<"BB34EndZ "<<BB34EndZ/mm;
//     G4cout<<G4endl<<"BB34EndY "<<BB34EndY/mm;
//     G4cout<<G4endl<<"Ramp2Thick "<<Ramp2Thick/mm;
//     G4cout<<G4endl<<"rampangle2 "<<rampangle2/deg;
//     G4cout<<G4endl;
    
    if(Ramp2Thick>0&&BB34EndY>0){
    
        G4Box* Box3 = new G4Box("Box3", ShieldXWidth/2, BB34EndY/2, Ramp2Thick/2); 
        G4Box* Box4 = new G4Box("Box4", ShieldXWidth, BB34EndY*1000, Ramp2Thick); 
        
        G4RotationMatrix* rotate2 = new G4RotationMatrix;
        rotate2->rotateX(-rampangle2);
        G4VSolid* Ramp2 = new G4SubtractionSolid("Ramp2", Box3, Box4,rotate2,G4ThreeVector(0,0,(BB34EndY/2)*tan(rampangle2)-Ramp2Thick/2-Ramp2Thick/cos(rampangle2)));
// G4UnionSolid
        
        G4LogicalVolume *ShieldReadL = new G4LogicalVolume(Ramp2, mMaterial,"ShieldReadL",0,0,0);
        ShieldReadL->SetVisAttributes(FourVisAtt);
        new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,BB34EndY/2+BeamYOffset,TargetToShield+ShieldMaxD+ShieldHole+Ramp2Thick/2), ShieldReadL,"ShieldRead", expHallLog,false,0);  
    }
    
    // Place BB34
    
    G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateX(-DetDipAngle);
    rotate3->rotateZ(90*deg);
    rotate3->rotateY(90*deg);
    G4ThreeVector detpos(0,BeamYOffset+ShieldDetHeight-0.5*BB34Span*sin(DetDipAngle),TargetToShield+ShieldMinThick+ShieldToDet+0.5*BB34Span*cos(DetDipAngle));
    G4ThreeVector detoffset(0,fBB34PCB_HalfThickness*1.2*cos(DetDipAngle),fBB34PCB_HalfThickness*1.2*sin(DetDipAngle));
    detpos+=detoffset;
    new G4PVPlacement(rotate3,detpos, BuildBB34(),"BB34Detector", expHallLog,false,0);

    // Place scint
    
    G4double ScintThick=1*mm; //half
    G4double ScintD=20*mm; //half
    
    G4Material* SiliconMat = G4Material::GetMaterial(fWaferMaterial);
    
    G4Box* ScintBox = new G4Box("ScintBox", ScintD, ScintThick, ScintD); 
    G4LogicalVolume *ScintL = new G4LogicalVolume(ScintBox, SiliconMat,"ScintL",0,0,0);
	ScintL->SetVisAttributes(ThreeVisAtt);
    
    G4double scintgap=(ScintThick+fBB34PCB_HalfThickness)*1.1;
    G4ThreeVector scintoffset(0,scintgap*cos(DetDipAngle),scintgap*sin(DetDipAngle));
    G4ThreeVector scintslide(0,-(0.5*mm+ScintD)*sin(DetDipAngle),(0.5*mm+ScintD)*cos(DetDipAngle));
    
    G4RotationMatrix* rotate4 = new G4RotationMatrix;
    rotate4->rotateX(-DetDipAngle);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,detpos+scintoffset+scintslide, ScintL,tt.str(), expHallLog,false,0);
    
    std::stringstream TT;
    TT<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,detpos+scintoffset-scintslide, ScintL,TT.str(), expHallLog,false,0);
    
}


void ApparatusDICE::BuildPlaceFlatOrange(G4LogicalVolume* expHallLog,G4double Zbar){
	G4RotationMatrix rotZbar;
    rotZbar.rotateZ(-Zbar);	

	G4double 	Orange_BeamShieldSep=10*mm;
	G4double 	Orange_BeamDetY=75*mm;
	G4double 	Orange_MagZ=55*mm;
	
	if(fUseA)Orange_BeamShieldSep=fAdjLengthA;
	if(fUseB)Orange_BeamDetY=fAdjLengthB;
	if(fUseE)Orange_MagZ=fAdjLengthE;
	
	G4double 	Orange_ShieldHalfHypo=Orange_BeamShieldSep/((Orange_BeamDetY/(fBB34Chip_HalfWidth+2*mm))-1);
	G4double 	Orange_ShieldHalfWidth=sqrt(pow(Orange_ShieldHalfHypo,2)/2);
	G4double 	Orange_ShieldHalfX=(fBB34Chip_HalfLength/Orange_BeamDetY)*(Orange_ShieldHalfHypo*2+Orange_BeamShieldSep);
	G4double 	Orange_ShieldY=Orange_BeamShieldSep+Orange_ShieldHalfHypo;
	
	G4double 	Orange_ShieldDetGap=Orange_BeamDetY-Orange_BeamShieldSep-Orange_ShieldHalfHypo*2;
	
				Orange_MagZ-=Orange_ShieldHalfHypo;
	G4double 	Orange_MagHalfThick=3*mm;
	
    G4cout<<G4endl<<G4endl<<"Flat Orange Configuration: ";
    G4cout<<G4endl<<"Beam Shield Distance "<<Orange_BeamShieldSep;
    G4cout<<G4endl<<"Beam Detector Distance "<<Orange_BeamDetY;
    G4cout<<G4endl<<"Detector Total Height "<<Orange_ShieldHalfHypo*2;
    G4cout<<G4endl<<"Shield Detector Distance "<<Orange_ShieldDetGap;
    G4cout<<G4endl<<G4endl;
	
	// Place basic shield detector

	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	if(!fRemoveShield){
		G4Box* ShieldBox = new G4Box("Box1", Orange_ShieldHalfX, Orange_ShieldHalfWidth, Orange_ShieldHalfWidth); 
		G4LogicalVolume *ShieldBoxL = new G4LogicalVolume(ShieldBox, mMaterial,"ShieldBoxL_FORBID",0,0,0);
		G4RotationMatrix* rotate1 = new G4RotationMatrix;
		rotate1->rotateZ(Zbar);
		rotate1->rotateX(45*deg);
		ShieldBoxL->SetVisAttributes(OneVisAtt);
		new G4PVPlacement(rotate1,rotZbar*G4ThreeVector(0,-Orange_ShieldY,0), ShieldBoxL,"Shield", expHallLog,false,0); 
	}
	
	// Place BB34 detector
	
	G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateZ(Zbar);
    rotate3->rotateZ(90*deg);
    rotate3->rotateY(90*deg);
    new G4PVPlacement(rotate3,rotZbar*G4ThreeVector(0,-Orange_BeamDetY,0), BuildBB34(),"BB34Detector", expHallLog,false,0);
	
	// Build Mag Shapes
	
	std::vector<G4TwoVector> PCBPolygon(4);
	
	PCBPolygon[0].set(0,Orange_ShieldHalfHypo*0.2);
	PCBPolygon[1].set(Orange_MagZ,Orange_ShieldHalfHypo);
	PCBPolygon[2].set(Orange_MagZ,-(Orange_ShieldHalfHypo+Orange_ShieldDetGap*0.5));
	PCBPolygon[3].set(0,-Orange_ShieldHalfHypo*0.2);
	
	PCBPolygon[0].set(0,Orange_ShieldHalfHypo*0.6);
	PCBPolygon[1].set(Orange_MagZ,Orange_ShieldHalfHypo*0.6);
	PCBPolygon[2].set(Orange_MagZ,-Orange_ShieldHalfHypo);
	PCBPolygon[3].set(0,-Orange_ShieldHalfHypo*0.6);

	if(fUseC||fUseD){
		PCBPolygon[0].set(0,fAdjLengthC);
		PCBPolygon[1].set(Orange_MagZ,fAdjLengthD);
		PCBPolygon[2].set(Orange_MagZ,-fAdjLengthD);
		PCBPolygon[3].set(0,-fAdjLengthC);
	}

	G4VSolid* fFieldBox = new G4ExtrudedSolid("FieldVolBox", PCBPolygon, Orange_ShieldHalfX, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
	G4VSolid* fMagBox = new G4ExtrudedSolid("MagBox", PCBPolygon, Orange_MagHalfThick, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);

	// Place Magnetic Field Volumes 
	
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	
	G4UniformMagField* magField1 = new G4UniformMagField(rotZbar*G4ThreeVector(fFieldStength,0.,0.));
	G4UniformMagField* magField2 = new G4UniformMagField(rotZbar*G4ThreeVector(-fFieldStength,0.,0.));
    G4FieldManager* localFieldManager1=new G4FieldManager(magField1);
    G4FieldManager* localFieldManager2=new G4FieldManager(magField2);
    localFieldManager1->CreateChordFinder(magField1);
    localFieldManager2->CreateChordFinder(magField2);
//     fFieldVolume->SetFieldManager(localFieldManager1, true);

	G4LogicalVolume* fFieldVolume1 = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog1", localFieldManager1, 0, 0);
    fFieldVolume1->SetVisAttributes(G4VisAttributes::Invisible); 
	G4RotationMatrix* rotmag1 = new G4RotationMatrix;
    rotmag1->rotateZ(Zbar);
    rotmag1->rotateY(90*deg);
    new G4PVPlacement(rotmag1,rotZbar*G4ThreeVector(0,-Orange_ShieldY,Orange_ShieldHalfHypo),fFieldVolume1,"Field1",expHallLog,false,0);
	
	G4LogicalVolume* fFieldVolume2 = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog2", localFieldManager2, 0, 0);
    fFieldVolume2->SetVisAttributes(G4VisAttributes::Invisible); 
	G4RotationMatrix* rotmag2 = new G4RotationMatrix;	
    rotmag2->rotateZ(Zbar);	
    rotmag2->rotateY(-90*deg);
    new G4PVPlacement(rotmag2,rotZbar*G4ThreeVector(0,-Orange_ShieldY,-Orange_ShieldHalfHypo),fFieldVolume2,"Field2",expHallLog,false,0);
	
	// Magnetic Solids
	G4double 	MPX=(Orange_ShieldHalfX+Orange_MagHalfThick);
	G4LogicalVolume *MagBoxL = new G4LogicalVolume(fMagBox, mMaterial,"MagBoxL_FORBID",0,0,0);
    new G4PVPlacement(rotmag1,rotZbar*G4ThreeVector(MPX,-Orange_ShieldY,Orange_ShieldHalfHypo), MagBoxL,"Mag1", expHallLog,false,0); 
    new G4PVPlacement(rotmag1,rotZbar*G4ThreeVector(-MPX,-Orange_ShieldY,Orange_ShieldHalfHypo), MagBoxL,"Mag2", expHallLog,false,0); 
    new G4PVPlacement(rotmag2,rotZbar*G4ThreeVector(MPX,-Orange_ShieldY,-Orange_ShieldHalfHypo), MagBoxL,"Mag3", expHallLog,false,0); 
    new G4PVPlacement(rotmag2,rotZbar*G4ThreeVector(-MPX,-Orange_ShieldY,-Orange_ShieldHalfHypo), MagBoxL,"Mag4", expHallLog,false,0); 
	
	
	
}

void ApparatusDICE::BuildPlaceTripleOrange(G4LogicalVolume* expHallLog){

	BuildPlaceFlatOrange(expHallLog,0);
	BuildPlaceFlatOrange(expHallLog,120*deg);
	BuildPlaceFlatOrange(expHallLog,-120*deg);
	
	
}
