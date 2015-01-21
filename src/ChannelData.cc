#include "ChannelData.hh"
#include <algorithm> //min, max

ChannelData::ChannelData():
  daq_channel_num(-1),
  channel_id(-1),
  adc_gain(-1),
  adc_offset(-1),
  adc_range(-1),
  spe_mean(-1),
  saturated(false),
  raw_waveform(),
  baseline_subtracted_waveform(),
  zero_suppressed_waveform(),
  integral_waveform(),
  baseline_mean(-1),
  baseline_sigma(-1),
  baseline_valid(false),
  npulses(0)
{ }


double ChannelData::integrate(int start_samp, int end_samp) const
{
  //if ( start_samp < 0 || end_samp < 0   || start_samp >= integral_waveform.size() || end_samp > integral_waveform.size())

  int start = std::max(start_samp, 1);
  int end   = std::min(end_samp, (int)integral_waveform.size());

  double result = integral_waveform[end-1] - integral_waveform[start-1];
  
  if (start_samp == 0)
    result += integral_waveform[0];
  
  return result;
}
