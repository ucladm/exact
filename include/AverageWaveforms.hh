/*

  Defines the AverageWaveforms class module.
  Combines waveforms in various ways to produce averges.

  v0.1 AFan 2014-02-12
  v0.2 AFan 2015-12-01

 */


#ifndef AverageWaveforms_hh
#define AverageWaveforms_hh

#include "Module.hh"
#include "EventData.hh"
#include <iostream>
#include <string>
#include <vector>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TFile.h"

class AverageWaveforms : public Module
{
public:
  AverageWaveforms(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);

private:

  std::vector<TH1F*> _plots;
  //std::vector<int> _nevents;
  TH1I* _nevents;
  
  
};


#endif
