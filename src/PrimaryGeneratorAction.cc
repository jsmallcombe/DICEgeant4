//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file analysis/shared/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.cc 68015 2013-03-13 13:27:27Z gcosmo $
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh" // for command-based data input

#include "Global.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4LorentzVector.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Geantino.hh"
#include "Randomize.hh"



#include "DetectorConstruction.hh" //for detector based information

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

	PrimaryGeneratorAction::PrimaryGeneratorAction(HistoManager* histoManager)
: G4VUserPrimaryGeneratorAction(),
	fParticleGun(nullptr),
	fDetector(histoManager->GetDetectorConstruction()),
	fHistoManager(histoManager)
{
	G4int nParticle = 1;
	fParticleGun  = new G4ParticleGun(nParticle); //In our code, the gun is called fParticleGun
	//create a messenger for this class
	fGunMessenger = new PrimaryGeneratorMessenger(this);

	//these 3 lines initialise the Gun, basic values
	fParticleGun->SetParticleMomentum(0*eV);
	fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

	//defaults for gun properties
	fEffEnergy = 0.0;
	fEffDirection = G4ThreeVector(0.0*mm,0.0*mm,0.0*mm);
	fEffDirectionBool = false;//initialises bools, if command entered will go to true and loops below entered
	fEffPositionBool = false;
	fEffParticleBool = false;
	fEffGunBetaZ = 0;
	fEffGunLifetime = 0;

	fBeamSpotSigma = 0.*mm;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fParticleGun;
	delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
		
		// Changed so that most grsi "/Detsys/gun/" commands still effect gun when using
		// Underlying geant4 commands such as '/gun/particle ion" & "/gun/ion"
		G4double m0=0;
		if(fEffParticleBool) {
			G4ParticleDefinition* effPart;
			if(fEffParticle == "electron" || fEffParticle == "e-") {
				effPart = G4ParticleTable::GetParticleTable()->FindParticle("e-");
			} else if(fEffParticle == "positron" || fEffParticle == "e+") {
				effPart = G4ParticleTable::GetParticleTable()->FindParticle("e+");
			} else if(fEffParticle == "neutron"){
				effPart = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
			} else {
				effPart = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
			}
			fParticleGun->SetParticleDefinition(effPart);
			m0=effPart->GetPDGMass();
		}


		//////////
		//ORIGIN CONTROLS 
		//////////        

		G4double x = 0.*mm;
		G4double y = 0.*mm;
		G4double z = 0.*mm;
		if(fEffPositionBool){
			x = fEffPosition.x();
			y = fEffPosition.y();
			z = fEffPosition.z();	
		}

		// If we want to simulate a realistic beam spot, instead of perfect pencil beam.
		if(fBeamSpotSigma>0){
			x = G4RandGauss::shoot(x,fBeamSpotSigma)*mm;
			y = G4RandGauss::shoot(y,fBeamSpotSigma)*mm;
		}

		//////////
		//DIRECTION CONTROLS (directly forced, beam, or cone)
		//////////
		G4double effRandCosTheta, effRandSinTheta, effRandPhi;
		G4ThreeVector effdirection;
		if(fEffDirectionBool) { 
			effdirection = fEffDirection;

			if(fConeAngleBool){
				//min max input order doesnt actually matter
				G4double cmin =cos(fAngleMinInit);
				G4double cmax =cos(fAngleInit);
				G4double CosTheta = G4UniformRand()*abs(cmax-cmin);
				if(cmin<cmax)CosTheta+=cmin;
				else CosTheta+=cmax;

				// 	      G4cout<<asin(SinTheta)<<G4endl;
				G4double SinTheta = sqrt(1. - pow(CosTheta, 2.0));
				G4double Phi      = (2.0*CLHEP::pi)*G4UniformRand();

				effdirection = G4ThreeVector(SinTheta*cos(Phi), SinTheta*sin(Phi), CosTheta);
				// G4cout<<effdirection<<G4endl;

			}	  
		} else {
			//G4cout<<"Random "<< G4endl; //may offer the solution, an altered 2pi rando. Using 4pi for efficiency
			// random direction if no preference provided
			effRandCosTheta = 2.*G4UniformRand()-1.0; //cos(theta) = 2cos^2(0.5theta)-1 ??
			effRandSinTheta = sqrt(1. - effRandCosTheta*effRandCosTheta); //from sin^2(theta)+cos^2(theta)=1
			effRandPhi      = (360.*deg)*G4UniformRand();
			effdirection = G4ThreeVector(effRandSinTheta*cos(effRandPhi), effRandSinTheta*sin(effRandPhi), effRandCosTheta);
			//converts from Spherical polar(physics def.) to cartesian via (rsin(theta)cos(phi),rsin(theta)cos(phi),rcos(theta)) r=1,unit length
		}

		
		//after running through if-statements above we now have particle type definition, position, mom. direction, and the energy (or their initialised values)
		
		
		if(fEffGunBetaZ>0){
			effdirection.setMag(sqrt(pow(m0+fEffEnergy,2)-m0*m0));
			G4LorentzVector LorVec=G4LorentzVector(effdirection,m0+fEffEnergy);
			LorVec.boostZ(fEffGunBetaZ);
			fParticleGun->SetParticleMomentumDirection(LorVec.getV());
			fParticleGun->SetParticleEnergy(LorVec.getT()-m0);
			
			if(fEffGunLifetime>0){
				double t=G4RandExponential::shoot(fEffGunLifetime);
				t/=sqrt(1-fEffGunBetaZ*fEffGunBetaZ);
				z+=t*CLHEP::c_light*fEffGunBetaZ;
			}
			
		}else{
			fParticleGun->SetParticleEnergy(fEffEnergy);
			fParticleGun->SetParticleMomentumDirection(effdirection);
		}
		
		G4ThreeVector thisEffPosition = G4ThreeVector(x,y,z);//in constructor
		fParticleGun->SetParticlePosition(thisEffPosition);
		
        if(fDetector->RecordingGun()){
            fHistoManager->BeamEnergy(fEffEnergy);	
            fHistoManager->BeamTheta(effdirection.theta());
            fHistoManager->BeamPhi(effdirection.phi());
            fHistoManager->BeamPos(thisEffPosition);
        }


	//create vertex
	//
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::PassEfficiencyPosition(G4ThreeVector  num){
	fDetector->PassEfficiencyPosition(num);
}

