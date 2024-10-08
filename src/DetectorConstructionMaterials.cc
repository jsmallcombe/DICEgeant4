// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

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
//
// $Id: DetectorConstruction.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
// 

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"

#include "G4SystemOfUnits.hh" // new version geant4.10 requires units

void DetectorConstruction::DefineMaterials()
{ 
    // use G4-NIST materials data base
    //
    G4NistManager* man = G4NistManager::Instance();
    man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    man->FindOrBuildMaterial("G4_AIR");
    man->FindOrBuildMaterial("G4_C");
    man->FindOrBuildMaterial("G4_BGO");
    
    G4double a, z, density, temperature, pressure;
    G4String name, symbol;
    G4int    nelements, ncomponents, natoms;
    G4double fractionmass;

    std::vector<G4Element*>  myElements;    // save pointers here to avoid
    std::vector<G4Material*> myMaterials;   // warnings of unused variables

    // Elements
    G4Element* elH = new G4Element(name="H", symbol="H", z=1., a = 1.00*g/mole);
    myElements.push_back(elH);

    G4Element* elC = new G4Element(name="C", symbol="C", z=6., a = 12.01*g/mole);
    myElements.push_back(elC);

    G4Element* elN  = new G4Element(name="N", symbol="N",  z=7.,  a= 14.00674*g/mole);
    myElements.push_back(elN);

    G4Element* elO  = new G4Element(name="O",   symbol="O",  z=8.,  a= 15.9994 *g/mole);
    myElements.push_back(elO);

    G4Element* elAr  = new G4Element(name="Ar",   symbol="Ar",  z=18.,  a= 39.948 *g/mole);
    myElements.push_back(elAr);

    G4Element* elLa = new G4Element(name="La", symbol="La", z=57., 138.9055*g/mole);
    myElements.push_back(elLa);

    G4Element* elBr = new G4Element(name="Br", symbol="Br", z=35., 79.904*g/mole);
    myElements.push_back(elBr);

    G4Element* elGe = new G4Element(name="Ge", symbol="Ge", z=32., 72.64*g/mole);
    myElements.push_back(elGe);

    G4Element* elI = new G4Element(name="I", symbol="I", z=53., 126.90*g/mole);
    myElements.push_back(elI);

    G4Element* elCs = new G4Element(name="Cs", symbol="Cs", z=55., 132.91*g/mole);
    myElements.push_back(elCs);

    G4Element* elTa = new G4Element(name="Ta", symbol="Ta", z=73., 180.95*g/mole);
    myElements.push_back(elTa);

    G4Element* elW = new G4Element(name="W", symbol="W", z=74., 183.84*g/mole);
    myElements.push_back(elW);

    G4Element* elBi = new G4Element(name="Bi", symbol="Bi", z=83., 208.98*g/mole);
    myElements.push_back(elBi);

    G4Element* elCe = new G4Element(name="Ce", symbol="Ce", z=58., 140.116*g/mole);
    myElements.push_back(elCe);

    G4Element* elNi = new G4Element(name="Ni", symbol="Ni", z=28., 58.69*g/mole);
    myElements.push_back(elNi);

    G4Element* elCu = new G4Element(name="Cu", symbol="Cu", z=29., 63.546*g/mole);
    myElements.push_back(elCu);

    G4Element* elNd = new G4Element(name="Nd", symbol="Nd", z=60., 144.242*g/mole);
    myElements.push_back(elNd);

    G4Element* elFe = new G4Element(name="Fe", symbol="Fe", z=26., 55.845*g/mole);
    myElements.push_back(elFe);

    G4Element* elB = new G4Element(name="B", symbol="B", z=5., 10.811*g/mole);
    myElements.push_back(elB);

    G4Element* elNa = new G4Element(name="Na", symbol="Na", z=11., 22.99*g/mole);
    myElements.push_back(elNa);

    G4Element* elLi = new G4Element(name="Li", symbol="Li", z=3. , a=  6.94   *g/mole);
    myElements.push_back(elLi);

    G4Element* elF  = new G4Element(name="F" , symbol="F" , z=9. , a= 18.9984 *g/mole);
    myElements.push_back(elF);

    G4Element* elSi = new G4Element(name="Si", symbol="Si", z=14., a= 28.0855 *g/mole);
    myElements.push_back(elSi);

    G4Element* elCr = new G4Element(name="Cr", symbol="Cr", z=24., a= 51.9961 *g/mole);
    myElements.push_back(elCr);

    G4Element* elPb = new G4Element(name="Pb", symbol="Pb", z=82., a=207.2    *g/mole);
    myElements.push_back(elPb);
    
    G4Element* elPd = new G4Element(name="Palladium", symbol="Pd", z=46., a=106.42    *g/mole);
    myElements.push_back(elPd);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    G4Material* Air = new G4Material(name="Air", density=1.29*mg/cm3, nelements=2);
    Air->AddElement(elN, .7);
    Air->AddElement(elO, .3);
    myMaterials.push_back(Air);

    // Laboratory vacuum: Dry air (average composition)
    density = 1.7836*mg/cm3 ;       // STP
    G4Material* Argon = new G4Material(name="Argon", density, ncomponents=1);
    Argon->AddElement(elAr, 1);

    density = 1.25053*mg/cm3 ;       // STP
    G4Material* Nitrogen = new G4Material(name="N2", density, ncomponents=1);
    Nitrogen->AddElement(elN, 2);

    density = 1.4289*mg/cm3 ;       // STP
    G4Material* Oxygen = new G4Material(name="O2", density, ncomponents=1);
    Oxygen->AddElement(elO, 2);

    // LaboratoryVacuum
    density  = 1.2928*mg/cm3 ;       // STP
    density *= 1.0e-8 ;              // pumped vacuum
    temperature = STP_Temperature;
    pressure = 1.0e-8*STP_Pressure;
    G4Material* Vacuum = new G4Material(name="Vacuum",
                                        density,ncomponents=3,
                                        kStateGas,temperature,pressure);
    Vacuum->AddMaterial( Nitrogen, fractionmass = 0.7557 ) ;
    Vacuum->AddMaterial( Oxygen, fractionmass = 0.2315 ) ;
    Vacuum->AddMaterial( Argon,fractionmass = 0.0128 ) ;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    G4Material* Water = new G4Material(name="Water", density=1000*kg/m3, nelements=2);
    Water->AddElement(elH, natoms=2);
    Water->AddElement(elO, natoms=1);
    myMaterials.push_back(Water);

    G4Material* Al = new G4Material(name="Aluminium", z=13., a= 26.98154*g/mole, density= 2.70  *g/cm3);
    myMaterials.push_back(Al);

    G4Material* W = new G4Material(name="Tungsten", z=74., a= 183.84*g/mole, density= 19.25*g/cm3);
    myMaterials.push_back(W);
    
    G4Material* Cu = new G4Material("Copper", z=29., a = 63.546*g/mole, density = 8960*kg/m3);
    myMaterials.push_back(Cu);

    G4Material* Si = new G4Material(name="Silicon", z=14., a= 28.0855*g/mole, density= 2.330  *g/cm3);
    myMaterials.push_back(Si);

    G4Material* Ti = new G4Material(name="Titanium", z=22., 47.867*g/mole, 4.54*g/cm3); //
    myMaterials.push_back(Ti);
    
    G4Material* Ni = new G4Material(name="Nickle", z=28., 58.69*g/mole, 8.90*g/cm3); //
    myMaterials.push_back(Ni);
    
    G4Material* Sn = new G4Material(name="Tin", z=50., 118.71*g/mole, 6.99*g/cm3);//
    myMaterials.push_back(Sn);

    G4Material* Au = new G4Material(name="Gold", z=79., a= 196.9666*g/mole, density= 19.30  *g/cm3);
    myMaterials.push_back(Au);

    G4Material* Ag = new G4Material(name="Silver", z=47., a= 107.8682*g/mole, density=10.49  *g/cm3);
    myMaterials.push_back(Ag);

    G4Material* Pb = new G4Material(name="Lead", z=82., a= 207.19*g/mole, density= 11.35  *g/cm3);
    myMaterials.push_back(Pb);

    G4Material* Ge = new G4Material(name="Germanium", z=32., a= 72.64*g/mole, density=5.323 *g/cm3);
    myMaterials.push_back(Ge);

    G4Material* Hevimetal = new G4Material("Hevimetal", density=19.0*g/cm3, nelements=3);
    Hevimetal->AddElement(elTa, 80*perCent);
    Hevimetal->AddElement(elNi, 13*perCent);
    Hevimetal->AddElement(elCu, 7*perCent);
    myMaterials.push_back(Hevimetal);

    G4Material* LN2 = new G4Material("LiquidN2", density=804*kg/m3, ncomponents=1);
    LN2->AddElement(elN, natoms=2);
    myMaterials.push_back(LN2);

    G4Material* Delrin = new G4Material("Delrin", density = 1.4*g/cm3, ncomponents=3);
    Delrin->AddElement(elC, natoms=1);
    Delrin->AddElement(elO, natoms=1);
    Delrin->AddElement(elH, natoms=2);
    myMaterials.push_back(Delrin);

    G4Material* Be = new G4Material("Beryllium", z=4., a=9.012*g/mole, density=1848*kg/m3);
    myMaterials.push_back(Be);

    G4Material* Zn = new G4Material("Zinc"   , 30., 65.409*g/mole, 7.14*g/cm3);
    myMaterials.push_back(Zn);

    G4Material* Brass= new G4Material("Brass", 8.5*g/cm3, 2);
    Brass->AddMaterial(Cu  , 70*perCent);
    Brass->AddMaterial(Zn , 30*perCent);
    myMaterials.push_back(Brass);

    G4Material* Mylar = new G4Material("Mylar", density = 1.397*g/cm3, ncomponents=3);
    Mylar->AddElement(elC, natoms=10);
    Mylar->AddElement(elH, natoms=8);
    Mylar->AddElement(elO, natoms=4);
    myMaterials.push_back(Mylar);

    G4Material* NdFeB = new G4Material("NdFeB", density = 7.45*g/cm3, ncomponents=3);  //From Wikipedia
    NdFeB->AddElement(elNd, natoms=2);
    NdFeB->AddElement(elFe, natoms=14);
    NdFeB->AddElement(elB, natoms=1);
    myMaterials.push_back(NdFeB);

    G4Material* iron = new G4Material("Iron", density = 7.874*g/cm3, ncomponents=1);  //From Google
    iron->AddElement(elFe, natoms=1);
    myMaterials.push_back(iron);

    G4Material* Teflon = new G4Material("Teflon", density = 2.2*g/cm3, ncomponents=2);
    Teflon->AddElement(elC, natoms=2);
    Teflon->AddElement(elF, natoms=4);
    myMaterials.push_back(Teflon);

    G4Material* SiLi = new G4Material("Si(Li)", density = 2.330*g/cm3, ncomponents=2);
    SiLi->AddElement(elSi, 99.999*perCent);
    SiLi->AddElement(elLi,  0.001*perCent);
    myMaterials.push_back(SiLi);

    G4Material* Peek = new G4Material("Peek",density = 1.26*g/cm3, ncomponents=3);
    Peek->AddElement(elC, natoms=19);
    Peek->AddElement(elH, natoms=12);
    Peek->AddElement(elO, natoms=3);
    myMaterials.push_back(Peek);

    G4Material* WTa = new G4Material("WTa",density = 18677*kg/m3, ncomponents=2 );
    WTa->AddElement(elW, 80.*perCent);
    WTa->AddElement(elTa, 20.*perCent);
    myMaterials.push_back(WTa);
    

    G4Material* WNiCu = new G4Material("WNiCu",density = 18460*kg/m3, ncomponents=3 );
    WNiCu->AddElement(elW, 98.5*perCent);
    WNiCu->AddElement(elNi, 1.0*perCent);
    WNiCu->AddElement(elCu, 1.5*perCent);
    myMaterials.push_back(WNiCu);

    G4Material* Kapton = new G4Material("Kapton",density = 1.43*g/cm3, ncomponents=4);
    Kapton->AddElement(elC,natoms=22);
    Kapton->AddElement(elH,natoms=10);
    Kapton->AddElement(elN,natoms=2);
    Kapton->AddElement(elO,natoms=5);
    myMaterials.push_back(Kapton);
    
    G4Material* Acrylic = new G4Material(name="Acrylic", density=1.18*g/cm3, nelements=3);
    Acrylic->AddElement(elC, natoms=5);
    Acrylic->AddElement(elO, natoms=2);
    Acrylic->AddElement(elH, natoms=8);
    myMaterials.push_back(Acrylic);

    //Epoxy (for FR4 )
	//from http://www.physi.uni-heidelberg.de/~adler/TRD/TRDunterlagen/RadiatonLength/tgc2.htm //???
	G4Material* Epoxy = new G4Material("Epoxy" , 1.2*g/cm3,2);
	Epoxy->AddElement(elH, 2);
	Epoxy->AddElement(elC, 2);
    
	//SiO2 (Quarz)
	G4Material* SiO2 = new G4Material("SiO2",2.200*g/cm3,2);
	SiO2->AddElement(elSi, 1);
	SiO2->AddElement(elO , 2);

	G4Material* FR4 = new G4Material("FR4" , 1.86*g/cm3,2);
	FR4->AddMaterial(Epoxy, 0.472);
	FR4->AddMaterial(SiO2,0.528);
    myMaterials.push_back(FR4);
    
    
    G4Material* EJ212 = new G4Material("EJ212",density = 1.023*g/cm3, ncomponents=2);
    EJ212->AddElement(elC,natoms=47);
    EJ212->AddElement(elH,natoms=52);
    myMaterials.push_back(EJ212);
    
    

    
    
    //G4cout << *(G4Material::GetMaterialTable()) << G4endl; //outputs material table to terminal
    
}
