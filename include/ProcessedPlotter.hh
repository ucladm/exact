/*
  Defines the ProcessedPlotter class to plot rwa waveforms
  and some processed results for each channel

  v0.1 AFan 2013-08-17

  borrowed mostly from genroot


 */

#ifndef ProcessedPlotter_hh
#define ProcessedPlotter_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include "RootGraphix.hh"
#include <string>
#include <iostream>

// Forward declarations
class TCanvas;
class TLegend;

class ProcessedPlotter
{
public:
  ProcessedPlotter();
  void Initialize(CfgReader cfg, TCanvas* canvas, RootGraphix* graphix);
  int Process(EventData* event);
  void Finalize();
  
  const TCanvas* GetCanvas() { return _canvas; }

  int chans_per_pad;

private:
  std::string module_name;
  bool enabled;

  
  TCanvas* _canvas;
  std::vector<TLegend*> _legends;

  RootGraphix* _graphix;

};


#endif
