/*

  define EventData class.
  contains all processed information for each event
  

  v0.1 AFan 2013-05-01
  
 */

#ifndef EventData_hh
#define EventData_hh

#include "Rtypes.h" // has the ClassDef macros
#include <vector>
#include <map>

using namespace std;

class TMultiGraph;

class EventData
{

public:
  EventData() {Clear();}
  
  void Clear() {
    run_id = -1;
    event_id = -1;
    nchans = -1;
    nsamps = -1;
    us_per_samp = -1.0;
    trigger_index = -1;
    channel_id.clear();
    channel_index.clear();
    spe_mean.clear();
    raw_waveform.clear();
    baseline_mean.clear();
    baseline_sigma.clear();
    baseline_valid.clear();
    baseline_subtracted_waveform.clear();
    integral.clear();
    
  }

  // all the data for a single event  
  int run_id;
  int event_id;
  int nchans;
  int nsamps;
  double us_per_samp;
  int trigger_index;


  vector<int> channel_id;
  map<int, int> channel_index;
  vector<double> spe_mean;
  vector< vector<double> > raw_waveform;
  vector<double> baseline_mean;
  vector<double> baseline_sigma;
  vector<bool> baseline_valid;
  vector< vector<double> > baseline_subtracted_waveform;
  vector< vector<double> > integral;
  


  double SampleToTime(int samp) const;
  int TimeToSample(double time, bool checkrange=true) const;

  TMultiGraph* GetTMultiGraph(int ch);

private:

  
  ClassDef(EventData, 1)
};


#endif

