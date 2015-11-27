#include "BaselineFinder.hh"
#include "ChannelData.hh"
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>


#define BASELINE_INIT -999

using namespace std;


BaselineFinder::BaselineFinder(const Setting & cfg) : Module(cfg)
{
  cfg.lookupValue("mode", _mode);

  // fixed baseline params
  cfg.lookupValue("start_time", _start_time);
  cfg.lookupValue("end_time", _end_time);
  cfg.lookupValue("threshold", _threshold);

  // moving baseline parameters
  cfg.lookupValue("pre_samps", _pre_samps);
  cfg.lookupValue("post_samps", _post_samps);
  cfg.lookupValue("max_sigma", _max_sigma);
  cfg.lookupValue("max_amplitude", _max_amplitude);
  cfg.lookupValue("baseline_fixed_window", _baseline_fixed_window);

  baseline_mean = new Float_t[NCHANS];
  baseline_sigma = new Float_t[NCHANS];

}

void BaselineFinder::Initialize()
{
  Module::Initialize();
  TString suffix = TString::Format("[%d]/F", NCHANS);
  tree->Branch("baseline_mean", baseline_mean, "baseline_mean"+suffix);
  tree->Branch("baseline_sigma",baseline_sigma,"baseline_sigma"+suffix);
}

void BaselineFinder::Process(EventData* event)
{
  if (_mode == "FIXED")
    fixed_baseline(event);
  else if (_mode == "MOVING")
    moving_baseline(event);
  else {
    std::cout << "BaselineFinder mode not recognized. Using FIXED."<<std::endl;
    fixed_baseline(event);
  }


  // This must be the last call within this function.
  Module::Process();
}

void BaselineFinder::Finalize(TTree* master)
{
  // This must be the last call within this function.
  Module::Finalize(master);
}


void BaselineFinder::fixed_baseline(EventData* event)
{
  // Loop over channels
  for (int idx = 0; idx<event->nchans; ++idx) {

    ChannelData* channel = event->GetChannel(idx);
    
    vector<double> const& raw = channel->raw_waveform;
    double sum = 0;
    double var = 0;
    int start_samp = event->TimeToSample(_start_time);
    int end_samp = event->TimeToSample(_end_time);
    for (int i = start_samp; i<end_samp; ++i) {
      sum += raw[i];
      var += raw[i]*raw[i];
    }
      
    // parameters used for saturation search
    const int saturating_count = -127;
    bool ch_saturated = false;
      
    for (int i = 0; i<event->nsamps; i++){
          
      if(raw[i]<saturating_count){
        ch_saturated = true;
        break;
      }
    }

    channel->saturated = ch_saturated;
      
    double mean = sum/(end_samp-start_samp);
    channel->baseline_mean = mean;
    channel->baseline_sigma = sqrt(var/(end_samp - start_samp) - mean*mean);
    
    vector<double> & bs_wfm = channel->baseline_subtracted_waveform;
    bs_wfm.resize(raw.size());
    if (channel->baseline_sigma < _threshold) {
      channel->baseline_valid = true;

      // compute the baseline-subtracted and inverted waveform
      for (size_t i=0; i<raw.size(); ++i)
        bs_wfm[i] = raw[i] - mean;
    }
    else
      channel->baseline_valid = false;
  } // end loop over channels
}

void BaselineFinder::interpolate_baseline(vector<double> & baseline, int start, int end)
{
  double m = (baseline[end] - baseline[start])/(end-start);
  double b = baseline[start] - m*start;
  for (int samp = start+1; samp<end; samp++)
    baseline[samp] = m*samp+b;
}


