#include "SumChannel.hh"
#include "ChannelData.hh"
#include <vector>

SumChannel::SumChannel(CfgReader const& cfg):
  module_name("SumChannel"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true))
{ }


int SumChannel::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  event->sumchannel = new ChannelData();
  std::vector<double> & sum = event->sumchannel->raw_waveform;
  
  //Loop over the channels
  for (int idx = 0; idx<event->nchans; ++idx) {

    ChannelData* channel = event->GetChannel(idx);
    
    vector<double> const& wfm = channel->zero_suppressed_waveform;

    // size the sum channel appropriately
    if (idx == 0) {
      sum.resize(wfm.size());
    }

    if (!channel->baseline_valid) {
      sum.clear();
      sum.resize(wfm.size());
      return 0;
    }

    // add waveform to sum channel
    for (size_t i=0; i<wfm.size(); ++i) {
      sum[i] += wfm[i]*channel->adc_gain*1000*2/channel->spe_mean; // ADC gain in V, the sum channel is in unit of PE
    }
    
  }// end loop over channels

  
  return 1;
}
