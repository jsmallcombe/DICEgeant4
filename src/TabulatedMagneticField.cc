#include "TabulatedMagneticField.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ThreeVector.hh"

#include <algorithm> //for max_element

TabulatedMagneticField::TabulatedMagneticField(const char* filename) //called in 'nonuniformfield'
: fMaxbx(0),fMaxby(0),fMaxbz(0),fMaxM(0), fInvertXIndex(false), fInvertYIndex(false), fInvertZIndex(false), MirrorXField(false), MirrorYField(false), MirrorZField(false),MirrorFieldXInverse(false), MirrorFieldYInverse(false), MirrorFieldZInverse(false), MirrorLineX(0), MirrorLineY(0), MirrorLineZ(0)
{    
	
	
	// Units of the input files
	G4double lenUnit= mm;
	G4double fieldUnit= tesla; 
	G4cout<<"\n-----------------------------------------------------------"
		<<"\n      Magnetic field"
		<<"\n-----------------------------------------------------------";

	G4cout<<"\n ---> " "Reading the field grid from "<<filename<<" ... "<<std::endl; 

	std::ifstream file(filename); // Open the file for reading.

	if(!file.is_open()) {
		G4cout<<"\n\ncannot open file : "<<filename<<"\n\n" ;
		G4cin.get();
	}
	
	// Read table dimensions 
	file>>fNx>>fNy>>fNz; // Note dodgy order //20/7 what? xyz looks fine!

	G4cout<<"  [ Number of values x,y,z: " 
		<<fNx<<" "<<fNy<<" "<<fNz<<" ] "//111*111*106 gives 1306026 rows
		<<std::endl;

	G4cout<<" ---> assumed the order:  x, y, z, Bx, By, Bz ";

	// Set up storage space for table using values from above- does not initialise values
	fXField.resize(fNx);
	fYField.resize(fNx);
	fZField.resize(fNx);
	for(G4int ix=0; ix<fNx; ix++) {
		fXField[ix].resize(fNy);
		fYField[ix].resize(fNy);
		fZField[ix].resize(fNy);
		for(G4int iy=0; iy<fNy; iy++) {
			fXField[ix][iy].resize(fNz);
			fYField[ix][iy].resize(fNz);
			fZField[ix][iy].resize(fNz);
		}
	}//three field values per point, hence three arrays needed

	// Read in the data and fill arrays
	G4double xval,yval,zval,bx,by,bz;
	
	for(G4int iz=0; iz<fNz; iz++) {
		for(G4int ix=0; ix<fNx; ix++) {
			for(G4int iy=0; iy<fNy; iy++) {
// 				file >> xval >> yval >> zval >> bx >> by >> bz;
				file >> zval >> xval >> yval >> bz >> bx >> by;

				if(ix==0 && iy==0 && iz==0) {//min is first value?? theres no sort/search 
					fMinx = xval * lenUnit;
					fMiny = yval * lenUnit;
					fMinz = zval * lenUnit;
				}
				fXField[ix][iy][iz] = bx * fieldUnit;
				fYField[ix][iy][iz] = by * fieldUnit;
				fZField[ix][iy][iz] = bz * fieldUnit;
				
				G4ThreeVector MagVec(bx,  by,  bz); 

				if(std::abs(bx) > std::abs(fMaxbx)) fMaxbx = bx; // no unit as read-out in terminal
				if(std::abs(by) > std::abs(fMaxby)) fMaxby = by;
				if(std::abs(bz) > std::abs(fMaxbz)) fMaxbz = bz;
				if(MagVec.mag() > fMaxM) fMaxM = MagVec.mag() ;
			}
		}
	}
	file.close(); //internally stored values, so can close file
	
	fMaxbx *=fieldUnit;
	fMaxby *=fieldUnit;
	fMaxbz *=fieldUnit;
	fMaxM *=fieldUnit;
				

	fMaxx = xval * lenUnit; //now max dimension values are the last values - i.e. post-loop values
	fMaxy = yval * lenUnit;
	fMaxz = zval * lenUnit;

	G4cout<<"\n ---> ... done reading "<<std::endl;


	// Should really check that the limits are not the wrong way around.
	if(fMaxx < fMinx) {
		std::swap(fMaxx,fMinx);
		fInvertXIndex = true;
	} 
	if(fMaxy < fMiny) {
		std::swap(fMaxy,fMiny);
		fInvertYIndex = true;
	} 
	if(fMaxz < fMinz) {
		std::swap(fMaxz,fMinz); 
		fInvertZIndex = true;
	} 
	
	G4cout<< "\n ---> Min values x,y,z: " 
	<< fMinx/cm<<" "<<fMiny/cm<<" "<<fMinz/cm<<" cm "
	<< "\n ---> Max values x,y,z: " 
	<< fMaxx/cm<<" "<<fMaxy/cm<<" "<<fMaxz/cm<<" cm ";
		
	G4cout<<"\t\t\tMax Bx value = "<< fMaxbx/tesla <<" Tesla."<<G4endl;
	G4cout<<"\t\t\tMax By value = "<< fMaxby/tesla <<" Tesla."<<G4endl;
	G4cout<<"\t\t\tMax Bz value = "<< fMaxbz/tesla <<" Tesla."<<G4endl;
	G4cout<<"\t\t\tMax Field value = "<< fMaxM/tesla <<" Tesla."<<G4endl;
	
	
	fDx = fMaxx - fMinx;
	fDy = fMaxy - fMiny;
	fDz = fMaxz - fMinz;
	G4cout<<"\n ---> Dif values x,y,z (range): " 
		<<fDx/cm<<" "<<fDy/cm<<" "<<fDz/cm<<" cm in z "
		<<"\n-----------------------------------------------------------"<<std::endl;
}


