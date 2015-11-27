/*
  2015-11-25 AFan
 */

#ifndef CONVERTER_H
#define CONVERTER_H


#include "Module.hh"
#include "EventData.hh"
#include "LVDAQHeader.hh"


class Converter : public Module
{
public:
  Converter(const Setting & cfg);
  void Initialize();
  void Process(EventData* event, LVDAQHeader & daq_header);
  //void Finalize(TTree* master);


private:

  Float_t trigger_offset;

  Int_t run_id;
  Int_t subrun_id;
  Int_t event_id;
  Int_t nchans;
  Int_t nsamps;
  Int_t timestamp_sec;
  Int_t timestamp_usec;
  
  
};



#endif
