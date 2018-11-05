void AddHistos(unsigned int first_run, unsigned int last_run, unsigned int last_segment, unsigned int detn){

	TH1F *hamp = new TH1F("hamp", "amplitude spectrum", 16384, 0, 16383);

	TFile *f1 = TFile::Open(TString::Format("amp_%d_%d_%d.root", first_run, last_run, last_segment));
	for (unsigned int run=first_run; run<=last_run; ++run){
		for (unsigned int seg=1; seg<=last_segment; ++seg){
			for (unsigned int idx=0; idx<=19; ++idx){
				TH1F *h1 = (TH1F*)f1->Get(TString::Format("hamp_%d_%d_%d_%d", detn, run, seg, idx));
				for (int i=0; i<hamp->GetNbinsX(); i++){
					hamp->AddBinContent(i+1, h1->GetBinContent(i+1) );
				}
			}// end of loops over indices
		}// end of loops over segments
	}// end of loop over runs

// (2) Save the file
	TFile fout(TString::Format("amp_spectrum_%d_%d_%d_%d.root", detn, first_run, last_run, last_segment), "RECREATE");
	cout << "Saving File " << fout.GetName() << endl;
	hamp->Write();
	fout.Close();

/*	TFile *f2 = TFile::Open("amp_102632_102632_21-32.root");
	TH1D *h2 = (TH1D*)f2->Get("hamp");
	for (int i=0; i<hamp->GetNbinsX(); i++){
		hamp->AddBinContent(i+1, h2->GetBinContent(i+1) );
	}

	TFile *f3 = TFile::Open("amp_102632_102632_33-48.root");
	TH1D *h3 = (TH1D*)f3->Get("hamp");
	for (int i=0; i<hamp->GetNbinsX(); i++){
		hamp->AddBinContent(i+1, h3->GetBinContent(i+1) );
	}

	TFile fout("amplitudes.root", "RECREATE");
	cout << "Saving File " << fout.GetName() << endl;
	hamp->Write();
	fout.Close();
*/

/*
	const int T_MAX   = 7;
	const int T_MIN   = 0;
	const int N_BPDEC = 1000;
	const int ndec    = T_MAX - T_MIN;
	int		  nbins   = (Int_t) ndec*N_BPDEC;
	double	  step    = (double) ndec / nbins;
	double	  tbins[nbins+1];

	for(int i=0; i <= nbins; i++) {
		tbins[i] = (float) pow(10., step * (int) i + T_MIN);
	}

	TH1D *htof    = new TH1D("htof", "tof spectrum", 1e6, 0, 0.01);
	TH1F *htofLog = new TH1F("htofLog", "tof spectrum", nbins, tbins);
	TFile *f = new TFile("tof_2_102642_102642_2000_TEST.root");
	for (int i=1000; i<=2000; i++){
		TH1D *h    = (TH1D*)f->Get(TString::Format("htof_%d", i));
		TH1D *hLog = (TH1D*)f->Get(TString::Format("htofLog_%d", i));
		if(!h) continue; if(!hLog) continue;
		for (int j=0; j<htof->GetNbinsX(); j++){
			htof->AddBinContent(j+1, h->GetBinContent(j+1));
		}
		for (int j=0; j<htofLog->GetNbinsX(); j++){
			htofLog->AddBinContent(j+1, hLog->GetBinContent(j+1));
		}
	}

	TFile fout("tof_102626_2_1000_TEST.root", "RECREATE");
	cout << "Saving File " << fout.GetName() << endl;
	htof->Write(); htofLog->Write();
	fout.Close();
*/

}//___AddHistos()

void AddHTof(){

	TH1D *htof    = new TH1D("htof", "tof spectrum", 1e6, 0, 0.01);
cout << "1"  << endl;
	for (int i = 1000; i<=2000; i+=1000){
		TFile *f = new TFile(TString::Format("tof_102642_2_%d_TEST.root", i));
		TH1D *h    = (TH1D*)f->Get("htof");
		for (int j=0; j<htof->GetNbinsX(); j++){
			htof->AddBinContent(j+1, h->GetBinContent(j+1));
		}
		f->Close();
	}
	TFile fout("tof_102626_2.root", "RECREATE");
	cout << "Saving File " << fout.GetName() << endl;
	htof->Write(); //htofLog->Write();
	fout.Close();

}//___AddHTof


void Offset(){

	TH1D *hMovie    = new TH1D("hMovie", "", 54000000, 0, 0.06);
	TFile *f = new TFile("HPGe_102642_102642_1000.root");
	for (int i=2; i<=7; i++){
		TH1D *h = (TH1D*)f->Get(TString::Format("h_HPGe_2_102642_%d", i));
		cout << "Total number of points : " << h->GetNbinsX() << endl;
		for(int j=0; j<hMovie->GetNbinsX(); j++){
			hMovie->AddBinContent(j+1, h->GetBinContent((j+1)-(i-2)*9000000));
		}
		h->Delete();
	}
	
	TFile fout("Movie_FULL.root", "RECREATE");
	cout << "Saving File " << fout.GetName() << endl;
	hMovie->Write();
	fout.Close();

}//___Offset()
