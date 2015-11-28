#include "PulseFinder.hh"
#include <algorithm> //min
#include <numeric>   //accumulate
#include <cmath>

using namespace std;

PulseFinder::PulseFinder(const Setting & cfg) : Module(cfg)
{
  cfg.lookupValue("mode", _mode);
  cfg.lookupValue("down_sample_factor", _down_sample_factor);
  cfg.lookupValue("pulse_start_threshold", _pulse_start_threshold);
  cfg.lookupValue("pulse_start_amp", _pulse_start_amp);
  cfg.lookupValue("pulse_end_threshold", _pulse_end_threshold);

  //pulse_start = new Float_t[MAXNPULSES];
  //pulse_end = new Float_t[MAXNPULSES];
}

void PulseFinder::Initialize()
{
  Module::Initialize();
  tree->Branch("npulses", &npulses, "npulses/I");
  tree->Branch("pulse_start", pulse_start, "pulse_start[npulses]/F");
  tree->Branch("pulse_end", pulse_end, "pulse_end[npulses]/F");
}

void PulseFinder::Process(EventData* event)
{
  npulses = 0;
  if (_mode=="THRESHOLD")
    npulses = ThresholdSearch(event);
  else if (_mode=="INTEGRAL")
    npulses = IntegralSearch(event);
  else {
    std::cout << "PulseFinder mode not recognized. Using THRESHOLD."<<std::endl;
    npulses = ThresholdSearch(event);
  }

  if (npulses>0)
    EvaluatePulses(event);


  // Set variables
  for (int i=0; i<npulses; i++) {
    pulse_start[i] = event->pulses[i].start_time;
    pulse_end[i] = event->pulses[i].end_time;
  }




  
  
  Module::Process();  
}

void PulseFinder::Finalize(TTree* master)
{
  Module::Finalize(master);
}

void PulseFinder::EvaluatePulses(EventData* event)
{
  vector<double> const& waveform = event->sumchannel.raw_waveform;
  
  for (int i=0; i<event->npulses; i++) {
    int start_index = event->TimeToSample(event->pulses[i].start_time);
    int end_index = event->TimeToSample(event->pulses[i].end_time);
    int peak_index = std::min_element(waveform.begin() + start_index, waveform.begin() + end_index)-waveform.begin();
    event->pulses[i].peak_time = event->SampleToTime(peak_index);
    event->pulses[i].peak_amp = waveform[peak_index];
    event->pulses[i].integral = event->sumchannel.integral_waveform[start_index-1] - event->sumchannel.integral_waveform[end_index-1];
    
  }
  
   
}

int PulseFinder::ThresholdSearch(EventData* event)
{
  vector<double> const& waveform = event->sumchannel.raw_waveform;

  bool in_pulse = false;
  for (size_t i=1; i<waveform.size(); i++) {
    if (!in_pulse) {
      if (waveform[i] < _pulse_start_threshold) {
        in_pulse = true;
        PulseData pulse;
        pulse.start_time = event->SampleToTime(i);
        event->pulses.push_back(pulse);
      }
    }
    else { //in_pulse
      if (waveform[i] > _pulse_start_threshold) {
        in_pulse = false;
        event->pulses.back().end_time = event->SampleToTime(i);
      }
    }
  }
  event->npulses = event->pulses.size();
  return event->npulses;
}

int PulseFinder::IntegralSearch(EventData* event)
{

  vector<double> const& waveform = event->sumchannel.raw_waveform;
  vector<double> const& integral = event->sumchannel.integral_waveform;

  // create down-sampled integral waveform
  int ds_wfm_nsamps = (int) event->nsamps / _down_sample_factor;

  std::vector<double> ds_integral(ds_wfm_nsamps);
  bool in_pulse = false;
    
  for (int i=0; i<ds_wfm_nsamps; i++) {
    ds_integral[i] = integral[i*_down_sample_factor];

    // If two consecuative pulses in the down-sampled integral are vertically separated by
    // more than _pulse_start_threshold, then there's a pulse somewhere nearby
      
    if ( i>2 && !in_pulse && (ds_integral[i]-ds_integral[i-1] < _pulse_start_threshold) ) {
      in_pulse = true;
      //now do fine-grained search for pulse start point
      for (int ii=(i-2)*_down_sample_factor; ii<i*_down_sample_factor; ii++) {
        if (waveform[ii] < _pulse_start_amp) {
          PulseData pulse;
          pulse.start_time = event->SampleToTime(ii-1);
          event->pulses.push_back(pulse);
          break;
        }
      }//end fine-grained search
    }
    else if (in_pulse && i+1 < ds_wfm_nsamps-1 &&
             (std::fabs(ds_integral[i+1]-ds_integral[i-1]) < std::fabs(_pulse_end_threshold)) ) {
      in_pulse = false;
      event->pulses.back().end_time = event->SampleToTime(i*_down_sample_factor);
    }

    if(in_pulse && (i==ds_wfm_nsamps-1)){
      in_pulse = false;
      event->pulses.back().end_time = event->SampleToTime(i*_down_sample_factor);
    }

  }// end loop over down-sampled integral

  event->npulses = event->pulses.size();
  for (int i=0; i<event->npulses; ++i)
    event->pulses[i].pulse_id = i;
    
  //if(event->pulse_start_times.size()!=event->pulse_end_times.size())
    //std::cout<<"the size of pulse_start_times is not equal to pulse_end_times!"<<std::endl;

  return event->npulses;
        
}
