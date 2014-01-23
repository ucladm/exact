#include "Converter.hh"

#include <iostream>
#include <map>

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
  

  // Fill event-level info
  event->nchans = DAQ_header.getNchans();
  event->nsamps = DAQ_header.getTotSampleNbr();
  event->us_per_samp = DAQ_header.getTimeInterval_us();
  event->trigger_index = DAQ_header.getTriggerIndex();

  // Fill channel-level info
  for (int i=0; i<DAQ_header.getNchans(); ++i) {
    event->channel_id.push_back(DAQ_header.WorkingChannelNbr.at(i));
    event->channel_index.insert( std::pair<int, int>(event->channel_id[i], i) );
    event->spe_mean.push_back(1);
    event->raw_waveform.push_back(DAQ_header.ReadSingleChannel(event->event_id, event->channel_id[i]));
    
  }
  
  return 1;
}
