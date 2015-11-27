#include "SumChannel.hh"
#include "ChannelData.hh"
#include <vector>

SumChannel::SumChannel(const Setting & cfg) : Module(cfg)
{ }


void SumChannel::Initialize()
{
  Module::Initialize();
}

void SumChannel::Process(EventData* event)
{
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

  // This must be the last call within this function.
  Module::Process();
}


void SumChannel::Finalize(TTree* master)
{
  Module::Finalize(master);
}
