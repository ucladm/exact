/*
  Defines the ProcessedPlotter class to plot rwa waveforms
  and some processed results for each channel

  v0.1 AFan 2013-08-17

  borrowed mostly from genroot


 */

#ifndef ProcessedPlotter_hh
#define ProcessedPlotter_hh

#include "Module.hh"
#include "EventData.hh"
//#include "RootGraphix.hh"
#include <string>
#include <iostream>

// Forward declarations
class TCanvas;
class TLegend;

class ProcessedPlotter : public Module
{
public:
  ProcessedPlotter(const Setting & cfg);
  //void Initialize(TCanvas* canvas, RootGraphix* graphix);
  //void Initialize(TCanvas* canvas);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);
  
  //const TCanvas* GetCanvas() { return _canvas; }

private:

  int _chans_per_pad;
  std::vector<TLegend*> _legends;



};


#endif
