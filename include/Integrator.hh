/*

  Defines the Integral class module.
  Integrates a waveform, scaling the integral by spe_mean

  v0.1 AFan 2013-08-17
  v0.2 AFan 2015-11-25

 */

#ifndef Integrator_hh
#define Integrator_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>
#include <vector>

class Integrator : public Module
{
public:
  Integrator(const Setting & cfg);
  void Initialize(); 
  void Process(EventData* event);
  void Finalize(TTree* master);

private:
  void integrate(std::vector<double> const& wfm, std::vector<double> & result);
};

#endif
