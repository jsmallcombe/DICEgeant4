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
	fBlockerMaterial = "Peek"; //G4_C
	fMagnetMaterial = "NdFeB"; 
	fYokeMaterial = "Iron"; 
	fWaferMaterial   = "Silicon";
	fShieldMaterial   = "WTa";
// 	fShieldMaterial   = "Hevimetal";
	fScintMaterial   = "EJ212";
// 	fScintMaterial   = "G4_BGO";
	fXrayMaterial   = "Brass"; //Copper //Aluminium
	fKaptonMaterial   = "Kapton";
	fTargetMaterial   = "Nickle";

    fRemoveShield=false;
    fAddBlocker=false;
	fUniformFieldOn=true;

	
    OneVisAtt = new G4VisAttributes(G4Colour(PB_COL)); // Red
    TwoVisAtt = new G4VisAttributes(G4Colour(SN_COL)); // grey
    ThreeVisAtt = new G4VisAttributes(G4Colour(CU_COL)); // Yellow
    FourVisAtt = new G4VisAttributes(G4Colour(0.2,0.4,0.3)); // Dark Green
    FiveVisAtt = new G4VisAttributes(G4Colour(0.941,0.698,0.094)); // Kapton "orangish"
	vis_att_hid = new G4VisAttributes(G4VisAttributes::Invisible);
    
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
    
    
	//////////////////////////////////////////////////////////////////////
	////// Optionally Controlled by DetectorConstruction class ///////
	//////////////////////////////////////////////////////////////////////

	fAdjNumber=4;
	
	
	Orange_BeamShieldSep=10*mm;
	SetNmLstD.push_back("BeamShieldSep");
	SetPtrLstD.push_back(&Orange_BeamShieldSep);
	
	Orange_ShieldMidBeamSep=20*mm;  // Distance to the shield widest pointgb
	SetNmLstD.push_back("ShieldMidBeamSep");
	SetPtrLstD.push_back(&Orange_ShieldMidBeamSep);
	
	    
	Orange_MagMidOffset=30*mm; // Distance between center line of magnet and target
	SetNmLstD.push_back("MagMidOffset");
	SetPtrLstD.push_back(&Orange_MagMidOffset);
	
	Orange_BeamDetY=60*mm; // Height of target vs BB34
	SetNmLstD.push_back("BeamDetY");
	SetPtrLstD.push_back(&Orange_BeamDetY);
	
	Orange_MagY=20*mm;  // Total vertical coverage/height of the magnet field region (variable gets halved after input)
	SetNmLstD.push_back("MagY");
	SetPtrLstD.push_back(&Orange_MagY);
	
	Orange_MagAng=0*deg; // Flare angle of the magnets
	SetNmLstD.push_back("MagAng");
	SetPtrLstD.push_back(&Orange_MagAng);

	Orange_MagZ=60*mm; // Beam extent of magnets
	SetNmLstD.push_back("MagZ");
	SetPtrLstD.push_back(&Orange_MagZ);
	
	Orange_MagHalfThick=2.5*mm; // Perpendicular Half Thickness of the magnets 
	SetNmLstD.push_back("MagHalfThick");
	SetPtrLstD.push_back(&Orange_MagHalfThick);
	
	Orange_MagGapMinHalf=-1; // Min Half Gap between magnet halves (at side of shield)
	SetNmLstD.push_back("MagGapMinHalf");
	SetPtrLstD.push_back(&Orange_MagGapMinHalf);
	
	PoleSepHalf=-1;
	SetNmLstD.push_back("PoleSepHalf");
	SetPtrLstD.push_back(&PoleSepHalf);
	
	PhiBlockThickness=-1;
	SetNmLstD.push_back("PhiBlockThickness");
	SetPtrLstD.push_back(&PhiBlockThickness);
	
	SetParamB("UseNewShield",true);
	
// 	SetParamD("ShieldFrontY",10);
// 	SetParamD("ShieldFrontZ",8.43);
// 	SetParamD("ShieldMidY",19.7);
// 	SetParamD("ShieldMidZ",15);
	SetParamD("ShieldFrontY",10);
	SetParamD("ShieldFrontZ",8.435);
	SetParamD("ShieldMidY",20);
	SetParamD("ShieldMidZ",16.8);

	SetParamB("IncludeBB34Clamp",true);
	SetParamD("BB34ClampX",20);
	SetParamD("BB34ClampZ",40);
	SetParamD("BB34ClampY",0.75);
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	
} // end ApparatusDICE constructor

//////////////////////////////////////////////////
// delete ApparatusDICE
/////////////////////////////////////////////////

ApparatusDICE::~ApparatusDICE()
{
	//we should delete stuff properly, but who cares program is ending
} // end ~ApparatusDICE

///////////////////////////////////////

void ApparatusDICE::SetParamD(G4String str,G4double input){
	for(unsigned int i=0;i<SetNmLstD.size();i++){
		if(SetNmLstD[i]==str){
			*SetPtrLstD[i]=input;
			return;
		}
	}
	SetNmLstD.push_back(str);
	SetPtrLstD.push_back(new G4double(input));

}
void ApparatusDICE::SetParamI(G4String str,G4int input){
	for(unsigned int i=0;i<SetNmLstI.size();i++){
		if(SetNmLstI[i]==str){
			*SetPtrLstI[i]=input;
			return;
		}
	}
	SetNmLstI.push_back(str);
	SetPtrLstI.push_back(new G4int(input));
}
void ApparatusDICE::SetParamB(G4String str,G4bool input){
	for(unsigned int i=0;i<SetNmLstB.size();i++){
		if(SetNmLstB[i]==str){
			*SetPtrLstB[i]=input;
			return;
		}
	}
	SetNmLstB.push_back(str);
	SetPtrLstB.push_back(new G4bool(input));
}

