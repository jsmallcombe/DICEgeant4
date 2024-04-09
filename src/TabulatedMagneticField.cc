#include "TabulatedMagneticField.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ThreeVector.hh"

#include <algorithm> //for max_element

TabulatedMagneticField::TabulatedMagneticField(const char* filename) //called in 'nonuniformfield'
: fMaxbx(0),fMaxby(0),fMaxbz(0),fMaxM(0), fInvertXIndex(false), fInvertYIndex(false), fInvertZIndex(false), MirrorXField(false), MirrorYField(false), MirrorZField(false),MirrorFieldXInverse(false), MirrorFieldYInverse(false), MirrorFieldZInverse(false), MirrorLineX(0), MirrorLineY(0), MirrorLineZ(0), MirrorDeadX(0), MirrorDeadY(0), MirrorDeadZ(0), fOffx(0), fOffy(0), fOffz(0)
{    
	
	//////////////////////////
	/// Formatting of input file :
	/// Z Y X
	/// 20 10 10
	/// z1 y1 x1 bz by bx
	/// z1 y1 x2 bz by bx 
	/// ...
	
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
	
	int nX=0,nY=1,nZ=2;
	char n0='x',n1='y',n2='z';
	char c;
	file>>c;
	if(c=='y'||c=='Y'){nY=0;n0='y';}
	else if(c=='z'||c=='Z'){nZ=0;n0='z';}
	file>>c;
	if(c=='x'||c=='X'){nX=1;n1='x';}
	else if(c=='z'||c=='Z'){nZ=1;n1='z';}
	file>>c;
	if(c=='x'||c=='X'){nX=2;n2='x';}
	else if(c=='y'||c=='Y'){nY=2;n2='y';}
	
	int fN[3];
	// Read table dimensions 
	file>>fN[0]>>fN[1]>>fN[2]; 
	fNx=fN[nX];
	fNy=fN[nY];
	fNz=fN[nZ];

	G4cout<<"  [ Number of values x,y,z: " <<fNx<<" "<<fNy<<" "<<fNz<<" ] ";//111*111*106 gives 1306026 rows
	G4cout<<G4endl<<" Input order: "<<n0<<" "<<n1<<" "<<n2<<G4endl;

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
// 	G4double xval,yval,zval;
	G4double bx,by,bz;
	G4double xyxval[3],bval[3],ixyz[3];
	
	for(ixyz[0]=0; ixyz[0]<fN[0]; ixyz[0]++) {
		for(ixyz[1]=0; ixyz[1]<fN[1]; ixyz[1]++) {
			for(ixyz[2]=0; ixyz[2]<fN[2]; ixyz[2]++) {
// 				file >> xval >> yval >> zval >> bx >> by >> bz;
				file >> xyxval[0] >> xyxval[1] >> xyxval[2] >> bval[0] >> bval[1] >> bval[2];

				int ix=ixyz[nX],iy=ixyz[nY],iz=ixyz[nZ];
				
				if(ix==0 && iy==0 && iz==0) {//min is first value?? theres no sort/search 
					fMinx = xyxval[nX] * lenUnit;
					fMiny = xyxval[nY] * lenUnit;
					fMinz = xyxval[nZ] * lenUnit;
				}
				
				bx=bval[nX];
				by=bval[nY];
				bz=bval[nZ];
				
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

	fMaxx = xyxval[nX] * lenUnit; //now max dimension values are the last values - i.e. post-loop values
	fMaxy = xyxval[nY] * lenUnit;
	fMaxz = xyxval[nZ] * lenUnit;

// 	G4cout<<"\n ---> ... done reading "<<std::endl;
// 
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
		
	SetFieldMirrorPoint(G4ThreeVector(0,0,0));//Call needed to set dead zone correction
}


void TabulatedMagneticField::SetField(G4double MaxSetField){
	if(MaxSetField<0){
		MaxSetField=fMaxM*std::abs(MaxSetField/tesla);
	}
	
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
		
		MirrorDeadX=std::abs(fMinx-MirrorLineX);
		MirrorDeadY=std::abs(fMiny-MirrorLineY);
		MirrorDeadZ=std::abs(fMinz-MirrorLineZ);

		if(std::abs(fMaxx-MirrorLineX)<MirrorDeadX)MirrorDeadX=std::abs(fMaxx-MirrorLineX);
		if(std::abs(fMaxy-MirrorLineY)<MirrorDeadY)MirrorDeadY=std::abs(fMaxy-MirrorLineY);
		if(std::abs(fMaxz-MirrorLineZ)<MirrorDeadZ)MirrorDeadZ=std::abs(fMaxz-MirrorLineZ);
}

void TabulatedMagneticField::SetFieldOffsetVec(G4ThreeVector Mpoint){
  		fOffx=Mpoint.getX();
		fOffy=Mpoint.getY();
		fOffz=Mpoint.getZ();
}
  
void TabulatedMagneticField::GetFieldValue(const G4double point[4], G4double* Bfield) const
{
		Bfield[0] = 0.0;
		Bfield[1] = 0.0;
		Bfield[2] = 0.0;
		
	G4double x = point[0]+fOffx;
	G4double y = point[1]+fOffy;
	G4double z = point[2]+fOffz;

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

	
	// Control if the field vector is flipped in the mirror`
	G4int MirX=1,MirY=1,MirZ=1;
	
	if(MirrorXField){ // If this axis has a mirror
		
		if(std::abs(x-MirrorLineX)<MirrorDeadX){// If coord is in the dead region between mirror line and first point
			if((x-MirrorLineX)>0)x=MirrorLineX+MirrorDeadX; //move it TO the first point
			else x=MirrorLineX-MirrorDeadX;	
		}
			
		if(!(x>=fMinx && x<=fMaxx)){ // Is coord NOT in main field region
			x=2*MirrorLineX-x;// flip the coordinate in the mirror so it IS in the main region
			if(MirrorFieldXInverse){MirY*=-1;MirZ*=-1;} //Set filed mirroring parameters 
			else{MirX*=-1;}
		}
	}
	if(MirrorYField){
		if(std::abs(y-MirrorLineY)<MirrorDeadY){// If coord is in the dead region between mirror line and first point
			if((y-MirrorLineY)>0)x=MirrorLineY+MirrorDeadY; //move it TO the first point
			else y=MirrorLineY-MirrorDeadY;	
		}
		if(!(y>=fMiny && y<=fMaxy)){
			y=2*MirrorLineY-y;
			if(MirrorFieldYInverse){MirX*=-1;MirZ*=-1;}
			else{MirY*=-1;}
		}
	}
	if(MirrorZField){
		if(std::abs(z-MirrorLineZ)<MirrorDeadZ){// If coord is in the dead region between mirror line and first point
			if((z-MirrorLineZ)>0)x=MirrorLineZ+MirrorDeadZ; //move it TO the first point
			else z=MirrorLineZ-MirrorDeadZ;	
		}
		if(!(z>=fMinz && z<=fMaxz)){
			z=2*MirrorLineZ-z;
			if(MirrorFieldZInverse){MirX*=-1;MirY*=-1;}
			else{MirZ*=-1;}
		}
	}
	
	
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

		// There was a slight glitch with index+1 going over range when EXACTLY on the upper boundary.
		// Not sure how it never caused issues before
		if(xindex==fNx-1){xindex--;xlocal=1;}
		if(yindex==fNy-1){yindex--;ylocal=1;}
		if(zindex==fNz-1){zindex--;ylocal=1;}

#ifdef DEBUG_INTERPOLATING_FIELD
		std::cout<<"x,y,z: "<<x<<" "<<y<<" "<<z<<std::endl;
		std::cout<<"fMinx: "<<fMinx<<" "<<fMiny<<" "<<fMinz<<std::endl;
		std::cout<<"fDx: "<<fDx<<" "<<fDy<<" "<<fDz<<std::endl;
		std::cout<<"Fraction: "<<xfraction<<" "<<yfraction<<" "<<zfraction<<std::endl;
		std::cout<<"fNx: "<<fNx<<" "<<fNy<<" "<<fNz<<std::endl;
		std::cout<<"Local x,y,z: "<<xlocal<<" "<<ylocal<<" "<<zlocal<<std::endl;
		std::cout<<"ydindex x,y,z: "<<xdindex<<" "<<ydindex<<" "<<zdindex<<std::endl;
		std::cout<<"Index x,y,z: "<<xindex<<" "<<yindex<<" "<<zindex<<std::endl;
#endif
		
		// Full 3-dimensional version
		Bfield[0] =
			fXField[xindex  ][yindex  ][yindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
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

