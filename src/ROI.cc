#include "ROI.hh"
#include <algorithm>
#include <numeric> //accumulate
#include "ChannelData.hh"



ROI::ROI(CfgReader const& cfg):
  module_name("ROI"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _roi_start(cfg.getParam<double>(module_name, "roi_start", -0.2)),
  _roi_end(cfg.getParam<double>(module_name, "roi_end", -0.2))
{}


int ROI::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  int roi_start_index = event->TimeToSample(_roi_start);
  int roi_end_index = event->TimeToSample(_roi_end);
  
  // loop over channels
  for (int ch=0; ch<event->nchans; ++ch) {
    double roi = -std::accumulate(event->GetChannel(ch)->baseline_subtracted_waveform.begin()+roi_start_index,
                                  event->GetChannel(ch)->baseline_subtracted_waveform.begin()+roi_end_index,0.0);
    //double roi = std::accumulate(event->GetChannel(ch)->raw_waveform.begin()+roi_start_index,
    //                             event->GetChannel(ch)->raw_waveform.begin()+roi_end_index, 0.0);
    event->GetChannel(ch)->roi = roi;
    event->roi += roi;
  }

  
  return 1;
  
}
