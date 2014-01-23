#include "Converter.hh"

#include "ChannelData.hh"
#include <iostream>

Converter::Converter(CfgReader const& cfg):
  module_name("Converter"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true))
{ }


int Converter::Process(EventData* event, DAQheader & DAQ_header)
{
  if (!_enabled)
    return 0;
  
  // Event ID should already be set
  if (event->event_id == -1) {
    std::cerr << "Converter: event_id not set yet!" << std::endl;
    return 0;
  }
  

  // Fill some event-level info
  event->nchans = DAQ_header.getNchans();
  event->nsamps = DAQ_header.getTotSampleNbr();
  event->us_per_samp = DAQ_header.getTimeInterval_us();
  event->trigger_index = DAQ_header.getTriggerIndex();

  // Fill some channel-level info, including instantiating the
  // ChannelData objects
  for (int i=0; i<DAQ_header.getNchans(); ++i) {
    ChannelData chData;
    chData.event_id = event->event_id;
    chData.channel_id = DAQ_header.WorkingChannelNbr.at(i);
    chData.raw_waveform = DAQ_header.ReadSingleChannel(event->event_id, chData.channel_id);
    chData.trigger_index = event->trigger_index;
    chData.us_per_samp = event->us_per_samp;
    event->channels.push_back(chData);
  }
  
  return 1;
}
