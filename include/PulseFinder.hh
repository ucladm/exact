/*

  Defines the PulseFinder class module.
  Searches through the sum channel waveform to find pulses.

  v0.1 AFan 2014-02-03
  v0.2 AFan 2015-11-25

 */


#ifndef PulseFinder_hh
#define PulseFinder_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>
#include <vector>

#define MAXNPULSES 100

class PulseFinder : public Module
{
public:
  PulseFinder(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);

private:
  std::string _mode;

  int _down_sample_factor;
  double _pulse_start_threshold;
  double _pulse_start_amp;
  double _pulse_end_threshold;

  // variables to save to output
  Int_t npulses;
  Double_t pulse_start[MAXNPULSES];
  Double_t pulse_end[MAXNPULSES];
  Double_t pulse_integral[MAXNPULSES];
  Double_t pulse_prompt[MAXNPULSES];
  Bool_t  pulse_saturated[MAXNPULSES];
  
  void EvaluatePulses(EventData* event);

  int ThresholdSearch(EventData* event);
  int IntegralSearch(EventData* event);
};


#endif