G4double ApparatusDICE::gD(G4String str){
	for(unsigned int i=0;i<SetNmLstD.size();i++){
		if(SetNmLstD[i]==str){
			return *SetPtrLstD[i];
		}
	}
	return 0;
}
G4int ApparatusDICE::gI(G4String str){
	for(unsigned int i=0;i<SetNmLstI.size();i++){
		if(SetNmLstI[i]==str){
			return *SetPtrLstI[i];
		}
	}
	return 0;
}
G4bool ApparatusDICE::gB(G4String str){
	for(unsigned int i=0;i<SetNmLstB.size();i++){
		if(SetNmLstB[i]==str){
			return *SetPtrLstB[i];
		}
	}
	return 0;
}


void ApparatusDICE::ListParam(){
	std::cout<<std::endl<<"DiceDoubles :"<<std::flush;
	G4cout<<G4endl<<"DiceDoubles :";
	for(unsigned int i=0;i<SetNmLstD.size();i++){
		std::cout<<std::endl<<SetNmLstD[i]<<" = "<<*SetPtrLstD[i]<<std::flush;
		G4cout<<G4endl<<SetNmLstD[i]<<" = "<<*SetPtrLstD[i];
	}
	std::cout<<std::endl<<"DiceInts :"<<std::flush;
	G4cout<<G4endl<<"DiceInts :";
	for(unsigned int i=0;i<SetNmLstI.size();i++){
		std::cout<<std::endl<<SetNmLstI[i]<<" = "<<*SetPtrLstI[i]<<std::flush;
		G4cout<<G4endl<<SetNmLstI[i]<<" = "<<*SetPtrLstI[i];
	}
	std::cout<<std::endl<<"DiceBools :"<<std::flush;
	G4cout<<G4endl<<"DiceBools :";
	for(unsigned int i=0;i<SetNmLstB.size();i++){
		std::cout<<std::endl<<SetNmLstB[i]<<" = "<<*SetPtrLstB[i]<<std::flush;
		G4cout<<G4endl<<SetNmLstB[i]<<" = "<<*SetPtrLstB[i];
	}
	std::cout<<std::endl;
	G4cout<<G4endl;
}


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
	
	G4LogicalVolume* World=expHallLog;

	switch(abs(Opt)) {
		case 1: BuildPlaceFlatOrange(World,0); break;
		case 2: BuildPlaceFlatOrange(World,0);
                BuildPlaceFlatOrange(World,180*deg);break;
		case 3: BuildPlaceFlatOrange(World,0);
                BuildPlaceFlatOrange(World,120*deg);
                BuildPlaceFlatOrange(World,-120*deg); break;
        
		default: BuildPlaceBasicTest(World);
	}
	

	if(Opt<0){
		new G4PVPlacement(0,G4ThreeVector(), BuildISOK160Cross(),"ISOK", World,false,0); 

		BuildPlaceTargetLadder(World);
	}
	
	ListParam();
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
	G4Box* fDetectorSolid = new G4Box("fDetectorSolid", 15*mm, 15*mm, 15*mm);

	// Create a G4LogicalVolume a "logical", combines a "solid" with material properies and other options (not used here) describing how it should be treated during the simulation. Still only a blueprint, does not exist in the simulated world
	// Inputs : Solid (pointer), Material, name, 3 options not used here (0,0,0)
	G4LogicalVolume* fDetectorLogical = new G4LogicalVolume(fDetectorSolid, material, "fDetectorLogical", 0, 0, 0);
	
	// Create a G4PVPlacement, a type of "physical" which means placing a copy of our logical blueprints inside another logical volume. When a logical volume is placed inside the "world volume" that volume (and any contained within it) exist in the simulated world
	// Note : The actual instance of the object is not used further in this function, we only need it to be created. It now exists in memory and at the moment of creation information was sent to it's "mother volume".
	// Inputs : Rotation (pointer), Rotation (local), Material (pointer), name, mothervolume, 2 options not used here (false,0)
	// Mother volume is the "logical" within which this physical should occur, for instance we can but segments inside a detector
	// All physicals with name "SiSegmentPhys#_#" where # are number will be treated as detecors for the purposes of output, where the numbers # are used to specify the volumes
	// The exact definition of the special names can befound in DetectorConstruction::ParseVolumeName(G4String volumeName) of DetectorConstruction.cc
	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(0,0,0*mm), fDetectorLogical, "SiSegmentPhys0_0",  expHallLog, false, 0);
	
	
