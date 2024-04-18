void GammaRawSort(const char * outfileName = "GammaRawSort.root", const char *ntuplefileName = "g4out.root", string folder= ""){
	
    // Load the input file
    TFile *ntuplefile = new TFile(ntuplefileName,"READ");
	gROOT->cd();
    if(!ntuplefile->IsOpen())return;
    TTree *newtree = (TTree*)ntuplefile->Get("ntuple");
    if(newtree==nullptr)return;


	TFile out(outfileName,"UPDATE");
	if(folder.size()){
		out.mkdir(folder.c_str());
		out.cd(folder.c_str());
	}
	
		TH2F* ERawSeg=new TH2F("ERawSeg","ERawSeg;Energy (keV);Seg",200,0,200,17,0,17);
		TH1F* ERaw=new TH1F("ERaw","ERaw;Energy (keV);Counts",400,0,200);
		TH1F* DirectRaw=new TH1F("DirectRaw","DirectRaw;Energy (keV);Counts",400,0,200);
		TH1F* ComptonRaw=new TH1F("ComptonRaw","ComptonRaw;Energy (keV);Counts",400,0,200);
	
		TH1F* EPixel=new TH1F("EPixel","EPixel;Energy (keV);Counts",400,0,200);
		TH1F* EGuard=new TH1F("EGuard","EGuard;Energy (keV);Counts",400,0,200);
		
		TH1F* ComptonPixel=new TH1F("ComptonPixel","ComptonPixel;Energy (keV);Counts",400,0,200);
		TH1F* ComptonGuard=new TH1F("ComptonGuard","ComptonGuard;Energy (keV);Counts",400,0,200);
		
		TH1F* DirectPixel=new TH1F("DirectPixel","DirectPixel;Energy (keV);Counts",400,0,200);
		TH1F* DirectGuard=new TH1F("DirectGuard","DirectGuard;Energy (keV);Counts",400,0,200);
		
		TH2F* EComptonSeg=new TH2F("EComptonSeg","EComptonSeg;Energy (keV);Seg",200,0,200,17,0,17);
		TH2F* EDirectSeg=new TH2F("EDirectSeg","EComptonSeg;Energy (keV);Seg",200,0,200,17,0,17);
		
		TH1F* AlLayerHit=new TH1F("AlLayerHit","AlLayerHit;Energy (keV)",200,0,200);
		TH2F* ERawVsTrue=new TH2F("ERawTrue","ERawTrue;Energy (keV);Gun Energy",400,0,200,20,0,200);
	
		TH1F* Miss=new TH1F("Miss","Miss;(mm);Counts",500,-1,99);
		TH2F* MissE=new TH2F("MissVsE","MissVsE;(mm);(keV);Counts",20,-1,99,200,0,200);
		TH2F* MissEPixel=new TH2F("MissEPixel","MissEPixel;(mm);(keV);Counts",20,-1,99,200,0,200);
		TH2F* MissEGuard=new TH2F("MissEGuard","MissEGuard;(mm);(keV);Counts",20,-1,99,200,0,200);
		TH1F* ERawFull=new TH1F("ERawFull","ERawFull;Energy (keV);Counts",2000,0,2000);

	gROOT->cd();//cd back into main session memory 
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	int detNumber,cryNumber;
	double depEnergy,primaryE;
	double posx,posy,posz;
	double primaryTheta,primaryPhi;
	double originX,originY,originZ;
	
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("primaryE",&primaryE);
	newtree->SetBranchAddress("posx",&posx);
	newtree->SetBranchAddress("posy",&posy);
	newtree->SetBranchAddress("posz",&posz);
	newtree->SetBranchAddress("primaryTheta",&primaryTheta);
	newtree->SetBranchAddress("primaryPhi",&primaryPhi);
	newtree->SetBranchAddress("originX",&originX);
	newtree->SetBranchAddress("originY",&originY);
	newtree->SetBranchAddress("originZ",&originZ);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

		if(cryNumber==4){
			AlLayerHit->Fill(primaryE);
		}
		if(cryNumber==0){
			
			double rho =(posy-originY)/(sin(primaryTheta)*sin(primaryPhi));
			double x =rho*sin(primaryTheta)*cos(primaryPhi);
			x+=originX;
			double z =rho*cos(primaryTheta);
			z+=originZ;
			double m=sqrt(pow(x-posx,2)+pow(z-posz,2));
			bool scatter=(m>1);
			Miss->Fill(m);
			MissE->Fill(m,depEnergy);
			
			ERaw->Fill(depEnergy);
			ERawFull->Fill(depEnergy);
			ERawSeg->Fill(depEnergy,detNumber);
			ERawVsTrue->Fill(depEnergy,primaryE);
			
			if(scatter){
				ComptonRaw->Fill(depEnergy);
				EComptonSeg->Fill(depEnergy,detNumber);
			}else{
				DirectRaw->Fill(depEnergy);
				EDirectSeg->Fill(depEnergy,detNumber);
			}
			
			if(detNumber>0&&detNumber<17){//0 is guard ring
				EPixel->Fill(depEnergy);
				MissEPixel->Fill(m,depEnergy);
				if(scatter){
					ComptonPixel->Fill(depEnergy);
				}else{
					DirectPixel->Fill(depEnergy);
				}
			}else if(detNumber==0){
				EGuard->Fill(depEnergy);
				MissEGuard->Fill(m,depEnergy);
				if(scatter){
					ComptonGuard->Fill(depEnergy);
				}else{
					DirectGuard->Fill(depEnergy);
				}
			}
			
			
		}

	}
	
	if(MissEPixel->GetMaximum()<5)MissEPixel->SetMaximum(5);
	if(MissEGuard->GetMaximum()<5)MissEGuard->SetMaximum(5);
	if(MissE->GetMaximum()<5)MissE->SetMaximum(5);
	
    out.Write("", TObject::kOverwrite);;
    out.Close();	
    delete ntuplefile; // Delete the variable holding the input file, which also closes it
    
}

