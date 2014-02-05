#include "ZeroSuppressor.hh"

#include <cmath>


ZeroSuppressor::ZeroSuppressor(CfgReader const& cfg):
  module_name("ZeroSuppressor"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _threshold(cfg.getParam<double>(module_name, "threshold", 0))
{ }


int ZeroSuppressor::Process(EventData* event)
{
  if (!_enabled)
    return 0;


  // Loop over the channels
  for (int idx = 0; idx<event->nchans; ++idx) {
    if (!event->baseline_validities[idx])
      return 1;
    vector<double> const& bs_wfm = event->baseline_subtracted_waveforms[idx];

    vector<double> zs_wfm;
    zs_wfm.reserve(bs_wfm.size());
    for (size_t i=0; i<bs_wfm.size(); ++i) {
      if (std::fabs(bs_wfm[i]) > _threshold)
        zs_wfm.push_back(bs_wfm[i]);
      else
        zs_wfm.push_back(0);
    }

    event->zero_suppressed_waveforms.push_back(zs_wfm);


  }// end loop over channels
  

  return 1;
}
