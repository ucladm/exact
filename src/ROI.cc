#include "ROI.hh"
#include <algorithm>
#include <numeric> //accumulate
#include "ChannelData.hh"



ROI::ROI(const Setting & cfg)
  : Module(cfg)
{
  cfg.lookupValue("roi_start", _roi_start);
  cfg.lookupValue("roi_end", _roi_end);
}


void ROI::Initialize()
{
  Module::Initialize();
  tree->Branch("roi", _roi, "roi/F");
  TString suffix = TString::Format("[%d]/F", NCHANS);
  tree->Branch("ch_roi", _ch_roi, "ch_roi"+suffix);
}

void ROI::Process(EventData* event)
{
  // reset tree variables
  for (int i=0; i<event->nchans; ++i) _ch_roi[i] = 0;
  _roi = 0;
  
  int roi_start_index = event->TimeToSample(_roi_start);
  int roi_end_index = event->TimeToSample(_roi_end);
  
  // loop over channels
  for (int ch=0; ch<event->nchans; ++ch) {
    double roi = -std::accumulate(event->GetChannel(ch)->baseline_subtracted_waveform.begin()+roi_start_index,
                                  event->GetChannel(ch)->baseline_subtracted_waveform.begin()+roi_end_index,0.0);
    //double roi = std::accumulate(event->GetChannel(ch)->raw_waveform.begin()+roi_start_index,
    //                             event->GetChannel(ch)->raw_waveform.begin()+roi_end_index, 0.0);
    event->GetChannel(ch)->roi = roi;
    event->roi += roi;

    _ch_roi[ch] = roi;
    _roi += roi;
  }


  Module::Process();
  
}


void ROI::Finalize(TTree* master)
{
  Module::Finalize(master);
}
