#include "Converter.hh"
#include "ChannelData.hh"
#include <iostream>
#include <map>
#include <algorithm> //sort
#include <cmath> //fabs

Converter::Converter(CfgReader const& cfg):
  module_name("Converter"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _trigger_index_offset(cfg.getParam<int>(module_name, "trigger_index_offset", 0))
{ }


int Converter::Process(EventData* event, LVDAQHeader & daq_header)
{
  if (!_enabled)
    return 0;
  
  // Event ID should already be set
  if (event->event_id == -1) {
    std::cerr << "Converter: event_id not set yet!" << std::endl;
    return 0;
  }
  

  // Fill event-level info
  event->nchans = daq_header.nchannels;
  event->nsamps = daq_header.nsamps;
  event->us_per_samp = daq_header.sample_interval*1.e-3;
  event->trigger_index = daq_header.trigger_sample;
  event->trigger_index_offset = _trigger_index_offset;



  // Fill channel-level info
  if ((int)event->channels.size() != daq_header.nchannels)
    event->channels.resize(daq_header.nchannels);
  for (int i=0; i<daq_header.nchannels; ++i) {

    // Make a new ChannelData object and fill it.
    ChannelData channel;

    channel.daq_channel_num = i;
    channel.channel_id = i;
    channel.spe_mean = 1;
    daq_header.read_event_channel(event->event_id, i, channel.raw_waveform);
    channel.adc_gain = 1;
    channel.adc_offset = 1;
    channel.adc_range = daq_header.vertical_full_scale[i];


    // Put the completed ChannelData object into the EventData channels array.
    event->channels.push_back(channel);

  }
  
  
  // Trim the waveforms so that they are aligned.
  // _trigger_index_offset is top channels with respect to bottom channel.
  // If >0, trim start of top channels, and end of bottom channel.
  // If <0, trim start of bottom channel, and end of top channels.
  for (int ch=0; ch<event->nchans; ch++) {
    std::vector<double> & waveform = event->GetChannel(ch)->raw_waveform;
    if (ch==BOT_CHANNEL_ID) {
      if (_trigger_index_offset < 0)
        waveform.erase(waveform.begin(), waveform.begin() - _trigger_index_offset);
      else
        waveform.resize(event->nsamps - _trigger_index_offset);
    }
    else {
      if (_trigger_index_offset < 0)
        waveform.resize(event->nsamps + _trigger_index_offset);
      else
        waveform.erase(waveform.begin(), waveform.begin()+_trigger_index_offset);
    }
  } // end loop over channels
  event->nsamps -= std::fabs(_trigger_index_offset);
  
  return 1;
}
