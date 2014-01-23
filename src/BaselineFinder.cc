#include "BaselineFinder.hh"
#include "ChannelData.hh"
#include <cmath>
#include <vector>


using namespace std;

/*
BaselineFinder::BaselineFinder():
  module_name("BaselineFinder")
{ }

BaselineFinder::BaselineFinder(std::string module_label):
  module_name(module_label)
{ }
*/

BaselineFinder::BaselineFinder(CfgReader const& cfg):
  module_name("BaselineFinder"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _start_time(cfg.getParam<double>(module_name, "start_time", -0.1)),
  _end_time(cfg.getParam<double>(module_name, "end_time", 0.0)),
  _threshold(cfg.getParam<double>(module_name, "threshold", 1.0))
{ }

void BaselineFinder::Initialize(CfgReader cfg)
{
  _enabled = cfg.getParam<bool>(module_name, "enabled", true);
  _start_time = cfg.getParam<double>(module_name, "start_time", -0.1);
  _end_time = cfg.getParam<double>(module_name, "end_time", 0.0);
  _threshold = cfg.getParam<double>(module_name, "threshold", 1.0);
}

int BaselineFinder::Process(EventData* event)
{
  if (!_enabled)
    return 0;


  // Loop over channels
  for (int idx = 0; idx<event->nchans; ++idx) {
    ChannelData & chData = event->channels[idx];
    
    vector<double> const& raw = chData.raw_waveform;
    double sum = 0;
    double var = 0;
    int start_samp = event->TimeToSample(_start_time);
    int end_samp = event->TimeToSample(_end_time);
    for (int i = start_samp; i<end_samp; ++i) {
      sum += raw[i];
      var += raw[i]*raw[i];
    }

    chData.baseline_mean = sum / (end_samp - start_samp);
    chData.baseline_sigma = sqrt(var / (end_samp - start_samp));

    if (chData.baseline_sigma < _threshold) {
      chData.baseline_valid = true;

      // compute the baseline-subtracted and inverted waveform
      vector<double> bs_wfm;
      bs_wfm.reserve(raw.size());
      for (size_t i=0; i<raw.size(); ++i)
        bs_wfm.push_back( -(raw[i] - chData.baseline_mean) );
      chData.baseline_subtracted_waveform = bs_wfm;
    }

    
  } // end loop over channels
  
  return 1;
}
