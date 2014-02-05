#include "PulseFinder.hh"
#include <algorithm> //min
#include <numeric>   //accumulate

PulseFinder::PulseFinder(CfgReader const& cfg):
  module_name("PulseFinder"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _pulse_start_threshold(cfg.getParam<double>(module_name, "pulse_start_threshold", 0))
{ }

int PulseFinder::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  vector<double> const& waveform = event->sum_waveform;

  // For now, do simple threshold pulse finding
  bool in_pulse = false;
  for (size_t i=0; i<waveform.size(); i++) {
    if (!in_pulse) {
      if (waveform[i] < _pulse_start_threshold) {
        in_pulse = true;
        event->pulse_start_times.push_back(event->SampleToTime(i));
      }
    }
    else { //in_pulse
      if (waveform[i] > _pulse_start_threshold) {
        in_pulse = false;
        event->pulse_end_times.push_back(event->SampleToTime(i));
      }
      if (i >= waveform.size()-1)
        event->pulse_end_times.push_back(waveform.size()-1);
    }
  }
  event->npulses = std::min(event->pulse_start_times.size(), event->pulse_end_times.size());
  EvaluatePulses(event);

  return 1;

}


int PulseFinder::EvaluatePulses(EventData* event)
{
  vector<double> const& waveform = event->sum_waveform;
  
  for (int i=0; i<event->npulses; i++) {
    int start_index = event->TimeToSample(event->pulse_start_times[i]);
    int end_index = event->TimeToSample(event->pulse_end_times[i]);
    int peak_index = std::min_element(waveform.begin() + start_index, waveform.begin() + end_index)-waveform.begin();
    event->pulse_peak_times.push_back(event->SampleToTime(peak_index));
    event->pulse_peak_amps.push_back(waveform[peak_index]);
    event->pulse_integrals.push_back(std::accumulate(waveform.begin()+start_index, waveform.begin()+end_index, 0.0));
    
  }
  return 1;
}
