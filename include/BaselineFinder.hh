/*

  Defines the BaselineFinder class module.
  Finds the baseline on some user defined region, computes
  a baseline-subtracted and inverted waveform.
  
  v0.1 AFan 2013-08-17

 */

#ifndef BaselineFinder_hh
#define BaselineFinder_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <string>

class BaselineFinder
{
public:
  //BaselineFinder();
  //BaselineFinder(std::string module_label);
  BaselineFinder(CfgReader const& cfg);
  void Initialize(CfgReader cfg);
  int Process(EventData* event);

  std::string module_name;



private:
  bool   _enabled;  
  double _start_time;
  double _end_time;
  double _threshold;
    

};


#endif
