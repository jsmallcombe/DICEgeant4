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

    fRemoveShield=false;

    
    OneVisAtt = new G4VisAttributes(G4Colour(PB_COL)); // Red
    TwoVisAtt = new G4VisAttributes(G4Colour(SN_COL)); // grey
    ThreeVisAtt = new G4VisAttributes(G4Colour(CU_COL)); // Yellow
    FourVisAtt = new G4VisAttributes(G4Colour(0.2,0.4,0.3)); // Dark Green
    
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
		case 1: BuildPlaceFlatOrange(expHallLog,0); break;
		case 2: BuildPlaceFlatOrange(expHallLog,0);
                BuildPlaceFlatOrange(expHallLog,120*deg);
                BuildPlaceFlatOrange(expHallLog,-120*deg); break;
        
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
    
G4int ApparatusDICE::BuildMicronSiN=0;

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

void ApparatusDICE::BuildPlaceFlatOrange(G4LogicalVolume* expHallLog,G4double Zbar){
	G4RotationMatrix rotZbar;
    rotZbar.rotateZ(-Zbar);	

	G4double 	Orange_MagGapMin=5*mm; //Gap between 2 halves of the magnet 
	G4double 	Orange_MagHalfThick=3*mm; //Perpendicular Thickness of the magnets 
	G4double 	Orange_BeamShieldSep=10*mm;
	G4double 	Orange_BeamDetY=75*mm; // Height of target vs BB34
	G4double 	Orange_ShieldMidBeamSep=20*mm;  // Distance to the shield widest point
	G4double 	Orange_MagZ=60*mm; // Beam extent of magnets
	G4double 	Orange_MagY=30*mm;  // Height of the magnet field region
	G4double 	Orange_MagAng=0*deg; // Flare angle of the magnets for larget Z
	G4double 	Orange_MagMidOffset=0*mm; // Distance between top edge of magnet and widest point of shield
    
	if(fUseA)Orange_MagMidOffset=fAdjLengthA; // Distance between top edge of magnet and widest point of shield
	if(fUseB)Orange_BeamDetY=fAdjLengthB; // Height of target vs BB34
	if(fUseC)Orange_ShieldMidBeamSep=fAdjLengthC;// Distance to the shield widest point
	if(fUseD)Orange_MagY=fAdjLengthD;    // Height of the magnet field region
	if(fUseE)Orange_MagAng=fAdjLengthE;  // Flare angle of the magnets for larget Z
    Orange_MagY*=0.5;
    
    bool StartMagAtShield=true;
    if(Orange_MagY<0){Orange_MagY=abs(Orange_MagY);StartMagAtShield=false;}
    
	G4double 	SafetyGap=2*mm;
	G4double 	Orange_ShieldTanAngle=(fBB34Chip_HalfWidth+SafetyGap)/Orange_BeamDetY;
	G4double 	Orange_ShieldFrontHalfWidth=Orange_ShieldTanAngle*(Orange_BeamShieldSep+SafetyGap);
	G4double 	Orange_ShieldMidDepth=Orange_ShieldMidBeamSep-Orange_BeamShieldSep;
	G4double 	Orange_ShieldMidHalfWidth=Orange_ShieldFrontHalfWidth+Orange_ShieldTanAngle*(Orange_ShieldMidDepth);

    G4double MStart=Orange_MagGapMin;
    if(StartMagAtShield){MStart=Orange_ShieldMidHalfWidth;}
 
	G4double 	Orange_ShieldHalfX=((fBB34Chip_HalfLength+SafetyGap)/Orange_BeamDetY)*(Orange_ShieldMidBeamSep+Orange_MagY+(fBB34Chip_HalfWidth-MStart)*tan(Orange_MagAng)-Orange_MagMidOffset);
   
	
    G4cout<<G4endl<<G4endl<<"Flat Orange Configuration: ";
    G4cout<<G4endl<<"Beam Shield Front Distance "<<Orange_BeamShieldSep;
    G4cout<<G4endl<<"Beam Shield Mid Distance "<<Orange_ShieldMidBeamSep;
    G4cout<<G4endl<<"Beam Detector Distance "<<Orange_BeamDetY;
    G4cout<<G4endl<<"Total Magnet Seperation "<<Orange_ShieldHalfX*2;
    
	// Place basic shield detector

	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
    G4LogicalVolume *ShieldBoxL=0;
    G4VSolid* ShieldBox=0;
    std::vector<G4TwoVector> OrangeShieldPoly(5);
    
	if(!fRemoveShield){
        OrangeShieldPoly[0].set(Orange_ShieldFrontHalfWidth,Orange_ShieldMidDepth);
        OrangeShieldPoly[1].set(Orange_ShieldMidHalfWidth,0);
        OrangeShieldPoly[2].set(0,-Orange_ShieldMidHalfWidth);
        OrangeShieldPoly[3].set(-Orange_ShieldMidHalfWidth,0);
        OrangeShieldPoly[4].set(-Orange_ShieldFrontHalfWidth,Orange_ShieldMidDepth);
        
        ShieldBox = new G4ExtrudedSolid("Box1", OrangeShieldPoly, Orange_ShieldHalfX, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
		ShieldBoxL = new G4LogicalVolume(ShieldBox, mMaterial,"ShieldBoxL_FORBID",0,0,0);
		G4RotationMatrix* rotate1 = new G4RotationMatrix;
		rotate1->rotateZ(Zbar);
		rotate1->rotateY(90*deg);
		ShieldBoxL->SetVisAttributes(OneVisAtt);
		new G4PVPlacement(rotate1,rotZbar*G4ThreeVector(0,-Orange_ShieldMidBeamSep,0), ShieldBoxL,"Shield", expHallLog,false,0); 
	}
	
	// Place BB34 detector
	
	G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateZ(Zbar);
    rotate3->rotateZ(90*deg);
    rotate3->rotateY(90*deg);
    new G4PVPlacement(rotate3,rotZbar*G4ThreeVector(0,-Orange_BeamDetY,0), BuildBB34(),"BB34Detector", expHallLog,false,0);
	
	// Place Scintilators
    
    G4double ScintThick=1*mm; //half
    G4double ScintD=20*mm; //half
    G4Material* SiliconMat = G4Material::GetMaterial(fWaferMaterial);
    G4Box* ScintBox = new G4Box("ScintBox", ScintD, ScintThick, ScintD); 
    G4LogicalVolume *ScintL = new G4LogicalVolume(ScintBox, SiliconMat,"ScintL",0,0,0);
	ScintL->SetVisAttributes(ThreeVisAtt);
    
    G4double scintgap=(ScintThick+fBB34PCB_HalfThickness)*1.1;
    
    G4RotationMatrix* rotate4 = new G4RotationMatrix;
    rotate4->rotateZ(Zbar);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-Orange_BeamDetY-scintgap,-(0.5*mm+ScintD)), ScintL,tt.str(), expHallLog,false,0);
    
    std::stringstream TT;
    TT<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-Orange_BeamDetY-scintgap,(0.5*mm+ScintD)), ScintL,TT.str(), expHallLog,false,0);
    
    
	// Build Mag Shapes
    
	std::vector<G4TwoVector> MagPolygon(4);
	
	MagPolygon[0].set(MStart,Orange_MagY);
	MagPolygon[1].set(Orange_MagZ,Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng));
	MagPolygon[2].set(Orange_MagZ,-(Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng)));
	MagPolygon[3].set(MStart,-Orange_MagY);
    
    for(unsigned int i=0;i<MagPolygon.size();i++)MagPolygon[i]+=G4TwoVector(0,Orange_MagMidOffset);

	G4VSolid* fFieldBox = new G4ExtrudedSolid("FieldVolBox", MagPolygon, Orange_ShieldHalfX, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
	G4VSolid* fMagBox = new G4ExtrudedSolid("MagBox", MagPolygon, Orange_MagHalfThick, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);

    if(!StartMagAtShield&&!fRemoveShield){
        for(unsigned int i=0;i<OrangeShieldPoly.size();i++)OrangeShieldPoly[i]*=1.01;
        G4ExtrudedSolid* CutBox = new G4ExtrudedSolid("CutBox1", OrangeShieldPoly, Orange_ShieldHalfX*1.1, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
        fFieldBox = new G4SubtractionSolid("FieldVolBoxCut", fFieldBox, CutBox,0,G4ThreeVector(0,0,0));
    }
    
	// Place Magnetic Field Volumes 
	
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
    
	G4double 	MPX=(Orange_ShieldHalfX+Orange_MagHalfThick);
    
	G4LogicalVolume *MagBoxL = new G4LogicalVolume(fMagBox, mMaterial,"MagBoxL_FORBID",0,0,0);
    
    for(int i=-1;i<2;i+=2){
        G4UniformMagField* magField = new G4UniformMagField(rotZbar*G4ThreeVector(i*fFieldStength,0.,0.));
        G4FieldManager* localFieldManager=new G4FieldManager(magField);
        localFieldManager->CreateChordFinder(magField);
        G4LogicalVolume* fOrangeFieldVolume = new G4LogicalVolume(fFieldBox, matWorld, "FieldBoxLog", localFieldManager, 0, 0);
        fOrangeFieldVolume->SetVisAttributes(G4VisAttributes::Invisible); 
        
        G4RotationMatrix* rotmag = new G4RotationMatrix;
        rotmag->rotateZ(Zbar);
        rotmag->rotateY(i*90*deg);
        new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(0,-Orange_ShieldMidBeamSep,0),fOrangeFieldVolume,"Field",expHallLog,false,0);
        
        for(int j=-1;j<2;j+=2){
            new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(j*MPX,-Orange_ShieldMidBeamSep,0), MagBoxL,"Mag", expHallLog,false,0); 
        }
    }

//     fOrangeFieldVolume->SetFieldManager(localFieldManager1, true);
	
}
