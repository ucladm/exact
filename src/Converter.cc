#include "Converter.hh"

#include <iostream>
#include <map>
#include <algorithm> //sort
#include <cmath> //fabs

Converter::Converter(CfgReader const& cfg):
  module_name("Converter"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", true)),
  _trigger_index_offset(cfg.getParam<int>(module_name, "trigger_index_offset", 0))
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
  event->trigger_index_offset = _trigger_index_offset;

  // Fill channel-level info
  for (int i=0; i<DAQ_header.getNchans(); ++i) {
    event->channel_nums.push_back(DAQ_header.WorkingChannelNbr.at(i));
    event->channel_ids.push_back(i);
    event->spe_means.push_back(1);
    event->raw_waveforms.push_back(DAQ_header.ReadSingleChannel(event->event_id, DAQ_header.WorkingChannelNbr.at(i)));
  }

  
  // Check that channel numbers match first event
  if (event->event_id == 1) {
    for (int i=0; i<event->nchans; i++) {
      _initial_channel_nums.push_back(event->channel_nums[i]);
    }
  }
  else {
    for (int i=0; i<event->nchans; i++) {
      if (event->channel_nums[i] != _initial_channel_nums[i])
        std::cerr << "Event "<<event->event_id<<" channels in wrong order!"<<std::endl;
    }
  }
  
  
  // Trim the waveforms so that they are aligned.
  // _trigger_index_offset is top channels with respect to bottom channel.
  // If >0, trim start of top channels, and end of bottom channel.
  // If <0, trim start of bottom channel, and end of top channels.
  for (int ch=0; ch<event->nchans; ch++) {
    std::vector<double> & waveform = event->raw_waveforms[ch];
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
