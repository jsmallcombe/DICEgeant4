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



G4double ApparatusDICE::fFieldStength = 0.065*CLHEP::tesla;	
//

ApparatusDICE::ApparatusDICE()//parameter chooses which lens is in place.
{
	// Materials
	fBlockerMaterial = "Tungsten"; 
	fMagnetMaterial = "Iron"; 
	fWaferMaterial   = "Silicon";
    
	fFieldOn=true;
	
	// Shoe box measurments
	fMagnetHeight= 104*CLHEP::mm;
	fMagnetWidth= 260*CLHEP::mm;
	fMagnetDepth= 150*CLHEP::mm;
	fMagnetThickness= 10*CLHEP::mm;
	
	fTargetMagnetDistance= 10*CLHEP::mm;
	
    fDetectorThickness= 5*CLHEP::mm;
	fDetectorWidth= 80*CLHEP::mm;
	fDetectorHeight= 80*CLHEP::mm;
	fSegX= 10;
	fSegY= 4;
	fGuardRingWidth= 4*CLHEP::mm;
    
	fDetPlaceAngle = 60*CLHEP::deg;
	fDetPlaceRadius= 30*CLHEP::mm;
	fDetPlaceRadius+=fDetectorWidth/2.;

	// Helios measurments
	fHMagnetOuterR= 90*CLHEP::mm;
	fHMagnetInnerR= 80*CLHEP::mm;
	fHMagnetDepth= 300*CLHEP::mm;
	fHMagTargetOffset= 5*CLHEP::mm;
	
	fHTargDetOffset= 30*CLHEP::mm;
	fHDetRad= 15*CLHEP::mm;
	fHDetLength= 150*CLHEP::mm;
	fHSegZ=10;

	
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
	fRSBlock0= 10*CLHEP::mm;
	fRSDetR= 15*CLHEP::mm;
	fRSInnerR= 10*CLHEP::mm;
	
	// Basic Test Detectors
	
	
} // end ApparatusDICE

//////////////////////////////////////////////////
// delete ApparatusDICE
/////////////////////////////////////////////////

ApparatusDICE::~ApparatusDICE()
{
} // end ~ApparatusDICE

///////////////////////////////////////
// build and place components        //
///////////////////////////////////////

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
		default: BuildPlaceBasicTest(expHallLog);
	}
	
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
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(), fDetectorLogical, "SiSegmentPhys0_0",  expHallLog, false, 0);
	
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
	
	BuildRadialDetector(); // Build fDetectorLog
	
    // Place fDetectorLog in fFieldVolume
    G4double rotY=90*CLHEP::deg-fDetPlaceAngle;
    G4double Xplace=sin(fDetPlaceAngle)*fDetPlaceRadius;
    G4double Zplace=cos(fDetPlaceAngle)*fDetPlaceRadius-(fTargetMagnetDistance+fMagnetDepth/2.);
    
    Xplace+=cos(fDetPlaceAngle)*(fDetectorThickness/2.0);
    Zplace+=-sin(fDetPlaceAngle)*(fDetectorThickness/2.0);
	
	G4ThreeVector move(Xplace, 0, Zplace);   
	G4RotationMatrix* rotate = new G4RotationMatrix;
    rotate->rotateY(rotY); 
    
	new G4PVPlacement(rotate,move, fDetectorLog, "SiLiDetActive",  fFieldVolume, false, 0);	
	
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
	
}

void ApparatusDICE::BuildRadialDetector(){
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);

	// Define rotation and movement objects
	G4RotationMatrix* rotate = new G4RotationMatrix;

	// Detector crystal mother
    G4Box* siDetMother = new G4Box("siDetMother", fDetectorWidth/2., fDetectorHeight/2., fDetectorThickness/2.);
	fDetectorLog = new G4LogicalVolume(siDetMother, matWorld, "fDetActiveLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);

	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);

	//Guard Rings
	
    G4Box* siGuardOuter = new G4Box("siGuardOuter", (fDetectorWidth)/2. -0.1*mm, (fDetectorHeight)/2. -0.1*mm, fDetectorThickness/2. -0.1*mm);
	G4double halfwidth=fDetectorWidth/2. -fGuardRingWidth;
	G4double halfheight=fDetectorHeight/2. -fGuardRingWidth;
    G4Box* siGuardInner = new G4Box("siGuardOuter", halfwidth +0.1*mm, halfheight +0.1*mm, fDetectorThickness/2.);
	G4VSolid* siGuard = new G4SubtractionSolid("siGuard", siGuardOuter, siGuardInner,0,G4ThreeVector(0,0,0));
    G4LogicalVolume* SiGuardLog =new G4LogicalVolume(siGuard, material, "SiGuardLog", 0,0,0);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,0), SiGuardLog,"siGuardRing",  fDetectorLog, false, 0);
	
    G4Box* siDetSegment = new G4Box("siSeg", halfwidth/fSegX -0.1*mm, halfheight/fSegY -0.1*mm, fDetectorThickness/2. -0.1*mm);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	for(int x=0;x<fSegX;x++){
        G4double xx=-halfwidth+(halfwidth*2/fSegX)*(0.5+x);
        
        for(int y=0;y<fSegY;y++){
            G4double yy=-halfheight +(halfheight*2/fSegY)*(0.5+y);
            std::stringstream temp;
			temp<<"SiSegmentPhys_"<<x<<"_"<<y;
			new G4PVPlacement(rotate,G4ThreeVector(xx,yy,0), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);
		}	
	}
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
	sVisAtt->SetVisibility(false);
    
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
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
	
    G4Tubs* siDetSegment = new G4Tubs("siSeg",0, fHDetRad, fHDetLength/(fHSegZ*2.) -0.1*mm, 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);
  
	G4double zz=fHDetLength/(fHSegZ*2.)-fHDetLength/2.;
	for(int x=0;x<fHSegZ;x++){
		std::stringstream temp;
		temp<<"SiSegmentPhys_"<<x<<"_"<<0;
		new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fSiSegmentLog, temp.str(),  fDetectorLog, false, 0);	
		zz+=fHDetLength/fHSegZ;
	}
}


