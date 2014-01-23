/*

  Defines the ChannelData object.
  Contains information specific to each channel.

  v0.1 AFan 2013-05-17

 */

#ifndef ChannelData_hh
#define ChannelData_hh

#include "Rtypes.h" //has the ClassDef macro

#include <vector>


using namespace std;

class TGraph;

class ChannelData
{
public:
  ChannelData():
    event_id(-1),
    channel_id(-1),
    spe_mean(-1),
    raw_waveform(),
    baseline_mean(0),
    baseline_sigma(-1),
    baseline_valid(false),
    baseline_subtracted_waveform(),
    integral()
  {}

  TGraph* GetTGraph() const;
  void Draw();
  
  // all channel-level information
  int event_id; 
  int channel_id; // global channel ID
  double spe_mean; // SPE mean for this channel
  vector<double> raw_waveform; //! the raw waveform for this channel
  double us_per_samp;
  int trigger_index;
  double baseline_mean;
  double baseline_sigma;
  bool baseline_valid;
  vector<double> baseline_subtracted_waveform; //! 
  vector<double> integral; //! 
  

  ClassDef(ChannelData, 1)

};



#endif
