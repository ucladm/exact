/*

  Defines the PulseFinder class module.
  Searches through the sum channel waveform to find pulses.

  v0.1 AFan 2014-02-03

 */


#ifndef PulseFinder_hh
#define PulseFinder_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <string>

class PulseFinder
{
public:
  PulseFinder(CfgReader const& cfg);
  //void Initialize(); //Add an Initialize function only if necessary.
  int Process(EventData* event);

  std::string module_name;

private:
  bool _enabled;

  double _pulse_start_threshold;

  int EvaluatePulses(EventData* event);
};


#endif