// // 	G4Box* bx = new G4Box("bx", 150*mm, 150*mm, 150*mm);
// // 	G4LogicalVolume* lg = new G4LogicalVolume(bx, G4Material::GetMaterial("Vacuum"), "lg", 0,0,0); 
// // // 	lg->SetVisAttributes(vis_att_hid); 
// // 	new G4PVPlacement(new G4RotationMatrix(),G4ThreeVector(), lg, "wrldbx",  expHallLog, false, 0);
}
    
    
void ApparatusDICE::BuildPlaceTargetLadder(G4LogicalVolume* expHallLog){
	
		G4Box* FoilTargBox = new G4Box("FoilTargBox", 5*mm,5*mm,1*um);
		G4Material* TargMat = G4Material::GetMaterial(fTargetMaterial);
		G4LogicalVolume *FoilTargLog = new G4LogicalVolume(FoilTargBox, TargMat,"FoilTarg",0,0,0);
		FoilTargLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.3,0.6,0.1)));
		new G4PVPlacement(0,G4ThreeVector(0,0,0), FoilTargLog,"TargetFoil", expHallLog,false,0); 
		
		
		G4Box* FrameBox = new G4Box("FrameBox", 30*mm,10*mm,1.5*mm);
		G4Box* FrameHoleBox = new G4Box("FrameHoleBox", 5*mm,5*mm,2*mm);
		G4VSolid* Frame = new G4SubtractionSolid("Frame", FrameBox, FrameHoleBox,0,G4ThreeVector(0,0,0));

		G4Material* FrameMat = G4Material::GetMaterial("G4_STAINLESS-STEEL");
		G4LogicalVolume *FrameLog = new G4LogicalVolume(Frame, FrameMat,"FrameLog",0,0,0);
		new G4PVPlacement(0,G4ThreeVector(0,0,0), FrameLog,"TargFrame", expHallLog,false,0); 
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
    G4LogicalVolume* ChipMetalLog = new G4LogicalVolume(ChipAl, G4Material::GetMaterial("Aluminium"), "ChipMetalLog", 0,0,0);
	ChipMetalLog->SetVisAttributes(vis_att_hid);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,fBB34Chip_HalfThickness+fBB34Chip_Al*0.5),ChipMetalLog, "SiSegmentPhys0_4",  ChipMotherLog, false, false,true);
	new G4PVPlacement(rotate,G4ThreeVector(0,0,-fBB34Chip_HalfThickness-fBB34Chip_Al*0.5),ChipMetalLog, "SiSegmentPhys0_4",  ChipMotherLog, false, false,true);
	
	// Metal given a detector name so that we can still record low energy electrons that wouldnt actually reach sensitive silicon
	
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
	
	//// Set Materials ////
	
	G4Material* BlockerMat = G4Material::GetMaterial(fBlockerMaterial);
	G4Material *mMaterial = G4Material::GetMaterial(fMagnetMaterial);
	G4Material *YokeMat = G4Material::GetMaterial(fYokeMaterial);
	G4Material *ShieldInMat = G4Material::GetMaterial(fShieldMaterial);
    G4Material* ScintMat = G4Material::GetMaterial(fScintMaterial);
    G4Material* XrayMat = G4Material::GetMaterial(fXrayMaterial);
    G4Material* KapMat = G4Material::GetMaterial(fKaptonMaterial);
	
	//// Initilisation of general parameters ////
	
	G4RotationMatrix rotZbar;
    rotZbar.rotateZ(-Zbar);	

	G4double 	SafetyGap=2*mm;

	G4cout<<"DICE SetableParameters :"<<G4endl;
	std::cout<<"DICE SetableParameters :"<<std::endl;
	for(unsigned int i=0;i<SetNmLstD.size();i++){
		G4cout<<SetNmLstD[i]<<" "<<*SetPtrLstD[i]<<G4endl;
		std::cout<<SetNmLstD[i]<<" "<<*SetPtrLstD[i]<<std::endl;
	}
	G4cout<<"Fin."<<G4endl;
	std::cout<<"Fin."<<std::endl;
	
	G4double 	Orange_YokeHalfThick=4*mm; // Perpendicular Half Thickness of the Yoke 
	G4double 	Orange_TargetWardingY=12.5*mm; // Half Gap Target Ladder Needs
	G4double 	Orange_TargetWardingZ=2.5*mm; // Half Gap Target Ladder Needs (Beamward)
    G4double ScintThick=1*mm; //half
    
	G4double CrossBlockHalfThickness=1*mm; //Under shield electron blocker
	G4double 	KaptonThickness=0.1*mm; // Abs thickness of kapton covers
    
	//// Parameter Processing ////
	
    G4double Orange_ScintSep=(ScintThick+fBB34PCB_HalfThickness)+2.55*mm; // Distance between midpoint off BB34 and Scint 
	
	bool SetMagCopyNum=(Orange_MagMidOffset<0);
	Orange_MagMidOffset=std::abs(Orange_MagMidOffset);
	
    Orange_MagY*=0.5;  // MAG HEIGHT IS DIVIDED BY TWO AFTER INPUT
    
    Orange_MagY=std::abs(Orange_MagY);
	
	G4double 	Orange_ShieldTanAngle=(fBB34Chip_HalfWidth+SafetyGap)/(Orange_BeamDetY+SafetyGap*2);
	G4double 	Orange_ShieldFrontHalfWidth=Orange_ShieldTanAngle*Orange_BeamShieldSep;
	G4double 	Orange_ShieldMidDepth=Orange_ShieldMidBeamSep-Orange_BeamShieldSep;
	G4double 	Orange_ShieldMidHalfWidth=Orange_ShieldFrontHalfWidth+Orange_ShieldTanAngle*(Orange_ShieldMidDepth);
	G4double 	Orange_ShieldBottomDepth=std::abs(Orange_MagMidOffset)-Orange_ShieldMidBeamSep;
	
	// Fixing unphysics geometry inputs
	if(Orange_ShieldBottomDepth+Orange_ShieldMidBeamSep>Orange_BeamDetY+SafetyGap){
		Orange_ShieldBottomDepth=Orange_BeamDetY-Orange_ShieldMidBeamSep-SafetyGap;
	}
	
	G4double ShieldInsertBottomY=Orange_ShieldBottomDepth+Orange_ShieldMidBeamSep;
	
    G4double MStart=Orange_ShieldMidHalfWidth;
    if(Orange_MagGapMinHalf>0){MStart=Orange_MagGapMinHalf;}
    
    G4double LowestPointOfMagnet=Orange_MagY+Orange_MagMidOffset;
	if(Orange_MagAng>0)LowestPointOfMagnet+=(fBB34Chip_HalfWidth-MStart)*tan(Orange_MagAng);
    
	G4double RecommendedPole=((fBB34Chip_HalfLength+SafetyGap)/(Orange_BeamDetY+SafetyGap*2))*LowestPointOfMagnet;
	if(PoleSepHalf<0){
		PoleSepHalf=RecommendedPole;
	}else{
		G4cout<<G4endl<<"Poll Separation manually set to "<<PoleSepHalf;
		G4cout<<G4endl<<"Recommened Value "<<RecommendedPole;
		std::cout<<std::endl<<"Poll Separation manually set to "<<PoleSepHalf;
		std::cout<<std::endl<<"Recommened Value "<<RecommendedPole<<std::endl;
	}
	
	if(PoleSepHalf<=0){
		PoleSepHalf=10;
		G4cout<<G4endl<<"POLL SEPARATION OVERRIDDEN DUE TO ERROR";
	}
	
	G4double Orange_ShieldHalfX=PoleSepHalf+Orange_MagHalfThick*2;
	
	G4double Orange_InsertHalfX=((fBB34Chip_HalfLength+SafetyGap)/Orange_BeamDetY)*ShieldInsertBottomY;
	if(Orange_ShieldHalfX<Orange_InsertHalfX){
		Orange_InsertHalfX=Orange_ShieldHalfX*0.8;
	}
	if(PoleSepHalf<Orange_InsertHalfX){
		Orange_InsertHalfX=PoleSepHalf;
	}
	
	if(Orange_MagAng<0){
		G4double ShrtMag=MStart+abs(Orange_MagY/tan(Orange_MagAng));
		if(ShrtMag<=Orange_MagZ){
			Orange_MagZ=ShrtMag-SafetyGap;
			G4cout<<G4endl<<"MAGNETS LENGTH OVERRIDDEN DUE TO MAGNET ANGLE";
		}
	}
	
	if(sqrt(pow(PoleSepHalf+Orange_MagHalfThick*2+SafetyGap,2) + pow(Orange_MagZ+SafetyGap,2))>145*mm)
    G4cout<<G4endl<<"MAGNETS TOO LARGE TO FIT INSIDE ISOK160";
	
	
    G4cout<<G4endl<<G4endl<<"Flat Orange Configuration: Beam Axis = Z ";
    G4cout<<G4endl<<"Beam <-> Shield Distance "<<Orange_BeamShieldSep;
    G4cout<<G4endl<<"Beam <-> ShieldCenter Distance "<<Orange_ShieldMidBeamSep;
    G4cout<<G4endl<<"ShieldCenter Z length (Tot.) "<<Orange_ShieldMidHalfWidth*2;
    G4cout<<G4endl<<"Magnet Shield-gap (i.e. Magnet separation along yoke) "<<MStart*2;
    G4cout<<G4endl<<"Shield X Thickness "<<Orange_ShieldHalfX*2;
    G4cout<<G4endl<<"Magnet Pole Separation "<<PoleSepHalf*2;
    G4cout<<G4endl<<"Beam  <-> Detector Distance "<<Orange_BeamDetY;
    G4cout<<G4endl<<"MagnetTop <-> Beam Separation "<<Orange_MagMidOffset-Orange_MagY;
    G4cout<<G4endl<<"MagnetCenterline <-> Beam Separation "<<Orange_MagMidOffset;
    G4cout<<G4endl<<"Lowest Point Of Magnet "<<LowestPointOfMagnet;
    G4cout<<G4endl<<"Magnet Height "<<Orange_MagY*2;
    G4cout<<G4endl<<"Magnet Length "<<Orange_MagZ-MStart;
    G4cout<<G4endl<<"Magnet Z Extent "<<Orange_MagZ;
    G4cout<<G4endl;
	
	
	//////////////////////////////////////
	// Build and Place Shield
	//////////////////////////////////////
	
	// Used outside shield loop
	G4ExtrudedSolid* ShieldCutBox=nullptr;
	
	if(!fRemoveShield){

		if(gB("UseNewShield")){
			
			// Temorary rules which should match current design constraints in fusion 18/4/24
			SetParamD("ShieldFrontZ",(41.5/(Orange_BeamDetY+1.5))*(gD("ShieldFrontY")+2.5));
			SetParamD("ShieldMidY",20);
			SetParamD("ShieldMidZ",((38.5/(Orange_BeamDetY+2.9))*(gD("ShieldMidY")+5))+1.5);

			std::vector<G4TwoVector> TaPolyTot(10);		
			TaPolyTot[0].set(3,-16);
			TaPolyTot[1].set(3,-gD("ShieldFrontY"));
			TaPolyTot[2].set(gD("ShieldFrontZ"),-gD("ShieldFrontY"));
			TaPolyTot[3].set(gD("ShieldMidZ"),-gD("ShieldMidY"));
			TaPolyTot[4].set(gD("ShieldMidZ"),-30);
			
			G4double BrassFrontZ=((16-gD("ShieldFrontY"))*((gD("ShieldMidZ")-gD("ShieldFrontZ"))/(gD("ShieldMidY")-gD("ShieldFrontY"))))+gD("ShieldFrontZ");
			
			G4double BrassMidY=((gD("ShieldMidZ")-1.1)*(4/8.9))-44;
			
			std::vector<G4TwoVector> BrassPolyTot(10);
			BrassPolyTot[0].set(BrassFrontZ,-16);
			BrassPolyTot[1].set(gD("ShieldMidZ"),-gD("ShieldMidY"));
			BrassPolyTot[2].set(gD("ShieldMidZ"),BrassMidY);
			BrassPolyTot[3].set(1.1,-44);
			BrassPolyTot[4].set(1.1,-40);
			for(int i=4;i>=0;i--){
				BrassPolyTot[9-i].set(-BrassPolyTot[i].x(),BrassPolyTot[i].y());
				TaPolyTot[9-i].set(-TaPolyTot[i].x(),TaPolyTot[i].y());
			}
			
			std::vector<G4TwoVector> KapPol(8);	
			KapPol[0].set(TaPolyTot[2].x()+0.01,TaPolyTot[2].y());
			KapPol[1].set(TaPolyTot[3].x()+0.01,TaPolyTot[3].y());
			KapPol[2].set(BrassPolyTot[2].x()+0.01,BrassPolyTot[2].y());
			KapPol[3].set(BrassPolyTot[3].x()+0.01,BrassPolyTot[3].y());
			for(int i=3;i>=0;i--){
				KapPol[7-i].set(KapPol[i].x()+KaptonThickness,KapPol[i].y());
			}
			
			G4double PoleGap=16*mm;
			G4double MagGap=10*mm;
			G4double YokeGap=21*mm;
			G4double TaGap=12.5*mm;
			G4double CutL=20*mm;
			
			G4VSolid* BrassS = new G4ExtrudedSolid("BrassS", BrassPolyTot, YokeGap+5, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
			
			G4Box* CutBoxS = new G4Box("CutBoxS",CutL,CutL,CutL);
			
			for(int i=-1;i<2;i+=2){
				BrassS = new G4SubtractionSolid("BrassS", BrassS, CutBoxS,0,G4ThreeVector(0,-20-CutL,i*(YokeGap+CutL)));//CutForYokes
				for(int j=-1;j<2;j+=2){
					BrassS = new G4SubtractionSolid("BrassS", BrassS, CutBoxS,0,G4ThreeVector(j*(MagGap+CutL),-20-CutL,i*(CutL+TaGap-0.01)));
				}
			}
			
			G4Box* CutBoxT = new G4Box("CutBoxT",CutL,30.01,TaGap+0.01);
			BrassS = new G4SubtractionSolid("BrassS", BrassS, CutBoxT,0,G4ThreeVector(0,0,0));
			
			G4LogicalVolume *BrassLog = new G4LogicalVolume(BrassS, XrayMat,"BrassLog_FORBID",0,0,0);
			BrassLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.6,0.471,0.059)));
			
			G4RotationMatrix* rotateblock = new G4RotationMatrix;
			rotateblock->rotateZ(Zbar);
			rotateblock->rotateY(90*deg);
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,0,0), BrassLog,"XrayBlock", expHallLog,false,0);
			
			G4VSolid* TaS = new G4ExtrudedSolid("TaS", TaPolyTot,TaGap, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
			
			G4LogicalVolume *ShieldBoxL = new G4LogicalVolume(TaS, ShieldInMat,"ShieldBoxL_FORBID",0,0,0);
			ShieldBoxL->SetVisAttributes(OneVisAtt);
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,0,0), ShieldBoxL,"ShieldInsert", expHallLog,false,0); 
			
			G4VSolid* KapS = new G4ExtrudedSolid("kapS", KapPol,TaGap, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
			G4LogicalVolume *XrayCoverLog = new G4LogicalVolume(KapS, KapMat,"KaptonCoverLog_FORBID",0,0,0);
			XrayCoverLog->SetVisAttributes(FiveVisAtt);
			
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,0,0), XrayCoverLog,"XrayBlockCover", expHallLog,false,0);
			G4RotationMatrix* rotate1 = new G4RotationMatrix;
			rotate1->rotateZ(Zbar);
			rotate1->rotateY(-90*deg);
			new G4PVPlacement(rotate1,rotZbar*G4ThreeVector(0,0,0), XrayCoverLog,"XrayBlockCover", expHallLog,false,0);
			
			
			if(fAddBlocker){
				
				G4double HalfBlockGap=0.5*(Orange_BeamDetY-2.95*mm-40);
				G4Box* ElectronBlockerBox = new G4Box("ElectronBlockerBox",fBB34Chip_HalfLength,HalfBlockGap-0.1*mm,CrossBlockHalfThickness);
				G4LogicalVolume *ElectronBlockerLog = new G4LogicalVolume(ElectronBlockerBox, BlockerMat,"ElectronBlockerLog_FORBID",0,0,0);
				ElectronBlockerLog->SetVisAttributes(ThreeVisAtt);
				new G4PVPlacement(new G4RotationMatrix,rotZbar*G4ThreeVector(0,-HalfBlockGap-40,0), ElectronBlockerLog,"ElectronBlocker", expHallLog,false,0);
				
			}
			
			
		}else{
		
			// Define the shape of the insert
			std::vector<G4TwoVector> OrangeShieldPoly(6);		
			OrangeShieldPoly[0].set(Orange_ShieldFrontHalfWidth,Orange_ShieldMidDepth);
			OrangeShieldPoly[1].set(Orange_ShieldMidHalfWidth,0);
	//         OrangeShieldPoly[2].set(0,-Orange_ShieldBottomDepth);
			OrangeShieldPoly[2].set(Orange_ShieldMidHalfWidth,-Orange_ShieldBottomDepth);
			OrangeShieldPoly[3].set(-Orange_ShieldMidHalfWidth,-Orange_ShieldBottomDepth);
			OrangeShieldPoly[4].set(-Orange_ShieldMidHalfWidth,0);
			OrangeShieldPoly[5].set(-Orange_ShieldFrontHalfWidth,Orange_ShieldMidDepth);
			
			// Extrude the solid
			G4VSolid* ShieldBox = new G4ExtrudedSolid("Box1", OrangeShieldPoly, Orange_InsertHalfX, G4TwoVector(0,-Orange_ShieldMidBeamSep), 1, G4TwoVector(0,-Orange_ShieldMidBeamSep), 1);
			
			// Cut the notch for target ladder 
			if(Orange_BeamShieldSep<Orange_TargetWardingY){
				G4Box* CutBoxS = new G4Box("CutBoxS",Orange_TargetWardingZ, Orange_TargetWardingY, Orange_ShieldHalfX+SafetyGap);
				ShieldBox = new G4SubtractionSolid("Box2", ShieldBox, CutBoxS,0,G4ThreeVector(0,0,0));
			}
			
			// Make the logical volume, with FORBID tag for pretty pictures
			G4LogicalVolume *ShieldBoxL = new G4LogicalVolume(ShieldBox, ShieldInMat,"ShieldBoxL_FORBID",0,0,0);
			G4RotationMatrix* rotate1 = new G4RotationMatrix;
			rotate1->rotateZ(Zbar);
			rotate1->rotateY(90*deg);
			ShieldBoxL->SetVisAttributes(OneVisAtt);
			new G4PVPlacement(rotate1,rotZbar*G4ThreeVector(0,0,0), ShieldBoxL,"ShieldInsert", expHallLog,false,0); 
		
			// Extend the poly very slightly before making a cut box
			for(unsigned int i=0;i<OrangeShieldPoly.size();i++)OrangeShieldPoly[i]*=1.01;
			// Extrude the slightly bigger shape to use as a cutout in other parts (field region and xray block)
			ShieldCutBox = new G4ExtrudedSolid("CutBox1", OrangeShieldPoly, Orange_InsertHalfX*1.01, G4TwoVector(0,-Orange_ShieldMidBeamSep), 1, G4TwoVector(0,-Orange_ShieldMidBeamSep), 1);
			
			
			// Build and place the new under-shield x-ray block
			
			G4double XrayHW=Orange_ShieldMidHalfWidth;
			if(Orange_ShieldMidHalfWidth>MStart){XrayHW=MStart;}
			// Z-Width only goes as far as the magnets
			
			G4double XrayHH=std::abs(Orange_ShieldMidBeamSep-LowestPointOfMagnet);
			if(Orange_ShieldBottomDepth>=XrayHH)XrayHH=Orange_ShieldBottomDepth; 
			XrayHH/=2;
			//Goes to EITHER the bottom of the magnets OR a little under the shield.
			
			G4VSolid* XrayBox = new G4Box("XrayBox",Orange_ShieldHalfX,XrayHH,XrayHW-KaptonThickness);
			G4VSolid* XrayCover = new G4Box("XrayBox",Orange_ShieldHalfX-0.1*mm,XrayHH,XrayHW);
			
			
			G4RotationMatrix* subtrarot = new G4RotationMatrix;
			subtrarot->rotateY(90*deg); //ShieldCutBox is the wrong way as G4ExtrudedSolid in Z-axis by default
			XrayCover = new G4SubtractionSolid("XrayShape", XrayCover, XrayBox,0,G4ThreeVector(0,KaptonThickness,0));
			XrayCover = new G4SubtractionSolid("XrayShape", XrayCover, ShieldCutBox,subtrarot,G4ThreeVector(0,Orange_ShieldMidBeamSep+XrayHH+KaptonThickness,0));
			XrayBox = new G4SubtractionSolid("XrayShape", XrayBox, ShieldCutBox,subtrarot,G4ThreeVector(0,Orange_ShieldMidBeamSep+XrayHH,0));
			
			
			G4LogicalVolume *XrayLog = new G4LogicalVolume(XrayBox, XrayMat,"XrayLog_FORBID",0,0,0);
			XrayLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.6,0.471,0.059)));
			G4RotationMatrix* rotateblock = new G4RotationMatrix;
			rotateblock->rotateZ(Zbar);
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,-XrayHH-Orange_ShieldMidBeamSep,0), XrayLog,"XrayBlock", expHallLog,false,0);
			
			G4LogicalVolume *XrayCoverLog = new G4LogicalVolume(XrayCover, KapMat,"XrayCoverLog_FORBID",0,0,0);
			XrayCoverLog->SetVisAttributes(FiveVisAtt);
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,-XrayHH-Orange_ShieldMidBeamSep-KaptonThickness,0), XrayCoverLog,"XrayBlockCover", expHallLog,false,0);
			

			// Place carbon blocker
			
			if(fAddBlocker){
				
				G4double HalfBlockGap=0.5*(Orange_BeamDetY-2.95*mm-(XrayHH*2+Orange_ShieldMidBeamSep+KaptonThickness));

				if(HalfBlockGap>SafetyGap*1.5){
					G4Box* ElectronBlockerBox = new G4Box("ElectronBlockerBox",fBB34Chip_HalfLength,HalfBlockGap,CrossBlockHalfThickness);
					G4LogicalVolume *ElectronBlockerLog = new G4LogicalVolume(ElectronBlockerBox, BlockerMat,"ElectronBlockerLog_FORBID",0,0,0);
					ElectronBlockerLog->SetVisAttributes(ThreeVisAtt);
					new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,-HalfBlockGap-XrayHH*2-Orange_ShieldMidBeamSep-KaptonThickness,0), ElectronBlockerLog,"ElectronBlocker", expHallLog,false,0);
				}else{
					G4cout<<G4endl<<"NO ROOM FOR ELECTRON BLOCKER";
				}
			}
		}
	}
	
	// Blocker for field region in phi
	
	if(PhiBlockThickness>0){
		
		PhiBlockThickness*=0.5;
		
		G4double phiblkuc=PoleSepHalf-(((fBB34Chip_HalfLength-fBB34Chip_Dead)/Orange_BeamDetY)*(Orange_MagMidOffset-Orange_MagY));
		G4double phiblkl=(Orange_MagZ-Orange_ShieldMidHalfWidth)*0.5;
		G4double phiblkw=phiblkuc+Orange_MagHalfThick*2;
		phiblkw*=0.5;
		
		G4RotationMatrix* rotatepblock = new G4RotationMatrix;
		rotatepblock->rotateZ(Zbar);
		G4Box* PhiBlockerBox = new G4Box("ElectronBlockerBox",phiblkw,PhiBlockThickness,phiblkl);
		G4LogicalVolume *PhiBlockerLog = new G4LogicalVolume(PhiBlockerBox, BlockerMat,"PhiBlockerLog_FORBID",0,0,0);
		PhiBlockerLog->SetVisAttributes(ThreeVisAtt);
		
		G4double phX=PoleSepHalf+phiblkw-phiblkuc;
		G4double phY=Orange_MagY+PhiBlockThickness-Orange_MagMidOffset;
		G4double phZ=Orange_ShieldMidHalfWidth+phiblkl;
		
		for(int x=-1;x<2;x+=2){
			for(int z=-1;z<2;z+=2){
				new G4PVPlacement(rotatepblock,rotZbar*G4ThreeVector(x*phX,phY,z*phZ), PhiBlockerLog,"PhiBlocker", expHallLog,false,0);
		}}
	}
	
	// Place BB34 detector
	
	G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateZ(Zbar);
    rotate3->rotateZ(90*deg);
    rotate3->rotateY(90*deg);
    new G4PVPlacement(rotate3,rotZbar*G4ThreeVector(0,-Orange_BeamDetY,0), BuildBB34(),"BB34Detector", expHallLog,false,0);
	
	// Place Scintilators
    
    G4double ScintD=20*mm; //half
    G4Box* ScintBox = new G4Box("ScintBox", ScintD, ScintThick, ScintD); 
    G4LogicalVolume *ScintL = new G4LogicalVolume(ScintBox, ScintMat,"ScintL",0,0,0);
	ScintL->SetVisAttributes(ThreeVisAtt);
    
    G4RotationMatrix* rotate4 = new G4RotationMatrix;
    rotate4->rotateZ(Zbar);
    
    std::stringstream tt;
    tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-Orange_BeamDetY-Orange_ScintSep,-(0.5*mm+ScintD)), ScintL,tt.str(), expHallLog,false,0);
    
    std::stringstream TT;
    TT<<"SiSegmentPhys_0_"<<BuildMicronSiN;
    BuildMicronSiN++;
    new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-Orange_BeamDetY-Orange_ScintSep,(0.5*mm+ScintD)), ScintL,TT.str(), expHallLog,false,0);
    
	// Place Copper Coolant Block Under Detector
	
    G4Box* CopperBox = new G4Box("CopperBox", fBB34PCB_HalfALength, 5*mm, fBB34PCB_HalfWidth); 
    G4LogicalVolume *CopperL = new G4LogicalVolume(CopperBox, G4Material::GetMaterial("Copper"),"CopperL",0,0,0);
	new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-Orange_BeamDetY-Orange_ScintSep-1.6*mm-ScintThick-5*mm,0), CopperL,"CopperBlock", expHallLog,false,0);
