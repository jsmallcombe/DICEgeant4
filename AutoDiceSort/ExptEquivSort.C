void ExptEquivSort(const char * DetDataFileName,const char * HistFolder,double beta=0,const char * outfileName = "DicePostSort.root", const char *ntuplefileName = "g4out.root",int Nevent=0,double dE=0) {
	    
	//// Random number generator to add electronic noise on to "perfect" geant4 detectors ///
	TRandom r;
	r.SetSeed();
	double noisefactor=2;
	// These factors should be tuned to data when possible 


    // Load the input file
    TFile *ntuplefile = new TFile(ntuplefileName,"READ");
	gROOT->cd();
    if(!ntuplefile->IsOpen())return;
    TTree *newtree = (TTree*)ntuplefile->Get("ntuple");
    if(newtree==nullptr)return;

	
	TFile DetDataFile(DetDataFileName,"READ");
	gROOT->cd();
	
	TGraph* AngleExpt[16][4];
	if(std::abs(beta)>0){
		for(int b=0;b<16;b++){
			for(int m=0;m<4;m++){
				stringstream ss;
				ss<<"ThetaEval/grid"<<b<<"_"<<m;
				TGraph *grf = (TGraph*)DetDataFile.Get(ss.str().c_str());
				
				if(grf){
					AngleExpt[b][m]=grf;
				}else{
					AngleExpt[b][m]=nullptr;
					cout<<endl<<"No Graph "<<b<<" "<<m<<endl;
				}
			}
		}
	}
	

	
	
	TFile out(outfileName,"UPDATE");
	out.mkdir(HistFolder);
	gROOT->cd();
	
	
	TH2 *gdch = (TH2*)DetDataFile.Get("LowChanCut/E_Chan_GoodECut");
	if(!gdch){
		cout<<endl<<"No Good Chan"<<endl;
		return;
	}
	TH2 *gdchtheta = (TH2*)DetDataFile.Get("LowChanCut/E_Chan_GoodEThetaLimCut");
	if(!gdchtheta){
		cout<<endl<<"No Good Chan Theta"<<endl;
		return;
	}
	TH2 *gdchthetahalf = (TH2*)DetDataFile.Get("LowChanCut/E_Chan_GoodEThetaLimHalfCut");
	if(!gdchthetahalf){
		cout<<endl<<"No Good Chan ThetaHalf"<<endl;
		return;
	}
	
	TH2* GdTH[4]={gdchtheta,gdchtheta,gdchthetahalf,gdchthetahalf};
	
	out.cd(HistFolder);//cd into output file so histograms created in memory are associated with that file at creation rather than being manually saved there later.

		TH1F* E_RawSum=new TH1F("Raw","RawHitEnergy;Electron Energy (keV);Counts",2000,0,2000);
		TH1F* E_AddbackSum=new TH1F("AddbackSum","AddbackSum;Electron Energy (keV);Counts",2000,0,2000);
		TH1F* E_AddbackVetoSum=new TH1F("AddbackVetoSum","AddbackVetoSum;Electron Energy (keV);Counts",2000,0,2000);
		TH2F* AddbackVetoSumChan=new TH2F("AddbackVetoSumChan","AddbackVetoSumChan;Electron Energy (keV);Counts",2000,0,2000,16,0,16);
		TH1F* EGridGated=new TH1F("EGridGated","EGridGated;Electron Energy (keV);Counts",2000,0,2000);
		TH1F* E_Corr[4];
		TH1* FitHist;
		TH2F* E_CorrGrid;
		TH2F* CoMExptGrid[16];
		if(std::abs(beta)>0){
			E_Corr[0]=new TH1F("E_CorrMean","E_CorrMean;Electron Energy (keV);Counts",2000,0,2000);
			E_Corr[1]=new TH1F("E_CorrMode","E_CorrMode;Electron Energy (keV);Counts",2000,0,2000);
			E_Corr[2]=new TH1F("E_CorrMeanStrict","E_CorrMeanStrict;Electron Energy (keV);Counts",2000,0,2000);
			E_Corr[3]=new TH1F("E_CorrModeStrict","E_CorrModeStrict;Electron Energy (keV);Counts",2000,0,2000);
			
			E_CorrGrid=new TH2F("E_CorrGrid","E_CorrGrid;Electron Energy (keV);Grid #",2000,0,2000,16,0,16);
			
			if(dE){
				stringstream folder;
				folder<<HistFolder<<"/CoMExptGrid";
				out.mkdir(folder.str().c_str());
				out.cd(folder.str().c_str());

				int Ebin = round(2000/dE);
				double EbinMin=dE*0.5;
				double EbinMax=EbinMin+dE*Ebin;
	
				for(int b=0;b<16;b++){
					stringstream ss;
					ss<<"TrueEDetGrid"<<b;
					
					CoMExptGrid[b]=new TH2F(ss.str().c_str(),(ss.str()+";Hit Energy (keV);Emission Energy (keV);Counts").c_str(),2000,0,2000,Ebin,EbinMin,EbinMax);
				}
				
				out.cd(HistFolder);
			}
		}
		
		TH1 *Eff,*EffFit,*SigFit,*Metric;
		if(Nevent>0){
			Eff=(TH1*)DetDataFile.Get("Efficiency")->Clone("Eff");
			Eff->Reset();
			EffFit=(TH1*)Eff->Clone("EffFit");
			SigFit=(TH1*)Eff->Clone("SigFit");
			SigFit->SetTitle("Fit Sigma;Electron Energy (keV);Sigma [keV]");
			Metric=(TH1*)Eff->Clone("Metric");
			Metric->SetTitle("Metric;Electron Energy (keV);Quality Metric");
		}
	gROOT->cd();//cd back into main session memory 
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int EventID;
	int detNumber,cryNumber;
	double depEnergy,primaryE,primaryTheta;
	double posx,posz;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("primaryE",&primaryE);
	newtree->SetBranchAddress("primaryTheta",&primaryTheta);
	newtree->SetBranchAddress("posx",&posx);
	newtree->SetBranchAddress("posz",&posz);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<double> EventHolder(16,0); // A matrix to store the energies recorded in an event
    
    bool VETO1=false;
    bool VETO2=false;
    double Eaddback=0;
    double Theta=0;
    double Mult=0;
    double TMult=0;
    int Seg;
	
    double pX,pZ;

	TAxis* Ebinax=gdch->GetXaxis();
	
	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
            double e=depEnergy+r.Gaus(0,noisefactor);
//             double e=depEnergy;
			
            if(cryNumber==1&&e>100){
                VETO1=true;
            }
            if(cryNumber==2&&e>100){
                VETO2=true;
            }
            if(cryNumber==0&&e>0.1){
                if(detNumber>0&&detNumber<17){//0 is guard ring
                    Eaddback+=e;
                    detNumber--; // Shift to zero index
                    EventHolder[detNumber]=e;
                    E_RawSum->Fill(e);
                    Mult++;
                    if(Mult==1){
                        pX=posx;
                        pZ=posz;
                    }
                    Seg=detNumber;
                }
            }
            Theta=primaryTheta;
            TMult++;
            
		}else{
			CurrentEvent=EventID;
			EndOfEvent=true;
			jentry--;
            
//             newtree->GetEntry(jentry);//should be needed as we already loaded everything we needed from last event
		}

		// The loop only continues beyond here if we have collected all rows of an entire event
		if(jentry==(nentries-1))EndOfEvent=true;
		if(!EndOfEvent) continue; 
		
        /////////////////////////
        ////  Now we have collected a complete event and can analyse the event
        ////////////////////////
		
        double Esumtmp=0;
        double Emax=0;
		if(Mult>1){
			for(unsigned int i=0;i<EventHolder.size();i++){
				double e=EventHolder[i];
				if(e>30){
					Esumtmp+=e;
					if(e>Emax){
						Emax=e;
						Seg=i;
					}
				}else if(Esumtmp>0){ // So only segments neigbouring will be summed;
					break;
				}
			}
			if(abs(Esumtmp-Eaddback)>5)Eaddback=0;
		}
        
		if(Eaddback>0.1) {

			E_AddbackSum->Fill(Eaddback);
			if(!(VETO1||VETO2)){
				E_AddbackVetoSum->Fill(Eaddback);
				AddbackVetoSumChan->Fill(Eaddback,Seg);
				
				int Ebin=Ebinax->FindBin(Eaddback);
				if(gdch->GetBinContent(Ebin,Seg+1)>0){
				
					EGridGated->Fill(Eaddback);
				
					if(std::abs(beta)>0){
						
						for(int m=0;m<4;m++){
							if(GdTH[m]->GetBinContent(Ebin,Seg+1)>0){
								if(!AngleExpt[Seg][m]){
									cout<<endl<<"ATTEMPTING TO USE NULL GRAPH "<<Seg<<" "<<m<<endl;
									return;
								}
								double et=AngleExpt[Seg][m]->Eval(Eaddback);
			// 					if(m==0)cout<<endl<<Seg<<" "<<Eaddback<<" "<<et;
								if(et>0){
									double ecor=((Eaddback+511-beta*cos(et)*sqrt(Eaddback*Eaddback+2*511*Eaddback))/sqrt(1-beta*beta))-511;
									E_Corr[m]->Fill(ecor);
									if(m==0){
										E_CorrGrid->Fill(ecor,Seg);
									}
								}
							}
						}
						
						if(dE){
							CoMExptGrid[Seg]->Fill(Eaddback,primaryE);
						}
						
					}
				}
			}    

		}
			
		std::fill(EventHolder.begin(), EventHolder.end(), 0);
		
		VETO1=false;
		VETO2=false;
		Eaddback=0;
		Theta=0;
		Mult=0;
		TMult=0;
	
	}
	
	
	// Efficiency fitting
	if(Nevent>0){
		TH1* h=EGridGated;
		if(std::abs(beta)>0) h=E_Corr[0];
		
		out.cd(HistFolder);
			FitHist=(TH1*)h->Clone("FitHist");
		gROOT->cd();
		
		h=FitHist;
		
		TAxis* ax=Eff->GetXaxis();
		TAxis* AX=h->GetXaxis();
		for(int b=1;b<=ax->GetNbins();b++){
			double e=ax->GetBinCenter(b);
			double de=ax->GetBinWidth(b)*0.5;
			
			double bg=h->Integral(AX->FindBin(e-de*0.75),AX->FindBin(e+de*0.75));
			double pc=h->Integral(AX->FindBin(e-de*0.25),AX->FindBin(e+de*0.25));
			bg-=pc;
			pc-=bg*0.5;
			
			Eff->SetBinContent(b,pc*100/Nevent);

			TF1* fit=UserQuickSingleGausAutoFit(h,e,e-de,e+de,1);
			h->GetListOfFunctions()->Add(fit);
			double ef=2.5066*fit->GetParameter(0)*fit->GetParameter(2)*100/Nevent;
			EffFit->SetBinContent(b,ef);
			SigFit->SetBinContent(b,fit->GetParameter(2));
			Metric->SetBinContent(b,10*ef/fit->GetParameter(2));
		}
	}
	
	
    out.Write();
    out.Close();	
    delete ntuplefile; // Delete the variable holding the input file, which also closes it
    
}

