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
/// \file analysis/AnaEx01/include/HistoManager.hh
/// \brief Definition of the HistoManager class
//
//
// $Id: HistoManager.hh 74272 2013-10-02 14:48:50Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef HISTOMANAGER_HH
#define HISTOMANAGER_HH

#include "globals.hh"
#include "g4root.hh"

#include "G4SystemOfUnits.hh" // new version geant4.10 requires units

#include "DetectorConstruction.hh"

const G4int MAXNTCOL            = 21;

const G4bool WRITEEKINHISTOS    = true;//bools needed to write histos
const G4bool WRITEEDEPHISTOS    = true;
const G4bool WRITETRACKLHISTOS  = true;

const G4int MAXHISTO            = 500;//max number of histos in root file
const G4int MAXNUMDET           = 20;
const G4int MAXNUMDETPACES	= 5;
const G4int MAXNUMDETGRIFFIN    = 16;
const G4int MAXNUMCRYGRIFFIN    = 4;
const G4int NUMPARTICLETYPES    = 20;

// ekin histo properties    ///////////////////////
const G4int     EKINNBINS  = 10000;
const G4double  EKINXMIN   = 0.5*keV;
const G4double  EKINXMAX   = 10000.5*keV;

// edep histo properties    ///////////////////////
const G4int     EDEPNBINS  = 10000;//was 10000
const G4double  EDEPXMIN   = 0.0*keV;
const G4double  EDEPXMAX   = 10000.0*keV;//was 10000.5

// trackl histo properties  ///////////////////////
const G4int     TRACKLNBINS = 5000;
const G4double  TRACKLXMIN  = 0.5*mm;
const G4double  TRACKLXMAX  = 5000.5*mm;

///////////////////////////////////////////////////
const G4double MINENERGYTHRES   = 0.001*keV;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager {
public:
	HistoManager(DetectorConstruction*);
	~HistoManager();

	void Book();
	void Save();

	void FillHitNtuple(G4int eventNumber, G4int trackID, G4int parentID, G4int stepNumber, G4int particleType, G4int processType, G4int systemID, G4int cryNumber, G4int detNumber, G4double depEnergy, G4double posx, G4double posy, G4double posz, G4double time, G4int targetZ);
	void FillStepNtuple(G4int eventNumber, G4int trackID, G4int parentID, G4int stepNumber, G4int particleType, G4int processType, G4int systemID, G4int cryNumber, G4int detNumber, G4double depEnergy, G4double posx, G4double posy, G4double posz, G4double time, G4int targetZ);

// 	void FillHistogram(G4int ih, G4double e, G4double weight = 1.0);
// 	void Fill2DHistogram(G4int ih, G4double xbin, G4double ybin, G4double weight = 1.0);
// 	void Normalize(G4int id, G4double fac);

	void PrintStatistic();

	DetectorConstruction* GetDetectorConstruction() { return fDetectorConstruction; }

	G4bool GetStepTrackerBool() { return fStepTrackerBool; }
	G4bool GetHitTrackerBool()  { return fHitTrackerBool; }

	void BeamEnergy(G4double val) { fBeamEnergy = val*1000.; }//default is in MeV, but other outputs are keV
	void BeamTheta(G4double val)  { fBeamTheta = val; }
	void BeamPhi(G4double val)    { fBeamPhi = val; }
	void BeamPos(G4ThreeVector val)    { fBeamPos = val; }

	G4double BeamEnergy() { return fBeamEnergy; }
	G4double BeamTheta() { return fBeamTheta; }
	G4double BeamPhi() { return fBeamPhi; }
	G4ThreeVector BeamPos() {return fBeamPos; }
	
	bool HitsStore;
	void StoreHits(){HitsStore=true;}

private:
// 	void MakeHistogram(G4AnalysisManager* analysisManager, G4String filename,  G4String title, G4double xmin, G4double xmax, G4int nbins);
// 	void Make2DHistogram(G4AnalysisManager* analysisManager, const G4String& name, const G4String& title,
// 	             G4int nxbins, G4double xmin, G4double xmax,
// 	             G4int nybins, G4double ymin, G4double ymax);

	G4String G4intToG4String(G4int value);

	DetectorConstruction* fDetectorConstruction;
	G4bool        fFactoryOn;
	G4int         fMakeHistogramIndex;
	G4String      fFileName[2];

// 	G4int         fHistId[MAXHISTO];
// 	G4AnaH1*      fHistPt[MAXHISTO];
// 	G4AnaH2*      fHistPt2[MAXHISTO];

	G4int         fNtColId[MAXNTCOL];
	G4int         fNtColIdHit[MAXNTCOL];
	G4int         fNtColIdStep[MAXNTCOL];

	G4bool fStepTrackerBool;
	G4bool fHitTrackerBool;

	G4double fBeamEnergy;
	G4double fBeamTheta;
	G4double fBeamPhi;
	G4ThreeVector fBeamPos;

public:
	short PacesHistNumbers(int i) { return fPacesHistNumbers[i]; }
	short AngleDistro(int i) { return fAngleDistro[i]; }
private:
	short fPacesHistNumbers[MAXNUMDETPACES+2]; //+2 for edep and sum histos 
	short fAngleDistro[10]; //this variable will hold the histogran ID for various beam distribution histograms
};

enum HISTONAME
{
    kNullName = 0,
    kAstatsParticleTypeInEachStep,
    kAstatsParticleTypeInEachEvent
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

