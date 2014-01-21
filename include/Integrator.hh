/*

  Defines the Integral class module.
  Integrates a waveform, scaling the integral by spe_mean

  v0.1 AFan 2013-08-17

 */

#ifndef Integrator_hh
#define Integrator_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <string>

class Integrator
{
public:
  Integrator();
  void Initialize(CfgReader cfg);
  int Process(EventData* event);

  std::string module_name;
  bool enabled;

};

#endif
