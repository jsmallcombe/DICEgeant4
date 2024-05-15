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
	
	
	SetParamD("BeamDetTop",57.1*mm);
	
	SetParamD("MagZEnd",60);
	SetParamD("MagFlare",0*deg);
	SetParamD("MagYPos",30);
	SetParamD("MagHeight",20);
	SetParamD("MagThickA",5);
	SetParamD("MagThickB",5);
	
	SetParamD("PoleGapZH",10);
	SetParamD("YokeGapH",21);
	SetParamD("YokeDepth",4);
	
	SetParamD("ShieldTaThickH",12.5);
	SetParamD("ShieldFrontY",10);
	SetParamD("ShieldMidZ",17.0);
	SetParamD("ShieldFrontAng",50*deg);
	SetParamD("BrassBackAng",30*deg);

	SetParamD("BB34ClampX",21);
	SetParamD("BB34ClampZ",41);
	SetParamD("BB34ClampY",0.75);
	
	SetParamD("BeamScintTop",66.4);
	
	SetParamD("PhiBlockThickness",-1);
	
	SetParamB("IncludeBB34Clamp",true);
	SetParamB("SetMagCopyNum",false);
	SetParamB("RemoveShield",false);
	SetParamB("AddBlocker",true);
	
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
	
	G4cout<<"DICE SetableParameters :"<<G4endl;
	std::cout<<"DICE SetableParameters :"<<std::endl;
	for(unsigned int i=0;i<SetNmLstD.size();i++){
		G4cout<<SetNmLstD[i]<<" "<<*SetPtrLstD[i]<<G4endl;
		std::cout<<SetNmLstD[i]<<" "<<*SetPtrLstD[i]<<std::endl;
	}
	G4cout<<"Fin."<<G4endl;
	std::cout<<"Fin."<<std::endl;
	
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

	G4double SafetyGap=2*mm;
	G4double TargetWardingY=16*mm; // Half Gap Target Ladder Needs
	G4double TargetWardingZ=3*mm; // Half Gap Target Ladder Needs (Beamward)
    G4double ScintThick=1*mm; //half
	G4double KaptonThickness=0.1*mm; // Abs thickness of kapton covers
	G4double CrossBlockHalfThickness=1*mm; //Under shield electron blocker
	
	//// Parameter Processing ////
	
    G4double MStart=gD("PoleGapZH");
	G4double MagHH=gD("MagHeight")/2.;
	G4double MagY=gD("MagYPos");
	G4double ShieldBackY=MagY;
	G4double ShieldMidY=gD("MagYPos")-MagHH;
	G4double MagBottom=gD("MagYPos")+MagHH;
	G4double YokeGap=gD("YokeGapH");
	G4double TaGap=gD("ShieldTaThickH");
	G4double YokeThickH=gD("YokeDepth")*0.5;
	
	G4double MagAng=gD("MagFlare");
	if(MagAng>0)ShieldMidY-=(gD("ShieldMidZ")-MStart)*tan(MagAng);
	
	G4double MagZEnd=gD("MagZEnd");
	
	if(MagAng<0){
		G4double ShrtMag=MStart+abs(MagHH/tan(MagAng));
		if(ShrtMag<MagZEnd){
			MagZEnd=ShrtMag-SafetyGap;
			G4cout<<G4endl<<"MAGNETS LENGTH OVERRIDDEN DUE TO MAGNET ANGLE";
		}
	}

	
	//////////////////////////////////////
	// Build and Place Shield
	//////////////////////////////////////
		
	if(!gB("RemoveShield")){
		
		G4double ShieldMidZ=gD("ShieldMidZ");
		G4double ShieldFrontY=gD("ShieldFrontY");
		G4double ShieldFrontHeight=ShieldMidY-ShieldFrontY;
		G4double ShieldFrontZ = ShieldMidZ-(ShieldFrontHeight/tan(gD("ShieldFrontAng")));

		std::vector<G4TwoVector> TaPolyTot(10);		
		TaPolyTot[0].set(TargetWardingZ,-TargetWardingY);
		TaPolyTot[1].set(TargetWardingZ,-ShieldFrontY);
		TaPolyTot[2].set(ShieldFrontZ,-ShieldFrontY);
		TaPolyTot[3].set(ShieldMidZ,-ShieldMidY);
		TaPolyTot[4].set(ShieldMidZ,-ShieldBackY);
		

		G4double BrassFrontZ = ShieldMidZ-((ShieldMidY-TargetWardingY)/tan(gD("ShieldFrontAng")));
		G4double BrassFrontY = TargetWardingY;
		G4double BrassMidY=MagBottom-(ShieldMidZ-MStart)*tan(gD("BrassBackAng"));
		G4double BrassBackZ=(CrossBlockHalfThickness+0.1*mm);
		G4double BrassBackY=MagBottom+(MStart-BrassBackZ)*tan(gD("BrassBackAng"));
		
		std::vector<G4TwoVector> BrassPolyTot(10);
		BrassPolyTot[0].set(BrassFrontZ,-BrassFrontY);
		BrassPolyTot[1].set(ShieldMidZ,-ShieldMidY);
		BrassPolyTot[2].set(ShieldMidZ,-BrassMidY);
		BrassPolyTot[3].set(BrassBackZ,-BrassBackY);
		BrassPolyTot[4].set(BrassBackZ,-MagBottom);
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
		
		G4double CutL=MStart*2;
		
		G4VSolid* BrassS = new G4ExtrudedSolid("BrassS", BrassPolyTot, YokeGap+YokeThickH*2, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
		
		G4Box* CutBoxS = new G4Box("CutBoxS",CutL+0.01,(MagY-ShieldMidY)+0.01,CutL+0.01);
		
		for(int i=-1;i<2;i+=2){
			BrassS = new G4SubtractionSolid("BrassS", BrassS, CutBoxS,0,G4ThreeVector(0,-ShieldBackY,i*(YokeGap+CutL)));//CutForYokes
			for(int j=-1;j<2;j+=2){
				BrassS = new G4SubtractionSolid("BrassS", BrassS, CutBoxS,0,G4ThreeVector(j*(MStart+CutL),-ShieldBackY,i*(CutL+TaGap)));
			}
		}
// 		
		G4Box* CutBoxT = new G4Box("CutBoxT",CutL,ShieldBackY+0.01,TaGap+0.01);
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
		
		if(gB("AddBlocker")){
			G4double HalfBlockGap=0.5*(gD("BeamDetTop")-1*mm-MagBottom);
			G4Box* ElectronBlockerBox = new G4Box("ElectronBlockerBox",YokeGap+YokeThickH*2,HalfBlockGap-0.1*mm,CrossBlockHalfThickness);
			G4LogicalVolume *ElectronBlockerLog = new G4LogicalVolume(ElectronBlockerBox, BlockerMat,"ElectronBlockerLog_FORBID",0,0,0);
			ElectronBlockerLog->SetVisAttributes(ThreeVisAtt);
			new G4PVPlacement(new G4RotationMatrix,rotZbar*G4ThreeVector(0,-HalfBlockGap-MagBottom,0), ElectronBlockerLog,"ElectronBlocker", expHallLog,false,0);
			
			std::vector<G4TwoVector> AddOnPol(4);	
			AddOnPol[0].set(1.1,5);
			AddOnPol[1].set(9.01,5);
			AddOnPol[2].set(9.01-(5/tan(35*deg)),0);
			AddOnPol[3].set(1.1,0);

			G4VSolid* AddOnS = new G4ExtrudedSolid("AddOnS", AddOnPol,YokeGap+YokeThickH*2, G4TwoVector(0,0), 1, G4TwoVector(0,0), 1);
			G4LogicalVolume *AddOnLog = new G4LogicalVolume(AddOnS, BlockerMat,"AddOnLog_FORBID",0,0,0);
			AddOnLog->SetVisAttributes(ThreeVisAtt);
			
			new G4PVPlacement(rotateblock,rotZbar*G4ThreeVector(0,-gD("BeamDetTop")+1.55,0), AddOnLog,"ElectronBlocker", expHallLog,false,0);
			new G4PVPlacement(rotate1,rotZbar*G4ThreeVector(0,-gD("BeamDetTop")+1.55,0), AddOnLog,"ElectronBlocker", expHallLog,false,0);
			
			
		}
	}

// // // 	// Blocker for field region in phi
// // // 	if(PhiBlockThickness>0){
// // // 		
// // // 		PhiBlockThickness*=0.5;
// // // 		
// // // 		G4double phiblkuc=PoleSepHalf-(((fBB34Chip_HalfLength-fBB34Chip_Dead)/Orange_BeamDetY)*(MagY-Orange_MagY));
// // // 		G4double phiblkl=(Orange_MagZ-Orange_ShieldMidHalfWidth)*0.5;
// // // 		G4double phiblkw=phiblkuc+Orange_MagHalfThick*2;
// // // 		phiblkw*=0.5;
// // // 		
// // // 		G4RotationMatrix* rotatepblock = new G4RotationMatrix;
// // // 		rotatepblock->rotateZ(Zbar);
// // // 		G4Box* PhiBlockerBox = new G4Box("ElectronBlockerBox",phiblkw,PhiBlockThickness,phiblkl);
// // // 		G4LogicalVolume *PhiBlockerLog = new G4LogicalVolume(PhiBlockerBox, BlockerMat,"PhiBlockerLog_FORBID",0,0,0);
// // // 		PhiBlockerLog->SetVisAttributes(ThreeVisAtt);
// // // 		
// // // 		G4double phX=PoleSepHalf+phiblkw-phiblkuc;
// // // 		G4double phY=Orange_MagY+PhiBlockThickness-MagY;
// // // 		G4double phZ=Orange_ShieldMidHalfWidth+phiblkl;
// // // 		
// // // 		for(int x=-1;x<2;x+=2){
// // // 			for(int z=-1;z<2;z+=2){
// // // 				new G4PVPlacement(rotatepblock,rotZbar*G4ThreeVector(x*phX,phY,z*phZ), PhiBlockerLog,"PhiBlocker", expHallLog,false,0);
// // // 		}}
// // // 	}

	// Place BB34 detector
	
	G4RotationMatrix* rotate3 = new G4RotationMatrix;
    rotate3->rotateZ(Zbar);
    rotate3->rotateZ(90*deg);
    rotate3->rotateY(90*deg);
    new G4PVPlacement(rotate3,rotZbar*G4ThreeVector(0,-gD("BeamDetTop")-fBB34PCB_HalfThickness,0), BuildBB34(),"BB34Detector", expHallLog,false,0);

	// Place Scintilators
    
    G4double ScintD=20*mm; //half
    G4Box* ScintBox = new G4Box("ScintBox", ScintD, ScintThick, ScintD); 
    G4LogicalVolume *ScintL = new G4LogicalVolume(ScintBox, ScintMat,"ScintL",0,0,0);
	ScintL->SetVisAttributes(ThreeVisAtt);
    
    G4RotationMatrix* rotate4 = new G4RotationMatrix;
    rotate4->rotateZ(Zbar);
    
	for(int i=-1;i<2;i+=2){
		std::stringstream tt;
		tt<<"SiSegmentPhys_0_"<<BuildMicronSiN;
		BuildMicronSiN++;
		new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-gD("BeamScintTop")-ScintThick,i*(0.5*mm+ScintD)), ScintL,tt.str(), expHallLog,false,0);
	}
    
	// Place Copper Coolant Block Under Detector
	
    G4Box* CopperBox = new G4Box("CopperBox", 33*mm, 5*mm, 63*mm); 
    G4LogicalVolume *CopperL = new G4LogicalVolume(CopperBox, G4Material::GetMaterial("Copper"),"CopperL",0,0,0);
	new G4PVPlacement(rotate4,rotZbar*G4ThreeVector(0,-gD("BeamScintTop")-ScintThick*2-1.6*mm-5*mm,0), CopperL,"CopperBlock", expHallLog,false,0);
