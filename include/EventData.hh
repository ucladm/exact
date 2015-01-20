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

#define BOT_CHANNEL_ID 0

class TMultiGraph;

class EventData
{

public:
  
  EventData();
  
  // event-level metadata
  int run_id;
  int event_id;
  int nchans;
  int nsamps;
  double us_per_samp;
  int trigger_index;
  int trigger_index_offset; //for top-channels only
  int adc_bits; //number of ADC bits

  // channel-level metadata
  vector<int> daq_channel_nums;
  vector<int> channel_ids; // global channel ID used by all modules
  vector<double> adc_gains; // volts per ADC-count
  vector<double> adc_offsets;
  vector<double> adc_ranges;

  // laser calibration info
  vector<double> spe_means;


  // baseline finder
  vector<double> baseline_means;
  vector<double> baseline_sigmas;
  vector<bool> baseline_validities;

  // sum channels
  vector<double> sum_waveform;
  vector<double> sum_integral;

  // pulse finder
  int npulses;
  vector<bool> saturated;
  vector<double> pulse_start_times;
  vector<double> pulse_end_times;
  vector<double> pulse_peak_times;
  vector<double> pulse_peak_amps;
  vector<double> pulse_integrals;
  //vector<double> pulse_fixed_int1;
  //vector<double> pulse_fixed_int2;

  //vector< vector<double> > ch_pulse_peak_times;
  //vector< vector<double> > ch_pulse_peak_amps;
  //vector< vector<double> > ch_pulse_integrals;
  //vector< vector<double> > ch_pulse_fixed_int1;
  //vector< vector<double> > ch_pulse_fixed_int2;

    


  // channel waveforms
  vector< vector<double> > raw_waveforms;
  vector< vector<double> > baseline_subtracted_waveforms; // unit: ADC counts
  vector< vector<double> > integrals;
  vector< vector<double> > zero_suppressed_waveforms;
    
    
    
  double SampleToTime(int samp) const;
  int TimeToSample(double time, bool checkrange=true) const;

  TMultiGraph* GetTMultiGraph(int ch);
  TMultiGraph* GetTMultiGraph_sumch();

private:

  
  ClassDef(EventData, 1)
};


#endif

