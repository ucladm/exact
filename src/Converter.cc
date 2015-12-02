#include "Converter.hh"
#include "ChannelData.hh"

#include <iostream>
#include <map>
#include <algorithm> //sort
#include <cmath> //fabs

using namespace std;

Converter::Converter(const Setting & cfg) : Module(cfg)
{
  cfg.lookupValue("trigger_offset", trigger_offset);
}

void Converter::Initialize()
{
  Module::Initialize();
  tree->Branch("run_id",    &run_id,    "run_id/I");
  tree->Branch("subrun_id", &subrun_id, "subrun_id/I");
  tree->Branch("event_id",  &event_id,  "event_id/I");
  tree->Branch("nchans",    &nchans,    "nchans/I");
  tree->Branch("nsamps",    &nsamps,    "nsamps/I");
  tree->Branch("timestamp_sec", &timestamp_sec, "timestamp_sec/I");
  tree->Branch("timestamp_usec", &timestamp_usec, "timestamp_usec/I");
  
  
}

void Converter::Process(EventData* event, LVDAQHeader & daq_header)
{
  
  // Event ID should already be set
  if (event->event_id == -1) {
    std::cerr << "Converter: event_id not set yet!" << std::endl;
    return;
  }

  run_id = daq_header.run_id;
  subrun_id = 1; //to be implemented later.
  event_id = event->event_id;
  nchans = daq_header.nchannels;
  nsamps = daq_header.nsamps;
  daq_header.read_event_timestamp(event->event_id);
  timestamp_sec = (int) daq_header.event_sec;
  timestamp_usec = (int) daq_header.event_millisec;


  // Fill EventData
  event->run_id = run_id;
  event->nchans = nchans;
  event->nsamps = nsamps;
  event->us_per_samp = daq_header.sample_interval*1.e-3; // ns -> us
  event->trigger_index = daq_header.trigger_sample;
  event->trigger_offset = trigger_offset;
  event->timestamp_sec = timestamp_sec;
  event->timestamp_usec = timestamp_usec;


    // Fill channel-level info
  if ((int)event->channels.size() != daq_header.nchannels)
    event->channels.resize(daq_header.nchannels);
  for (int i=0; i<daq_header.nchannels; ++i) {

    // Make a new ChannelData object and fill it.
    ChannelData channel;

    channel.daq_channel_num = i;
    channel.channel_id = i;
    channel.spe_mean = -56.34; //hard-coded for now. warm temperature gain with rough Gaussian fit to 3" PMT
    daq_header.read_event_channel(event->event_id, i, channel.raw_waveform);
    channel.adc_gain = 1;
    channel.adc_offset = 1;
    channel.adc_range = daq_header.vertical_full_scale[i];


    // Put the completed ChannelData object into the EventData channels array.
    event->channels[i] = channel;

  }
  
  // Trim the waveforms so that they are aligned.
  // trigger_offset is top channels with respect to bottom channel.
  // If <0, expect that TOP channels are ahead of BOT channels: trim start of TOP channels and end of BOT channel.
  // If >0, expect that BOT channel is ahead of TOP channels: trim start of BOT channel and end of TOP channels.
  const int trigger_index_offset = trigger_offset / event->us_per_samp;
  for (int ch=0; ch<event->nchans; ch++) {
    std::vector<double> & waveform = event->GetChannel(ch)->raw_waveform;
    if (trigger_offset < 0) { // expect that TOP channels are ahead of BOT channel
      if (ch==BOT_CHANNEL_ID)
        waveform.resize(event->nsamps + trigger_index_offset);
      else
        waveform.erase(waveform.begin(), waveform.begin() - trigger_index_offset);
    }
    else { //expect that BOT channel is ahead of TOP channels
      if (ch==BOT_CHANNEL_ID)
        waveform.erase(waveform.begin(), waveform.begin() + trigger_index_offset);
      else
        waveform.resize(event->nsamps - trigger_index_offset);
    }
  } // end loop over channels
  event->nsamps -= std::fabs(trigger_index_offset);

  bool PRINT = false; 
  if (PRINT) {
    cout << "event "<<event->event_id<<" "<<event->timestamp_sec<<" "<<event->timestamp_usec<<endl;
    for (int ch=0; ch<event->nchans; ++ch) {
      cout << "event "<<event->event_id<<" ch "<<event->GetChannel(ch)->channel_id<<endl;
      for (int i=0; i<event->nsamps; ++i) cout << event->GetChannel(ch)->raw_waveform[i]<<" ";
      cout << endl << endl;
    }
  }


  // This must be the last call within this function.
  Module::Process();
}

void Converter::Finalize(TTree* master)
{
  // This must be the last call within this function.
  Module::Finalize(master);
}
