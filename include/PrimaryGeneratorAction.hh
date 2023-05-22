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
/// \file analysis/shared/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh 68015 2013-03-13 13:27:27Z gcosmo $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include "PrimaryGeneratorMessenger.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

class G4ParticleGun;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
private:    
    G4ParticleGun*                fParticleGun;  //pointer a to G4 class
    DetectorConstruction*         fDetector;     //pointer to the geometry
    HistoManager*                 fHistoManager; //pointer to the histo manager
    PrimaryGeneratorMessenger*    fGunMessenger; //messenger of this class

public:
    PrimaryGeneratorAction(HistoManager*);
    virtual ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event*);
    

    void SetEfficiencyEnergy(G4double num) { fEffEnergy = num; fHistoManager->BeamEnergy(num); }
    void SetEfficiencyDirection(G4ThreeVector num) { fEffDirection = num; fEffDirectionBool = true; }
    
    void PassEfficiencyPosition(G4ThreeVector num);
    void SetEfficiencyPosition(G4ThreeVector num) { fEffPosition = num; fEffPositionBool = true; PassEfficiencyPosition(fEffPosition); }
    
    void SetEfficiencyParticle(G4String val) { fEffParticle = val; fEffParticleBool = true; }
    void SetConeMaxAngle(G4double num1) { fAngleInit = num1; fConeAngleBool = true; fEffDirectionBool = true; }
    void SetConeMinAngle(G4double num1) { fAngleMinInit = num1; }
    void SetConeMinPhi(G4double num1) { fPhiMin = num1; }
    void SetConeMaxPhi(G4double num1) { fPhiMax = num1; }
    void SetBeamSpotSigma(G4double num1) { fBeamSpotSigma = num1; }
    void SetGunBeta(G4double num1) { fEffGunBetaZ = num1; }
    void SetGunLifetime(G4double num1) { fEffGunLifetime = num1; }
    void StoreHits(){fHistoManager->StoreHits();}
    void SetStoreMult(G4int num1){fHistoManager->SetStoreMult(num1);}
    void SetPrettyHitsOnly(){fHistoManager->SetPrettyHitsOnly();}
    
    
    
private:
    //variables
    G4double fEffEnergy;
    G4ThreeVector fEffDirection;
    G4ThreeVector fEffPosition;
    G4bool fEffDirectionBool;
    G4bool fEffPositionBool;
    G4String fEffParticle;
    G4bool fEffParticleBool;
	
	G4double fEffGunBetaZ;	
	G4double fEffGunLifetime;	
    
    G4double fAngleInit;
    G4bool fConeAngleBool;
    G4double fAngleMinInit;
    G4double fPhiMin;
    G4double fPhiMax;
    G4double fBeamSpotSigma;      
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