// 	CopperL->SetVisAttributes(G4VisAttributes::Invisible); 
	CopperL->SetVisAttributes(vis_att_hid); 
    
    
	// Build Mag Shapes
    
	std::vector<G4TwoVector> MagPolygon(4);
	
	MagPolygon[0].set(MStart,Orange_MagY);
	MagPolygon[1].set(Orange_MagZ,Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng));
	MagPolygon[2].set(Orange_MagZ,-(Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng)));
	MagPolygon[3].set(MStart,-Orange_MagY);
    
//     for(unsigned int i=0;i<MagPolygon.size();i++)MagPolygon[i]+=G4TwoVector(0,-Orange_MagMidOffset);

	G4VSolid* fMagBox = new G4ExtrudedSolid("MagBox", MagPolygon, Orange_MagHalfThick, G4TwoVector(0,-Orange_MagMidOffset), 1, G4TwoVector(0,-Orange_MagMidOffset), 1);
	G4VSolid* fMagKap = new G4ExtrudedSolid("MagKap", MagPolygon, KaptonThickness*0.5, G4TwoVector(0,-Orange_MagMidOffset), 1, G4TwoVector(0,-Orange_MagMidOffset), 1);

    
	std::vector<G4TwoVector> YokePolygon(8);
	
	YokePolygon[0].set(MStart,Orange_MagY);
	YokePolygon[1].set(Orange_MagZ,Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng));
	YokePolygon[2].set(Orange_MagZ,-(Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng)));
	YokePolygon[3].set(MStart,-Orange_MagY);
	YokePolygon[4].set(-MStart,-Orange_MagY);
	YokePolygon[5].set(-Orange_MagZ,-(Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng)));
	YokePolygon[6].set(-Orange_MagZ,Orange_MagY+(Orange_MagZ-MStart)*tan(Orange_MagAng));
	YokePolygon[7].set(-MStart,Orange_MagY);
	
	G4VSolid* fYokeBox = new G4ExtrudedSolid("YokeBox", YokePolygon, Orange_YokeHalfThick, G4TwoVector(0,-Orange_MagMidOffset), 1, G4TwoVector(0,-Orange_MagMidOffset), 1);

	// Place Magnetic Field Volumes 
	
	G4Material* matWorld = G4Material::GetMaterial("Vacuum");
    
	G4double 	MPX=(PoleSepHalf+Orange_MagHalfThick+KaptonThickness);
	G4double 	MPXk=(PoleSepHalf+KaptonThickness*0.5);
    
	G4LogicalVolume *MagBoxL = new G4LogicalVolume(fMagBox, mMaterial,"MagBoxL_FORBID",0,0,0);
	G4LogicalVolume *MagKapL = new G4LogicalVolume(fMagKap, KapMat,"MagKapL_FORBID",0,0,0);
	G4LogicalVolume *YokeL = new G4LogicalVolume(fYokeBox, YokeMat,"YokeL_FORBID",0,0,0);
	MagKapL->SetVisAttributes(FiveVisAtt);
