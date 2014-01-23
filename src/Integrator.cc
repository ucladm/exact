#include "Integrator.hh"

Integrator::Integrator(CfgReader const& cfg):
  module_name("Integrator"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true))
{ }


int Integrator::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  // Loop over channels
  for (int idx = 0; idx<event->nchans; ++idx) {
    ChannelData& chData = event->channels[idx];

    if (!chData.baseline_valid)
      return 1;
    
    vector<double> const& bs_wfm = chData.baseline_subtracted_waveform;


    
    // integral should start at t=0, NOT samp=0
    // so compute pretrigger sum first, then subtract from full integral
    double pretrigger_sum = 0;
    for (int i=0; i<event->trigger_index; ++i)
      pretrigger_sum += bs_wfm[i];

    vector<double> integral;
    integral.reserve(bs_wfm.size());
    double sum = 0;
    for (size_t i=0; i<bs_wfm.size(); ++i) {
      integral.push_back(sum - pretrigger_sum);
      sum += bs_wfm[i];
    }

    
    chData.integral = integral;
    
  } // end loop over channels

         
  return 1;
}
