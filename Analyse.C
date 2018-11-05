#include "functions.h"

void Analyze(unsigned int first_run, unsigned int last_run, unsigned int first_segment, unsigned int last_segment, unsigned int detn){

	const int T_MAX   = 7;
	const int T_MIN   = 0;
	const int N_BPDEC = 1000;
	const int ndec    = T_MAX - T_MIN;
	int		  nbins   = (int) ndec*N_BPDEC;
	double	  step    = (double) ndec / nbins;
	double	  tbins[nbins+1];

	for(int i=0; i <= nbins; i++) {
		tbins[i] = (float) pow(10., step * (int) i + T_MIN);
	}

// (1) Open the root file
	TFile *fin = new TFile(TString::Format("HPGe_%d_%d_%d.root", first_run, last_run, last_segment));
	if ( fin->IsOpen() ) cout << "File " << fin->GetName() << " opened successfully" << endl;

// (2) Get the Signals
	for (unsigned int run = first_run; run<=last_run; run++){
		for (unsigned int segm = first_segment; segm<=last_segment; segm++){//for (unsigned int segm = 2; segm<=2; segm++){
			for (unsigned int idx=0; idx<=19; ++idx){
		//for (unsigned int segm = 79; segm<=79; segm++){
				std::vector<float> tof;
				TH1F *htof    = new TH1F(TString::Format("htof_%d", segm), "tof spectrum", 1e6, 0, 0.01);
				TH1F *htofLog = new TH1F(TString::Format("htofLog_%d", segm), "tof spectrum", nbins, tbins);
				TH1F *hamp = new TH1F(TString::Format("hamp_%d_%d_%d_%d", detn, run, segm, idx), "amplitude spectrum", 16384, 0, 16383);

				cout << "-------" << endl;
				cout << "Starting segment : " << segm << endl;
			
				TH1D *hSignal     = (TH1D*)fin->Get(TString::Format("h_HPGe_%d_%d_%d_%d", detn, run, segm, idx));
				if(!hSignal){cout << "***Segment not found. Moving on." << endl;	continue;}
				TH1D *hDerivative = (TH1D*)hSignal->Clone("hDerivative");
				TH1D *hRMS        = (TH1D*)hSignal->Clone("hRMS");

				int points = hSignal->GetNbinsX();
				cout << "Total Number of points : " << points << endl;
				//double x[points], y[points], derivative[points]; // SIZE limited by COMPILER to the size of the stack frame
				double* x          = new double[points];           // SIZE limited only by OS/Hardware
				double* y          = new double[points];
				double* derivative = new double[points];
				double* mean       = new double[points];
				double* rms        = new double[points];
				std::vector<double> left;
				std::vector<double> right;
				std::vector<int> amplitude;
				std::vector<float> tof_tmp;
				double RMSthres = 0.;

				cout << "Filling vectors" << endl;
				for (int i = 0; i < points; i++){
					x[i] = hSignal->GetBinLowEdge(i+1);
					y[i] = hSignal->GetBinContent(i+1);
				}
				cout << "Calculating Derivative" << endl; 
				GetDerivative(points, x, y, 100, derivative);
				cout << "Calculating RMS"        << endl; 
				FindRMS(points, x, derivative, 0.9*points, mean, rms);
				if ( (*rms)==0. ) {cout << "***Something's wrong with the acquisition. DEAD Movie! Moving on" << endl; continue;}
				else if ( (*rms)<1000. ){RMSthres = -3.5*(*rms);}
				else if ( (*rms)>=1000. ) {RMSthres =  -0.55*(*rms);}
				cout << "Threshold is : " << RMSthres << endl;
				cout << "Locating Pulses" << endl;
				LocatePulseEdges(points, x, derivative, RMSthres, left, right);
				//if (left.size()>200){cout << "***Something's wrong with the derivative. Discaritng this movie. Moving on." << endl; continue;}
				cout << "Calculating amplitudes" << endl;
				FindAmplitudes (points, 100, x, y, left, right, amplitude);
				FindTime(points, left, right, x, derivative, tof, amplitude, 100, 5.e-7, (-1.)*RMSthres);

				for (int i=0; i<amplitude.size(); i++){
					hamp->Fill(amplitude[i]);
				}
//			for (int i=0; i<left.size(); i++){
//				if (right[i]<=0.01){
//					cout << "Left side : " << left[i] << ", Right side : " << right[i] << ", Amplitude : " << amplitude[i] << endl;
//				}
//			}
				delete[] x;
				delete[] y;
				delete[] mean;
				delete[] rms;
				cout << "Filling derivative and RMS histograms" << endl;
				for (int i = 0; i<=hDerivative->GetNbinsX(); i++){
					hDerivative->SetBinContent(i+1, derivative[i]);
					hRMS->SetBinContent(i+1, RMSthres );
				}

				delete[] derivative;

//			hSignal->SetLineColor(kBlack);
//			hDerivative->SetLineColor(kRed);
//			TFile fout(TString::Format("derivative_TEST_Au.root"), "UPDATE");
//			cout << "Saving File " << fout.GetName() << endl;
//			hSignal->SetName(TString::Format("hSignal_%d_%d_%d", detn, run, segm));
//	   		hSignal->Write();
//			hDerivative->SetName(TString::Format("hDerivative_%d_%d_%d", detn, run, segm));
//			hDerivative->Write();
//			hRMS->SetName(TString::Format("hRMS_%d_%d_%d", detn, run, segm));
//			hRMS->Write();
//			fout.Close();
//			hSignal->Delete(); hDerivative->Delete(); hRMS->Delete(); //hamp->Delete();

				TFile fout(TString::Format("amp_%d_%d_%d.root", first_run, last_run, last_segment), "UPDATE");
				cout << "Saving File " << fout.GetName() << endl;
				hamp->Write();
				fout.Close();
//			hamp->Delete();

//			for (int i=0; i<tof.size(); i++){
//				htof->Fill(tof[i]-tof[0]);
//				htofLog->Fill(tof[i]-tof[0]);
////				if (tof[i] <= 2e-4){
//					cout << "Time of flight : " << tof[i]-tof[0] << endl;
////				}
//			}

//			TFile ftof(TString::Format("tof_%d_%d_%d_%d_TEST.root", detn, first_run, last_run, last_segment), "UPDATE");
//			cout << "Saving File " << ftof.GetName() << endl;
//			htof->Write(); htofLog->Write();
//			ftof.Close();
//			htof->Delete(); htofLog->Delete();

			}// end of loop over indices
		}// end of loop over segments
	}// end of loop over runs
//	TFile fout(TString::Format("amp_%d_%d_%d.root", first_run, last_run, last_segment), "UPDATE");
//	cout << "Saving File " << fout.GetName() << endl;
//	hamp->Write();
//	fout.Close();
//	for (int i=0; i<tof.size(); i++){
//		htof->Fill(tof[i]);
//	}
//	TFile ftof(TString::Format("tof_%d_%d_%d.root", first_run, last_run, last_segment), "UPDATE");
//	cout << "Saving File " << ftof.GetName() << endl;
//	htof->Write();
//	ftof.Close();

}//___Analyze()
