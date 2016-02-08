/*

  Perform zero-suppression on the waveforms.
  Use simple threshold value to determine if sample is
  to be zero-suppressed.

  v0.1 AFan 2014-02-03
  v0.2 AFan 2015-11-25

 */

#ifndef ZeroSuppressor_hh
#define ZeroSuppressor_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>


class ZeroSuppressor : public Module
{
public:
  ZeroSuppressor(const Setting & cfg);
  void Initialize(); 
  void Process(EventData* event);
  void Finalize(TTree* master);

private:

  // main threshold to decide if a portion of the waveform should
  // be zero-suppressed
  float threshold;

  // once a portion of the waveform is determined to be zero-suppressed,
  // suppress those samples up to the first sample and past the last sample
  // that are beyond this threshold
  float edge_threshold;

  int padding;

};



#endif
