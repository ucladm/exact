/*

  Defines the BaselineFinder class module.
  Finds the baseline on some user defined region, computes
  a baseline-subtracted and inverted waveform.
  
  v0.1 AFan 2013-08-17
  v0.3 AFan 2015-11-26 -- revamp with new framework

 */

#ifndef BaselineFinder_hh
#define BaselineFinder_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>
#include <vector>

class BaselineFinder : public Module
{
public:
  BaselineFinder(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);

  void fixed_baseline(EventData* event);
  void moving_baseline(EventData* event);
  
private:
  std::string mode;

  // fixed baseline parameters
  float start_time;
  float end_time;
  float threshold;

  // moving baseline parameters
  int pre_samps;
  int post_samps;
  float max_sigma;
  float max_amplitude;
  float baseline_fixed_window;

  // variables to be saved
  Float_t* baseline_mean;
  Float_t* baseline_sigma;

  void interpolate_baseline(std::vector<double> & baseline, int start, int end);

  
};

#endif
