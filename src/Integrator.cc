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

    if (!event->baseline_validities[idx])
      return 1;
    
    vector<double> const& bs_wfm = event->baseline_subtracted_waveforms[idx];


    vector<double> integral;
    integral.reserve(bs_wfm.size());
    double sum = 0;
    for (size_t i=0; i<bs_wfm.size(); ++i) {
      integral.push_back(sum);
      sum += bs_wfm[i];
    }

    
    event->integrals.push_back(integral);
    
  } // end loop over channels

         
  return 1;
}
