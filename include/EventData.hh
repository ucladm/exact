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
#include <stdint.h>
#include "ChannelData.hh"
#include "PulseData.hh"


using namespace std;

#define BOT_CHANNEL_ID 0

class TMultiGraph;

class EventData
{

public:
  
  EventData();
  void Clear();
  
  // event-level metadata
  int run_id;
  int event_id;
  int timestamp_sec;
  int timestamp_usec;
  int nchans;
  int nsamps;
  double us_per_samp;
  int trigger_index;
  int trigger_index_offset; //for top-channels only
  int adc_bits; //number of ADC bits

  bool saturated;
  
  vector<ChannelData> channels;
  ChannelData sumchannel;
  int npulses;
  vector<PulseData> pulses;


  double roi;
  
  double SampleToTime(int samp) const;
  int TimeToSample(double time, bool checkrange=true) const;

  ChannelData* GetChannel(int channel_id);
  
  TMultiGraph* GetTMultiGraph(int ch);
  TMultiGraph* GetTMultiGraph_sumch();

private:

  
  ClassDef(EventData, 1)
};


#endif

