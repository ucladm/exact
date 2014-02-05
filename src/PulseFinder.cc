#include "PulseFinder.hh"
#include <algorithm> //min
#include <numeric>   //accumulate
#include <cmath>

PulseFinder::PulseFinder(CfgReader const& cfg):
  module_name("PulseFinder"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _mode(cfg.getParam<std::string>(module_name, "mode", "THRESHOLD")),
  _down_sample_factor(cfg.getParam<int>(module_name, "down_sample_factor", 10)),
  _pulse_start_threshold(cfg.getParam<double>(module_name, "pulse_start_threshold", 0)),
  _pulse_start_amp(cfg.getParam<double>(module_name, "pulse_start_amp", 0)),
  _pulse_end_threshold(cfg.getParam<double>(module_name, "pulse_end_threshold", 0))
{ }

int PulseFinder::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  if (_mode=="THRESHOLD")
    ThresholdSearch(event);
  else if (_mode=="INTEGRAL")
    IntegralSearch(event);
  else {
    std::cout << "PulseFinder mode not recognized. Using THRESHOLD."<<std::endl;
    ThresholdSearch(event);
  }
  
  EvaluatePulses(event);

  return 1;

}


void PulseFinder::EvaluatePulses(EventData* event)
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
}

void PulseFinder::ThresholdSearch(EventData* event)
{
  vector<double> const& waveform = event->sum_waveform;

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
    }
  }
  event->npulses = std::min(event->pulse_start_times.size(), event->pulse_end_times.size());
}

void PulseFinder::IntegralSearch(EventData* event)
{
  vector<double> const& waveform = event->sum_waveform;
  vector<double> const& integral = event->sum_integral;


  // create down-sampled integral waveform
  int ds_wfm_nsamps = (int) event->nsamps / _down_sample_factor;

  std::vector<double> ds_integral(ds_wfm_nsamps);
  bool in_pulse = false;
  for (int i=0; i<ds_wfm_nsamps; i++) {
    ds_integral[i] = integral[i*_down_sample_factor];
    
    // If two consecuative pulses in the down-sampled integral are vertically separated by
    // more than _pulse_start_threshold, then there's a pulse somewhere nearby

    if ( i>1 && !in_pulse && (ds_integral[i]-ds_integral[i-1] < _pulse_start_threshold) ) {
      in_pulse = true;
      //now do fine-grained search for pulse start point
      for (int ii=(i-2)*_down_sample_factor; ii<i*_down_sample_factor; ii++) {
        if (waveform[ii] < _pulse_start_amp) {
          event->pulse_start_times.push_back(event->SampleToTime(ii-1));
          break;
        }
      }//end fine-grained search
    }
    else if (in_pulse && (std::fabs(ds_integral[i]-ds_integral[i-1]) < std::fabs(_pulse_end_threshold)) ) {
      in_pulse = false;
      event->pulse_end_times.push_back(event->SampleToTime(i*_down_sample_factor));
    }
  }// end loop over down-sampled integral

  event->npulses = std::min(event->pulse_start_times.size(), event->pulse_end_times.size());
  
}