void ApparatusDICE::BuildPlaceSuperTube(G4LogicalVolume* expHallLog){
		//// fFieldVolume is the logical volume over which the field is defined
	//// The magnet yoke and detector are placed in fFieldVolume
	//// fFieldVolume is the placed in world volume
    
    G4Tubs* fFieldBox = new G4Tubs("FieldBox", 0,0.1*mm+fSTMagnetOuterR,fSTMagnetDepth,0 , 360*CLHEP::deg);
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	fFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", 0, 0, 0);
    fFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 

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
	BuildRecoilShadowUnit();
	
	G4Material* matblock = G4Material::GetMaterial(fBlockerMaterial);
	G4Tubs* blocktub = new G4Tubs("blocktub0",fRSInnerR,fRSDetR, fRSBlock0*0.5, 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSubBlock =new G4LogicalVolume(blocktub, matblock, "Block0Log", 0,0,0);
	
	G4RotationMatrix* rotate = new G4RotationMatrix;
	
	new G4PVPlacement(rotate,G4ThreeVector(0,0,fRSOffset+fRSBlock0*0.5), fSubBlock, "Block0",  expHallLog, false, 0);	

	G4double zz=fRSOffset+fRSBlock0+fRSDetR;
	for(int x=0;x<fHSegZ;x++){
		std::stringstream temp;
		temp<<"SiSegmentPhys_"<<x<<"_"<<0;
		new G4PVPlacement(rotate,G4ThreeVector(0,0,zz), fDetectorLog, temp.str(),  expHallLog, false, 0);	
		zz+=fRSDetR*2;
	}	
}
// 

void ApparatusDICE::BuildRecoilShadowUnit(){
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
	G4VisAttributes* vis_att_hid = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att_hid->SetVisibility(false);	

	G4Box* fUnitBox = new G4Box("fUnitBox",fRSDetR*2,fRSDetR*2,fRSDetR);
	fDetectorLog = new G4LogicalVolume(fUnitBox, matWorld, "fDetUnitLog", 0,0,0);  
	fDetectorLog->SetVisAttributes(vis_att_hid);

	G4Material* matblock = G4Material::GetMaterial(fBlockerMaterial);

	G4Tubs* blocktub = new G4Tubs("blocktub",fRSInnerR,fRSDetR, 1*mm/(2.), 0 , 360*CLHEP::deg);
    G4LogicalVolume* fSubBlock =new G4LogicalVolume(blocktub, matblock, "BlockRingLog", 0,0,0);

	G4RotationMatrix* rotate = new G4RotationMatrix;	

	new G4PVPlacement(rotate,G4ThreeVector(0,0,-fRSDetR/3.), fSubBlock,"BlockRing",  fDetectorLog, false, 0);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,fRSDetR/3.), fSubBlock,"BlockRing",  fDetectorLog, false, 0);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,fRSDetR), fSubBlock,"BlockRing",  fDetectorLog, false, 0);	
		
	
	G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
	vis_att->SetVisibility(true);  
    G4Material* material = G4Material::GetMaterial(fWaferMaterial);
	
   G4Tubs* siDetSegment = new G4Tubs("siSeg",0, fRSDetR, 5*mm/2., 0 , 360*CLHEP::deg);
   G4LogicalVolume* fSiSegmentLog =new G4LogicalVolume(siDetSegment, material, "fSiSegmentLog", 0,0,0);	
   fSiSegmentLog->SetVisAttributes(vis_att);

	for(int x=0;x<4;x++){

		G4ThreeVector place=G4ThreeVector(0,0,fRSDetR+2.5*mm);
		place.rotateY(90*CLHEP::deg);
		place.rotateZ(x*90*CLHEP::deg);
		
		G4RotationMatrix* r=new G4RotationMatrix;
		r->rotateY(90*CLHEP::deg); 
		r->rotateX(x*90*CLHEP::deg); 
		
		new G4PVPlacement(r,place, fSiSegmentLog,"fSiSegmentPhys",  fDetectorLog, false, 0);	
		
	}
	
}