void TabulatedMagneticField::SetField(G4double MaxSetField){
	G4cout<<"Changing Max Field value from "<< fMaxM/tesla <<" Tesla to"<<MaxSetField/tesla <<" Tesla."<<G4endl;
	
	G4double scale =MaxSetField/fMaxM;
	fMaxM=MaxSetField;
	
	for(G4int iz=0; iz<fNz; iz++) {
		for(G4int ix=0; ix<fNx; ix++) {
			for(G4int iy=0; iy<fNy; iy++) {
				fXField[ix][iy][iz] *= scale;
				fYField[ix][iy][iz] *= scale;
				fZField[ix][iy][iz] *= scale;
			}
		}
	}
	
}

void TabulatedMagneticField::SetFieldMirror(int xyz,bool antimirror){
	switch(xyz) {
		case 1: 
			MirrorFieldXInverse=antimirror;
			MirrorXField=true;
			break;
		case 2:
			MirrorFieldYInverse=antimirror;
			MirrorYField=true;
			break;
		case 3:
			MirrorFieldZInverse=antimirror;
			MirrorZField=true;
			break;
		default:
			break;
	}
}

void TabulatedMagneticField::SetFieldMirrorPoint(G4ThreeVector Mpoint){
  		MirrorLineX=Mpoint.getX();
		MirrorLineY=Mpoint.getY();
		MirrorLineZ=Mpoint.getZ();
}
  
