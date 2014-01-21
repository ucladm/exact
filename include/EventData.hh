/*

  define EventData class.
  contains all processed information for each event
  

  v0.1 AFan 2013-05-01
  
 */

#ifndef EventData_hh
#define EventData_hh

#include "Rtypes.h" // has the ClassDef macros

#include "ChannelData.hh"
#include <vector>


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
    channels.clear();
  }

  // all the data for a single event  
  int run_id;
  int event_id;
  int nchans;
  int nsamps;
  double us_per_samp;
  int trigger_index;
  std::vector<ChannelData> channels;


  double SampleToTime(int samp) const;
  int TimeToSample(double time, bool checkrange=true) const;


  
  ClassDef(EventData, 1)
};


#endif