// 	CopperL->SetVisAttributes(G4VisAttributes::Invisible); 
	CopperL->SetVisAttributes(vis_att_hid); 

	// Build Mag Shapes
    
	std::vector<G4TwoVector> MagPolygon(4);
	std::vector<G4TwoVector> YokePolygon(8);
	
	MagPolygon[0].set(MStart,MagHH);
	MagPolygon[1].set(MagZEnd,MagHH+(MagZEnd-MStart)*tan(MagAng));
	MagPolygon[2].set(MagZEnd,-(MagHH+(MagZEnd-MStart)*tan(MagAng)));
	MagPolygon[3].set(MStart,-MagHH);
	
	for(int i=0;i<4;i++){
		YokePolygon[i].set(MagPolygon[i].x(),MagPolygon[i].y());
		YokePolygon[7-i].set(-MagPolygon[i].x(),MagPolygon[i].y());
	}
	
	G4double MTA=gD("MagThickA")*0.5;
	G4double MTB=gD("MagThickB")*0.5;
	
	G4VSolid* fMagBoxA = new G4ExtrudedSolid("MagBoxA", MagPolygon, MTA, G4TwoVector(0,-MagY), 1, G4TwoVector(0,-MagY), 1);
	G4VSolid* fMagBoxB = new G4ExtrudedSolid("MagBoxB", MagPolygon, MTB, G4TwoVector(0,-MagY), 1, G4TwoVector(0,-MagY), 1);
	G4VSolid* fMagKap = new G4ExtrudedSolid("MagKap", MagPolygon, KaptonThickness*0.5, G4TwoVector(0,-MagY), 1, G4TwoVector(0,-MagY), 1);
	G4VSolid* fYokeBox = new G4ExtrudedSolid("YokeBox", YokePolygon, YokeThickH, G4TwoVector(0,-MagY), 1, G4TwoVector(0,-MagY), 1);


	// Place Magnetic Field Volumes 
    
    
	G4LogicalVolume *MagBoxAL = new G4LogicalVolume(fMagBoxA, mMaterial,"MagBoxAL_FORBID",0,0,0);
	G4LogicalVolume *MagBoxBL = new G4LogicalVolume(fMagBoxB, mMaterial,"MagBoxBL_FORBID",0,0,0);
	G4LogicalVolume *MagKapL = new G4LogicalVolume(fMagKap, KapMat,"MagKapL_FORBID",0,0,0);
	G4LogicalVolume *YokeL = new G4LogicalVolume(fYokeBox, YokeMat,"YokeL_FORBID",0,0,0);
	MagKapL->SetVisAttributes(FiveVisAtt);
