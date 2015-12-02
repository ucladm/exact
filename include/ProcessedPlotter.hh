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
#include <string>
#include <iostream>
#include <vector>

// Forward declarations
class TCanvas;
class TLegend;

class ProcessedPlotter : public Module
{
public:
  ProcessedPlotter(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  //void Finalize(TTree* master);
  
  //const TCanvas* GetCanvas() { return _canvas; }

private:

  std::vector<TLegend*> _legends;
  bool _draw_sum;


  void PlotChannel(EventData* event, int chID);
  void PlotSumChannel(EventData* event);
  std::vector<double> DrawIntegral(ChannelData* channel);
  void DrawPulses(EventData* event, double base);


};


#endif
