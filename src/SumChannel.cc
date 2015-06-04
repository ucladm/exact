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

  ChannelData sum_ch;
  std::vector<double> & sum = sum_ch.raw_waveform;
  
  //Loop over the channels
  for (int ch = 0; ch<event->nchans; ++ch) {

    ChannelData* channel = event->GetChannel(ch);
    
    vector<double> const& wfm = channel->zero_suppressed_waveform;
    if (wfm.size() == 0) {
      sum.clear();
      break;
    }
    
    // size the sum channel appropriately
    if (ch == 0) {
      sum.resize(wfm.size());
    }

    if (!channel->baseline_valid) {
      sum.clear();
      sum.resize(wfm.size());
      break;
    }

    // add waveform to sum channel
    for (size_t i=0; i<wfm.size(); ++i) {
      sum[i] += wfm[i]*channel->adc_gain*1000*2/channel->spe_mean; // ADC gain in V, the sum channel is in unit of PE
    }
    
  }// end loop over channels

  event->sumchannel = sum_ch;
  
  return 1;
}
