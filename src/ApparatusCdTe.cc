#include "ApparatusCdTe.hh"

#include <sstream>
#include <vector>

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

ApparatusCdTe::ApparatusCdTe() :
	fSize(10.0 * mm),
	fBuildOption(1),
	fCdTeQuadNumber(0)
{
}

ApparatusCdTe::~ApparatusCdTe()
{
}

void ApparatusCdTe::SetParamD(G4String name, G4double value)
{
	if(name == "Size" && value > 0.0) {
		fSize = value;
	}
}

void ApparatusCdTe::Build(G4LogicalVolume* motherVolume, G4String options)
{
	std::istringstream optionStream(options);
	if(!(optionStream >> fBuildOption)) {
		fBuildOption = 1;
	}
	if(fBuildOption != 1) {
		G4cerr << "CdTe apparatus supports only construction option 1." << G4endl;
		return;
	}

	const G4ThreeVector nominalSurfacePosition(0.0, 0.0, 0.0 * mm);
	G4RotationMatrix positiveRotation;
	positiveRotation.rotateX(30.0 * deg);
	BuildCdTeQuad(motherVolume, nominalSurfacePosition, positiveRotation);

	// G4RotationMatrix negativeRotation;
	// negativeRotation.rotateX(-30.0 * deg);
	// BuildCdTeQuad(motherVolume, nominalSurfacePosition, negativeRotation);
}

