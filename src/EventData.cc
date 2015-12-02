#include "EventData.hh"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLine.h"
#include "TBox.h"
#include "TPad.h"
#include "TList.h"
#include "TGaxis.h"
#include "TMarker.h"
#include "TStyle.h"
#include <complex> //abs
#include <algorithm> //min_element
#include <iostream>  // std::cerr
#include <stdexcept> // std::out_of_range
#include <vector>

EventData::EventData() :
  // event level metadata
  run_id(-1),
  event_id(-1),
  timestamp_sec(-1),
  timestamp_usec(-1),
  nchans(-1),
  nsamps(-1),
  us_per_samp(-1),
  trigger_index(-1),
  trigger_offset(-1),
  adc_bits(-1),

  channels(),
  sumchannel(),
  npulses(-1),
  pulses(),
  roi()

{}

void EventData::Clear()
{
  run_id = -1;
  event_id = -1;
  nchans = -1;
  nsamps = -1;
  us_per_samp = -1;
  trigger_index = -1;
  trigger_offset = -1;
  adc_bits = -1;
  //channels.clear();
  std::vector<ChannelData>::iterator it = channels.begin();
  for (; it != channels.end(); it++) it->Clear();
  sumchannel.Clear();
  npulses = 0;
  pulses.clear();
  roi.clear();
}


double EventData::SampleToTime(int samp) const
{
  return (samp-trigger_index)*us_per_samp;
}

int EventData::TimeToSample(double time, bool checkrange) const
{
  int samp = (int)(time/us_per_samp)+trigger_index;
  if (samp > nsamps)
    samp = nsamps;
  else if (samp < 0)
    samp = 0;

  return samp;
}


ChannelData* EventData::GetChannel(int const channel_id)
{
  //if (channels[channel_id]->channel_id != channel_id)
  //  std::cout << "Requesting non-existent channel!" << std::endl;
  //return channels[channel_id];
  std::vector<ChannelData>::iterator it = channels.begin();
  while (it != channels.end() && it->channel_id != channel_id) it++;
  return (it == channels.end() ? 0 : &(*it));
}



