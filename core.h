void GetDerivative(int points, double* signal_x, double* signal_y, int stepsize, double* derivative){
  double left,right;
  
  if (stepsize<=0) stepsize=1;
  
  for (int i=0; i<points; i++) {
    //Recursive finding of following terms
    if (i==0) {
      //First point
      left=0;
      right=0;
    } else if (i<=stepsize && 2*i<=points-1) {
      //Beginning of the movie
      left=left+signal_y[i-1];
      right=right-signal_y[i]+signal_y[2*i-1]+signal_y[2*i];
    } else if (i>stepsize && i+stepsize<=points-1) {
      //Middle of the movie
      left=left+signal_y[i-1]-signal_y[i-1-stepsize];
      right=right-signal_y[i]+signal_y[i+stepsize];
    } else if (i+stepsize>points-1 && 2*i>points-1) {
      //End of the movie
      left=left+signal_y[i-1]-signal_y[2*i-points-1]-signal_y[2*i-points];
      right=right-signal_y[i];
    }
    
    derivative[i]=right-left;
  }
}//___GetDerivative()

void FindRMS(int points, double* signal_x, double* signal_y, int presamples, double* mean, double* rms){ 
  	
  //Safety condition
  if (presamples>points) {
    printf("\nERROR: Too many presamples!\n\n");
    return;
  }
	
  //Main procedure
  (*rms)  = 0.;	
  (*mean) = 0.;

  for (int i=0; i<presamples; i++) {
    (*mean)+=signal_y[i];
    (*rms)+=pow(signal_y[i],2);
  }

  (*mean)/=presamples;
  (*rms)=sqrt((*rms)/presamples-pow((*mean),2));
  cout << "RMS was found to be : " << (*rms) << endl;

}//___FindRMS()

double FindRMS_2(int points, double* signal_x, double* signal_y, int presamples, double rms){
	if (presamples>points) {
		printf("\nERROR: Too many presamples!\n\n");
		return 0;
	}

	//for (int i = 0; i< points; i++){rms[i]=0;}
	rms = 0.;
	for (int i = 0; i<presamples; i++){
		rms +=pow(signal_y[i],2);
	}
	rms = (double) sqrt(rms/presamples);
	cout << "RMS was found to be : " << rms << endl;
	return rms;

}//___FindRMS_2()

std::vector<double>& LocatePulseEdges(int points, double* signal_x, double* signal_y, double threshold, vector<double>& left_edge, vector<double>& right_edge){
	int pulses = 0;
	bool inside_pulse = false;
	for (int i=0; i<points; i++){
		if( (signal_y[i] - threshold) < 0. ){// left side of the pulse
			if (!inside_pulse){
				left_edge.push_back(signal_x[i]);
				pulses++;
				inside_pulse = true;
			}
		}
		if( (signal_y[i] - threshold) > 0. ){// right side of the pulse
			if (inside_pulse){
				right_edge.push_back(signal_x[i]);
				inside_pulse = false;
			}
		}
	}//end of loop over points
	cout << "Total Pulses Located : " << pulses << endl;

}//___LocatePulseEdges()

std::vector<int>& FindAmplitudes(int points, int stepsize, double* signal_x, double* signal_y, vector<double>& left_edge, vector<double>& right_edge, vector<int>& amplitude){
	double mean_left  = 0.;
	double mean_right = 0.;
	int    amp        = 0;
	for (int i=0; i<left_edge.size(); i++){
		for (int j=0; j<points; j++){		
			if(signal_x[j] == left_edge[i]){
				for (int k=(int)(j-stepsize/2); k<(int)(j+stepsize/2); k++){
					mean_left+=signal_y[k];
				}
				mean_left/=stepsize;
				//break;
			}
			if (signal_x[j] == right_edge[i]){
				for (int k=(int)(j-stepsize/2); k<(int)(j+stepsize/2); k++){
					mean_right+=signal_y[k];
				}
				mean_right/=stepsize;
				//break;
			}
		}//end of loop over points 
		amp = (int) (abs(mean_left-mean_right)+0.5);
		amplitude.push_back(amp);
	}//end of loop over located pulses
	cout << endl;
}//___FindAMplitudes()

std::vector<float>& FindTime(int points, vector<double>& left_edge, vector<double>& right_edge, double* signal_x, double* signal_y, vector<float>& tof, vector<int>& amplitude, int stepsize, double t_window, double thres){
	int   pulses = 0;
	int   avg_points;
	float mean;
	for (int i=0; i<left_edge.size(); i++){
		if (amplitude[i] >= 3){//condition for false recognition
			mean       = 0.;
			avg_points = 0;
			for (int j=0; j<points; j++){
				if (signal_x[j] >= left_edge[i] - t_window && signal_x[j] <= left_edge[i]){
					mean+=signal_y[j];
					avg_points++;
				}
			}
			mean/=avg_points;
			if(mean<=thres){
				tof.push_back(left_edge[i]+((right_edge[i]-left_edge[i])/2));
				pulses++;
			}
		}
	}
	cout << "Total (real) pulses found : " << pulses << endl;
}//___FindTime()
