/*

  Perform zero-suppression on the waveforms.
  Use simple threshold value to determine if sample is
  to be zero-suppressed.

  v0.1 AFan 2014-02-03

 */

#ifndef ZeroSuppressor_hh
#define ZeroSuppressor_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <string>


class ZeroSuppressor
{
public:
  ZeroSuppressor(CfgReader const& cfg);
  // void Initialize(); //Add an Initialize function only if necessary
  int Process(EventData* event);

  std::string module_name;

private:
  bool _enabled;

  double _threshold;

};



#endif
