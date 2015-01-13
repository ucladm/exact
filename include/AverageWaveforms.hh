/*

  Defines the AverageWaveforms class module.
  Combines waveforms in various ways to produce averges.

  v0.1 AFan 2014-02-12

 */


#ifndef AverageWaveforms_hh
#define AverageWaveforms_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <iostream>
#include <string>
#include <vector>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TFile.h"

class AverageWaveforms
{
public:
  AverageWaveforms(CfgReader const& cfg);
  void Initialize();
  int Process(EventData* event);
  void Finalize(TFile* f);

  std::string module_name;

private:
  bool _enabled;

  //std::string _output_filename;

  std::vector<TGraphErrors*> _plots;
  std::vector<int> _nevents;
  
  
};


#endif