void ApparatusCdTe::BuildCdTeQuad(G4LogicalVolume* motherVolume, G4ThreeVector surfacePosition,
	const G4RotationMatrix& rotation)
{
	G4Material* cdTeMaterial = G4Material::GetMaterial("CdTe");
	G4Material* copperMaterial = G4Material::GetMaterial("Copper");
	G4Material* pcbMaterial = G4Material::GetMaterial("FR4");
	G4Material* aluminiumMaterial = G4Material::GetMaterial("Aluminium");
	G4Material* vacuumMaterial = G4Material::GetMaterial("Vacuum");
	if(!cdTeMaterial || !copperMaterial || !pcbMaterial || !aluminiumMaterial || !vacuumMaterial) {
		G4cerr << "CdTe quad material is not defined; CdTe quad was not built." << G4endl;
		return;
	}

	const G4int quadNumber = fCdTeQuadNumber++;

	// Aluminium tube: 30 x 20 x 96.5 mm outer dimensions, with a 27 x 17 mm
	// opening through both Z ends.  The two near-end corners are cut by planes
	// at +/-15 degrees about X, beginning 28 mm from that end face.
	G4Box* tubeOuter = new G4Box("CdTeQuadTubeOuter", 15.0 * mm, 10.0 * mm, 48.25 * mm);
	G4Box* tubeInner = new G4Box("CdTeQuadTubeInner", 13.5 * mm, 8.5 * mm, 50.0 * mm);

	G4double cutouter=(2-sqrt(3))*40 * mm;

	std::vector<G4TwoVector> CornerCut = {
		G4TwoVector(0, 0.0),
		G4TwoVector(cutouter, 40 * mm),
		G4TwoVector(cutouter, -1 * mm),
		G4TwoVector(-cutouter, -1 * mm),
		G4TwoVector(-cutouter, 40 * mm)
	};
	
	G4VSolid* CutSolid = new G4ExtrudedSolid("CdTeQuadTubeCut", CornerCut,15.1 * mm, G4TwoVector(), 1.0, G4TwoVector(), 1.0);
		
	G4RotationMatrix* cutRotation = new G4RotationMatrix;
	cutRotation->rotateY(90.0 * deg);
	cutRotation->rotateZ(90.0 * deg);
 	G4VSolid* tubeSolid = new G4SubtractionSolid("CdTeQuadTubeSolidFinal", tubeOuter, CutSolid, cutRotation, G4ThreeVector(0,0,-(48.25+28) * mm));

	G4LogicalVolume* assemblyLogical = new G4LogicalVolume(tubeSolid, vacuumMaterial, "CdTeQuadAssemblyLogical");
	assemblyLogical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.0)));

	tubeSolid = new G4SubtractionSolid("CdTeQuadTubeSolid", tubeSolid, tubeInner);

	G4LogicalVolume* tubeLogical = new G4LogicalVolume(tubeSolid, aluminiumMaterial, "CdTeQuadTubeLogical");
	tubeLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.70, 0.72, 0.75)));

	new G4PVPlacement(nullptr, G4ThreeVector(), tubeLogical,
		"CdTeQuadTubePhys", assemblyLogical, false, quadNumber);

	const G4double carrierHalfThickness = 0.5 * mm;

	// G4Box* assemblySolid = new G4Box("CdTeQuadAssemblySolid", 13.5 * mm, 8.5 * mm, 3.8 * mm);
	// G4LogicalVolume* assemblyLogical = new G4LogicalVolume(assemblySolid, vacuumMaterial, "CdTeQuadAssemblyLogical");
	// assemblyLogical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.0)));

	G4Box* carrierSolid = new G4Box("CdTeQuadCarrierSolid", 6.0 * mm, 6.0 * mm, carrierHalfThickness);
	G4LogicalVolume* carrierLogical = new G4LogicalVolume(carrierSolid, cdTeMaterial, "CdTeQuadCarrierLogical");
	carrierLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.35, 0.35, 0.40, 0.45)));

	G4Box* activeSolid = new G4Box("CdTeQuadActiveSolid", 2.0 * mm, 2.0 * mm, carrierHalfThickness);
	G4LogicalVolume* activeLogical = new G4LogicalVolume(activeSolid, cdTeMaterial, "CdTeQuadActiveLogical");
	activeLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.90, 0.10, 0.10)));

	G4Box* copperSolid = new G4Box("CdTeQuadCopperSolid", 6.25 * mm, 6.25 * mm, 2.5 * mm);
	G4LogicalVolume* copperLogical = new G4LogicalVolume(copperSolid, copperMaterial, "CdTeQuadCopperLogical");
	copperLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.72, 0.35, 0.12)));

	G4Box* pcbSolid = new G4Box("CdTeQuadPCBSolid", 13.5 * mm, 8.5 * mm, 0.8 * mm);
	G4LogicalVolume* pcbLogical = new G4LogicalVolume(pcbSolid, pcbMaterial, "CdTeQuadPCBLogical");
	pcbLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.10, 0.45, 0.15)));

	const G4ThreeVector activePositions[] = {
		G4ThreeVector( 3.0 * mm,  3.0 * mm, 0.0),
		G4ThreeVector( 3.0 * mm, -3.0 * mm, 0.0),
		G4ThreeVector(-3.0 * mm, -3.0 * mm, 0.0),
		G4ThreeVector(-3.0 * mm,  3.0 * mm, 0.0)
	};

	for(G4int elementNumber = 0; elementNumber < 4; ++elementNumber) {
		std::stringstream name;
		name << "CdTeSegmentPhys_" << elementNumber << "_" << quadNumber;
		new G4PVPlacement(nullptr, activePositions[elementNumber], activeLogical, name.str(), carrierLogical, false, elementNumber);
	}

	// The carrier is intentionally not named as a sensitive CdTe segment. 

	new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, -45.75 * mm), carrierLogical,
		"CdTeQuadCarrierPhys", assemblyLogical, false, quadNumber);
	new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, -42.75 * mm), copperLogical,
		"CdTeQuadCopperPhys", assemblyLogical, false, quadNumber);
	new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, -38.95 * mm), pcbLogical,
		"CdTeQuadPCBPhys", assemblyLogical, false, quadNumber);


	const G4ThreeVector assemblyCentre = rotation * (surfacePosition + G4ThreeVector(0.0, 0.0, 48.25+28 * mm));
	new G4PVPlacement(new G4RotationMatrix(rotation), assemblyCentre, assemblyLogical,
		"CdTeQuadAssemblyPhys", motherVolume, false, quadNumber);
}