void BaselineFinder::moving_baseline(EventData* event)
{
  // Loop over channels
  const int saturating_count = -126;
  bool ch_saturated = false;

  for (int idx=0; idx<event->nchans; idx++) {

    ChannelData* channel = event->GetChannel(idx);
    
    vector<double> const& raw = channel->raw_waveform;
    const int nsamps = event->nsamps;
    vector<double> baseline(nsamps, BASELINE_INIT);
    bool in_baseline = false;
    int baseline_start = -1; // start point of baseline. fill in waveform start to baseline_start after main loop.
    double mean = 0;
    double variance = 0;
    bool baseline_valid = false;
    int window_size = _pre_samps+_post_samps+1;
    double last_baseline_samp = 0;

    if(raw[idx]<=saturating_count)
      ch_saturated = true;

    // start the mean off with the very beginning of the waveform

    for (int i=0; i<_pre_samps+_post_samps+1; i++) {
      mean += raw[i]/window_size;
      variance += raw[i]*raw[i]/window_size;
    }
    variance -= mean*mean;
    // now traverse through the waveform
    for (int samp = _pre_samps; samp<nsamps-_post_samps-1; samp++) {
      if (!in_baseline) { // previously not in baseline

        // determine if now in baseline
        if (variance < _max_sigma*_max_sigma) {
          //now in baseline
          in_baseline = true;
          if (!baseline_valid)
            baseline_valid = true;
          if (baseline_start == -1)
            baseline_start = samp;
          baseline[samp] = mean;
          last_baseline_samp = samp;
        }
      }
    
      else { // previously in baseline

        // determine if still in baseline
        if (std::abs(raw[samp+_post_samps]-baseline[samp-1]) > _max_amplitude) {
          //no longer in baseline
          in_baseline = false;
        }

        else {
          //still in baseline
          baseline[samp] = mean;
          last_baseline_samp = samp;
        }
        
      }

      // First undo the 2nd term of the variance
      variance += mean*mean;

      // Then calculate new mean
      mean += (raw[samp+_post_samps] - raw[samp-_pre_samps])/window_size;

      // Calculte new variance
      variance += ((raw[samp+_post_samps]*raw[samp+_post_samps] -
                    raw[samp-_pre_samps]*raw[samp-_pre_samps])/window_size
                   - mean*mean);
    }// end loop over waveform


    // fill in the beginning of the baseline
    for (int samp=0; samp<baseline_start; samp++)
      baseline[samp] = baseline[baseline_start];

    // fill in the end of the baseline
    for (int samp=last_baseline_samp+1; samp<nsamps; samp++)
      baseline[samp] = baseline[last_baseline_samp];


    // subtract off the baseline
    vector<double> & bswfm = channel->baseline_subtracted_waveform;
    bswfm.resize(nsamps);
    
    for (int samp=0; samp<nsamps; samp++) {

      // interpolate the regions that are not yet set.
      if (baseline[samp] == BASELINE_INIT) {
        bool interpolate = true;
        int start = samp-1;
        int end = samp;
        while (interpolate) {
          ++end;
          if (end >= nsamps) {
            //std::cout << "Event "<<event->event_id<<" ch "<<channel->channel_id<<": "
            //          << "AHHH interpolating the end!"<<std::endl;
            break;
          }
          if (baseline[end] != BASELINE_INIT) {
            interpolate = false;
          }
        }
        interpolate_baseline(baseline, start, end);
      }

      bswfm[samp] = raw[samp] - baseline[samp];
     
    }
    
    channel->saturated = ch_saturated;
    //channel->baseline_mean = 1;
    //channel->baseline_sigma = 1;
    channel->baseline_mean = 0;
    channel->baseline_sigma = 0;
    // For moving baseline, baseline mean and sigma aren't very meaningful. So fill with fixed window calculation
    for (int samp = 0; samp < event->TimeToSample(_baseline_fixed_window); ++samp) {
      channel->baseline_mean += raw[samp];
      channel->baseline_sigma += raw[samp]*raw[samp];
    }
    channel->baseline_mean /= event->TimeToSample(_baseline_fixed_window);
    channel->baseline_sigma = std::sqrt(channel->baseline_sigma/event->TimeToSample(_baseline_fixed_window) -
                                        channel->baseline_mean*channel->baseline_mean);
    channel->baseline_valid = true;
    //event->baseline_subtracted_waveforms.push_back(bswfm);

    baseline_mean[channel->channel_id] = channel->baseline_mean;
    baseline_sigma[channel->channel_id] = channel->baseline_sigma;

  }// end loop over channels

}
