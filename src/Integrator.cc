#include "Integrator.hh"
#include "ChannelData.hh"

Integrator::Integrator(const Setting & cfg) : Module(cfg)
{ }

void Integrator::Initialize()
{
  Module::Initialize();
}

void Integrator::Process(EventData* event)
{
  //event->integrals.resize(event->nchans);
  
  // Loop over channels
  for (int idx = 0; idx<event->nchans; ++idx) {

    ChannelData* channel = event->GetChannel(idx);
    
//    vector<double> const& bs_wfm = event->baseline_subtracted_waveforms[idx];
      vector<double> const& bs_wfm = channel->zero_suppressed_waveform;


    vector<double> & integral = channel->integral_waveform;
    integrate(bs_wfm, integral);
    
  } // end loop over channels

  //Now integrate the sum channel
  vector<double> const& sum_wfm = event->sumchannel.raw_waveform;
  vector<double> & sum_integral = event->sumchannel.integral_waveform;

  integrate(sum_wfm, sum_integral);

  
  // This must be the last call within this function.
  Module::Process();
}

void Integrator::integrate(std::vector<double> const&  wfm, std::vector<double> & result)
{
  result.resize(wfm.size());
  double sum = 0;
  for (size_t i=0; i<wfm.size(); ++i) {
    result[i] = sum;
    sum += wfm[i];
  }

}


void Integrator::Finalize(TTree* master)
{
  Module::Finalize(master);
}