// 	YokeL->SetVisAttributes(vis_att_hid); 
    
    for(int i=-1;i<2;i+=2){
		G4RotationMatrix* rotmag = new G4RotationMatrix;
		rotmag->rotateZ(Zbar);
		rotmag->rotateY(i*90*deg);

        for(int j=-1;j<2;j+=2){
			int cn=0;;
			if(SetMagCopyNum)cn=i+j*0.5+2;
            new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(j*MPX,0,0), MagBoxL,"Mag", expHallLog,false,cn); 
            new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(j*MPXk,0,0), MagKapL,"MagKap", expHallLog,false,0); 
        }
        
        new G4PVPlacement(rotmag,rotZbar*G4ThreeVector((MPX+Orange_YokeHalfThick+Orange_MagHalfThick)*i,0,0),YokeL,"Yoke",expHallLog,false,0);
        
    }
//     fOrangeFieldVolume->SetFieldManager(localFieldManager1, true);
	
	
	// MakeClamp
	
	if(gB("IncludeBB34Clamp")){
		
// 		G4double CutRad=3*mm; //Including the curves slows things down, so leave out for now
		G4double CutRad=0*mm;
		G4double X=gD("BB34ClampX");
		G4double Y=gD("BB34ClampY");
		G4double Z=gD("BB34ClampZ");
		
		G4VSolid* Clamp = new G4Box("BB34ClampBox",fBB34PCB_HalfALength,Y,fBB34PCB_HalfWidth+15*mm);
		G4Box* ClampCut = new G4Box("ClampCut",X-CutRad,Y*2,Z);
		Clamp = new G4SubtractionSolid("BB34ClampBoxc", Clamp, ClampCut,0,G4ThreeVector(0,0,0));
		ClampCut = new G4Box("ClampCut",fBB34PCB_HalfALength+1*mm,0.51*mm,1.01*mm);
		Clamp = new G4SubtractionSolid("BB34ClampBoxcc", Clamp, ClampCut,0,G4ThreeVector(0,Y,0));

		if(CutRad>0){
		ClampCut = new G4Box("ClampCut",X,Y*2,Z-CutRad);
		Clamp = new G4SubtractionSolid("BB34ClampBoxC", Clamp, ClampCut,0,G4ThreeVector(0,0,0));
		G4Tubs* RadTub = new G4Tubs("RadTub", 0,CutRad,Y*2, 0,360*CLHEP::deg);
		G4RotationMatrix* rottub = new G4RotationMatrix;
		rottub->rotateX(90*deg);
		for(int i=-1;i<2;i+=2){
	        for(int j=-1;j<2;j+=2){
				Clamp = new G4SubtractionSolid("BB34ClampBoxCC", Clamp, RadTub,rottub,G4ThreeVector((X-CutRad)*i,0,(Z-CutRad)*j));
 
        }}}
		
		G4LogicalVolume* BB34ClampLog = new G4LogicalVolume(Clamp, G4Material::GetMaterial("Aluminium"), "BB34ClampLog_FORBID", 0,0,0);
		BB34ClampLog->SetVisAttributes(new G4VisAttributes(G4Colour(0.21,0.22,0.41)));

		new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,-Orange_BeamDetY+fBB34PCB_HalfThickness+Y+0.01*mm,0),BB34ClampLog, "",  expHallLog, false, 0);
	}
	

    G4cout<<G4endl;
}


