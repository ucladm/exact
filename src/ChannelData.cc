#include "ChannelData.hh"


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
  integral_waveform(),
  zero_suppressed_waveform(),
  baseline_mean(-1),
  baseline_sigma(-1),
  baseline_valid(false),
  npulses(0)
{ }