void TabulatedMagneticField::GetFieldValue(const G4double point[4], G4double* Bfield) const
{
	G4double x = point[0];
	G4double y = point[1];
	G4double z = point[2];

	//Rotation treatment Mhd : 25 Mar 2015
	//
	//  x,y,z   ---Rot (angle) --->  x',y',z'
	//  |
	//  v
	//  Bx,By,Bz <---Rot (-angle)--- Bx',By',Bz'	 
	//

	// Rotate the position here : Mhd : 25 Mar 2015 
// 	G4ThreeVector R(x,  y,  z); 
// 	R.rotateZ(-fZrotation*deg); // rotation made in the opposite direction of the lens
// 	x = R.getX();
// 	y = R.getY();
// 	z = R.getZ();

	
	G4int MirX=1,MirY=1,MirZ=1;
	
	if(MirrorXField){
	if(!(x>=fMinx && x<=fMaxx)){
		x=2*MirrorLineX-x;
		if(MirrorFieldXInverse){MirY*=-1;MirZ*=-1;}
		else{MirX*=-1;}
	}}
	if(MirrorYField){
	if(!(y>=fMiny && y<=fMaxy)){
		y=2*MirrorLineY-y;
		if(MirrorFieldYInverse){MirX*=-1;MirZ*=-1;}
		else{MirY*=-1;}
	}}
	if(MirrorZField){
	if(!(z>=fMinz && z<=fMaxz)){
		z=2*MirrorLineZ-z;
		if(MirrorFieldZInverse){MirX*=-1;MirY*=-1;}
		else{MirZ*=-1;}
	}}
	
	
	
	// Check that the point is within the defined region 
	if(x>=fMinx && x<=fMaxx &&
			y>=fMiny && y<=fMaxy && 
			z>=fMinz && z<=fMaxz) {

		// Position of given point within region, normalized to the range
		// [0,1]
		G4double xfraction = (x - fMinx)/fDx;
		G4double yfraction = (y - fMiny)/fDy; 
		G4double zfraction = (z - fMinz)/fDz;

		if(fInvertXIndex) { 
			xfraction = 1 - xfraction;
		}
		if(fInvertYIndex) { 
			yfraction = 1 - yfraction;
		}
		if(fInvertZIndex) { 
			zfraction = 1 - zfraction;
		}

		// Need addresses of these to pass to modf below.
		// modf uses its second argument as an OUTPUT argument.
		G4double xdindex, ydindex, zdindex;

		// Position of the point within the cuboid defined by the
		// nearest surrounding tabulated points
		G4double xlocal = std::modf(xfraction*(fNx-1), &xdindex);
		G4double ylocal = std::modf(yfraction*(fNy-1), &ydindex);
		G4double zlocal = std::modf(zfraction*(fNz-1), &zdindex);

		// The indices of the nearest tabulated point whose coordinates
		// are all less than those of the given point
		int xindex = static_cast<int>(xdindex);
		int yindex = static_cast<int>(ydindex);
		int zindex = static_cast<int>(zdindex);


#ifdef DEBUG_INTERPOLATING_FIELD
		G4cout<<"Local x,y,z: "<<xlocal<<" "<<ylocal<<" "<<zlocal<<std::endl;
		G4cout<<"Index x,y,z: "<<xindex<<" "<<yindex<<" "<<zindex<<std::endl;
		G4double valx0z0, mulx0z0, valx1z0, mulx1z0;
		G4double valx0z1, mulx0z1, valx1z1, mulx1z1;
		valx0z0= table[xindex  ][0][zindex];  mulx0z0=  (1-xlocal) * (1-zlocal);
		valx1z0= table[xindex+1][0][zindex];  mulx1z0=   xlocal    * (1-zlocal);
		valx0z1= table[xindex  ][0][zindex+1]; mulx0z1= (1-xlocal) * zlocal;
		valx1z1= table[xindex+1][0][zindex+1]; mulx1z1=  xlocal    * zlocal;
#endif

		// Full 3-dimensional version
		Bfield[0] =
			fXField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
			fXField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
			fXField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
			fXField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
			fXField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
			fXField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
			fXField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
			fXField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
		Bfield[1] =
			fYField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
			fYField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
			fYField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
			fYField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
			fYField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
			fYField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
			fYField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
			fYField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
		Bfield[2] =
			fZField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
			fZField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
			fZField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
			fZField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
			fZField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
			fZField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
			fZField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
			fZField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;

		// Rotate the BField here : Mhd : 25 Mar 2015  
		G4ThreeVector B(Bfield[0],  Bfield[1],  Bfield[2]);
// 		B.rotateZ(fZrotation*deg); // rotation made in the same direction of the lens
		Bfield[0] = B.getX()*MirX;
		Bfield[1] = B.getY()*MirY;
		Bfield[2] = B.getZ()*MirZ;

	} else {
		Bfield[0] = 0.0;
		Bfield[1] = 0.0;
		Bfield[2] = 0.0;
	}

}