G4LogicalVolume* ApparatusDICE::BuildISOK160Cross(){
    G4Tubs* OuterTub = new G4Tubs("OuterTub", 0, 154*0.5*mm,150*mm, 0,360*CLHEP::deg);
    G4Tubs* InnerTub = new G4Tubs("OuterTub", 0, 150*0.5*mm,(150-4)*mm, 0,360*CLHEP::deg);
    G4Tubs* BeamHole = new G4Tubs("OuterTub", 0, 150*0.5*mm,200*mm, 0,360*CLHEP::deg);
	G4RotationMatrix* rotate1 = new G4RotationMatrix;
	rotate1->rotateX(90*CLHEP::deg);
	G4RotationMatrix* rotate2 = new G4RotationMatrix;
	rotate2->rotateX(90*CLHEP::deg);
	rotate2->rotateY(90*CLHEP::deg);
	
	G4VSolid* Out2 = new G4UnionSolid("Out2", OuterTub, OuterTub,rotate1,G4ThreeVector());	
	G4VSolid* Out3 = new G4UnionSolid("Out3", Out2, OuterTub,rotate2,G4ThreeVector());
	G4VSolid* Inner2 = new G4UnionSolid("Inner2", BeamHole, InnerTub,rotate1,G4ThreeVector());	
	G4VSolid* Inner3 = new G4UnionSolid("Inner2", Inner2, InnerTub,rotate2,G4ThreeVector());
	
    G4VSolid* FinSolid = new G4SubtractionSolid("FinSolid", Out3, Inner3,0,G4ThreeVector(0,0,0));
	

    return new G4LogicalVolume(FinSolid, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "ISOK160Cross", 0,0,0);

	
}


