void HPGe(unsigned int first_run, unsigned int last_run, unsigned int first_segment, unsigned int last_segment){

double sample_rate = 225.; // sampling rate      [MHz] for IFIN detectors 900.
double window      = 100.; // acquisition window [ms]
float  points;
short  y1, y2;
float  time_bin;

float j;
int   n;
int   local_flag = 1; // 0 to dissable progress indicator


// (1) Open the root file that contains the movie
for (unsigned int run = first_run; run <= last_run; run++){
	for (unsigned int seg = first_segment; seg <= last_segment; seg++){
		for (unsigned int idx = 0; idx<=19; ++idx){
		
			cout << endl;
			cout << "Run     : " << run << endl;
			cout << "Segment :      " << seg << endl;
			cout << "-----------------" << endl;
			 *f = new TFile(TString::Format("Signals_%d_%d.root",run, seg));
			if ( f ) cout << "File " << f->GetName() << " opened succesfully" << endl;
			else continue;

			TTree *t1 = (TTree*)f->FindObjectAny(TString::Format("raw_1_%d;1", idx)); //TTree *t2 = (TTree*)f->FindObjectAny("raw_2_1;1");
			t1->SetBranchAddress("signal_y",&y1);              //t2->SetBranchAddress("signal_y",&y2);
			int entries1 = t1->GetEntries();                   //int entries2 = t2->GetEntries();

			cout << "HPGe1 : Found " << entries1 << " entries " << endl;


// (2) Create the movie based on the sampling rate

			points   = window*sample_rate*1.e3; // Total number of points in movie, ms*MS = 1000
			time_bin = 1.e9/sample_rate; // in ns
			TH1F *h1 = new TH1F(TString::Format("h_HPGe_1_%d_%d_%d", run, seg, idx), TString::Format("HPGe1, run %d, segment %d, index %d", run, seg, idx), points, 0, window*1.e-3);


			if(local_flag > 0) {
				printf("         10        20        30        40        50        60        70        80        90        100 %%\n");
				printf(" ---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|    \n");
			}

			j = entries1/100.;
			n = 1;
			for (int i = 0; i<entries1; i++){
				if(local_flag > 0) {
					if(i == 0) {cout << " ";}
					if(i >= n*j) {cout << '|' << flush; n++;}// Progress indicator
				}
				t1->GetEntry(i);
				h1->SetBinContent(i+1, y1);
			}

			cout << endl;
			cout << endl;
			h1->SetLineColor(kBlack); //h2->SetLineColor(kRed);

// (3a) Save a  root file with all movies
			TFile fout(TString::Format("HPGe_%d_%d_%d.root", first_run, last_run, last_segment),"UPDATE");
			cout << "Saving File " << fout.GetName() << endl;
			h1->Write(); //h2->Write();
			fout.Close();
			cout << "Deleting signal file" << endl;

			f->Close();
		}// end of loop over indices (raw_1_idx;1)
	}// end of loop over segments
}// end of loop over runs

}//___HPGe()