// 	YokeL->SetVisAttributes(vis_att_hid); 
    
	G4LogicalVolume* ML[2]={MagBoxBL,MagBoxAL};
	G4double MX[2]={MTB,MTA};	

	
    for(int i=-1;i<2;i+=2){
		int k=(i+1)/2;
		
		G4double 	MPX=(YokeGap-MX[k])-0.01;
		G4double 	MPXk=(YokeGap-MX[k]*2-KaptonThickness*0.5)-0.02;
		
		G4RotationMatrix* rotmag = new G4RotationMatrix;
		rotmag->rotateZ(Zbar);
		rotmag->rotateY(i*90*deg);

        for(int j=-1;j<2;j+=2){
			int cn=0;;
			if(gB("SetMagCopyNum"))cn=i+j*0.5+2;
            new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(j*MPX,0,0), ML[k],"Mag", expHallLog,false,cn); 
            new G4PVPlacement(rotmag,rotZbar*G4ThreeVector(j*MPXk,0,0), MagKapL,"MagKap", expHallLog,false,0); 
        }
        
        new G4PVPlacement(rotmag,rotZbar*G4ThreeVector((YokeGap+YokeThickH)*i,0,0),YokeL,"Yoke",expHallLog,false,0);
        
    }
    
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
		ClampCut = new G4Box("ClampCut",fBB34PCB_HalfALength+1*mm,0.51*mm,CrossBlockHalfThickness+.01*mm);
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

		new G4PVPlacement(new G4RotationMatrix,G4ThreeVector(0,-gD("BeamDetTop")+Y+0.01*mm,0),BB34ClampLog, "",  expHallLog, false, 0);
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


