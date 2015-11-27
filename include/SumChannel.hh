/*

  Create the sum channel.

  v0.1 AFan 2014-02-03
  v0.2 AFan 2015-11-25

  NOTE: currently just blind sum; will need to:
  - account for channel-to-channel SPE means
  x account for channel-to-channel timing alignment
    - accounted for in Converter

 */


#ifndef SumChannel_hh
#define SumChannel_hh

#include "Module.hh"
#include "EventData.hh"

#include <string>

class SumChannel : public Module
{
public:
  SumChannel(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);

private:

};

#endif
