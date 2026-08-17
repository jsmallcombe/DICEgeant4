#ifndef APPARATUSCDTE_HH
#define APPARATUSCDTE_HH

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;

// CdTe quad detector apparatus.
class ApparatusCdTe
{
public:
	ApparatusCdTe();
	~ApparatusCdTe();

	// Construction option 1 builds a CdTe quad at its default position.
	void Build(G4LogicalVolume* motherVolume, G4String options);

	// Place a quad whose unrotated front (-Z) surface is at surfacePosition.
	void BuildCdTeQuad(G4LogicalVolume* motherVolume, G4ThreeVector surfacePosition,
		const G4RotationMatrix& rotation);

	// Supported parameter: Size (the full edge length of the cube).
	void SetParamD(G4String name, G4double value);

private:
	G4double fSize;
	G4int fBuildOption;
	G4int fCdTeQuadNumber;
};

#endif
