/*

  contains all info related to a single channel

  v0.1 2015-01-19 AFan
  

 */


#ifndef ChannelData_hh
#define ChannelData_hh

#include "Rtypes.h" // has ClassDef macros
#include <vector>
#include "PulseData.hh"

using namespace std;

class ChannelData
{
public:
  ChannelData();
  void Clear();

  // metadata
  int daq_channel_num;
  int channel_id;
  float adc_gain;
  float adc_offset;
  float adc_range;

  // laser calibration info
  double spe_mean;

  bool saturated;
  
  // waveforms
  vector<double> raw_waveform;
  vector<double> baseline_subtracted_waveform;
  vector<double> zero_suppressed_waveform;
  vector<double> integral_waveform;
  
  // baseline finder
  double baseline_mean;
  double baseline_sigma;
  bool baseline_valid;

  // pulse finder
  int npulses;
  vector<PulseData*> pulses;

  // ROI
  vector<double> roi;

  double integrate(int start_samp, int end_samp) const;
  
};


#endif
