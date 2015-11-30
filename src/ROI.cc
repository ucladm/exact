#include "ROI.hh"
#include <algorithm>
#include <numeric> //accumulate
#include "ChannelData.hh"



ROI::ROI(const Setting & cfg)
  : Module(cfg)
{

  _nroi = cfg["roi_times"].getLength();
  for (int i=0; i<_nroi; ++i) {
    std::vector<float> roi(2);
    roi[0] = cfg["roi_times"][i][0];
    roi[1] = cfg["roi_times"][i][1];
    _roi_times.push_back( roi );
  }
  
}


void ROI::Initialize()
{
  Module::Initialize();

  TString suffix = TString::Format("[%d]/F", _nroi);
  tree->Branch("roi", _roi, "roi"+suffix);

  suffix = TString::Format("[%d]/F", NCHANS*_nroi);
  tree->Branch("ch_roi", _ch_roi, "ch_roi"+suffix);
}

void ROI::Process(EventData* event)
{
  // reset tree variables
  for (int i=0; i<MAXNROI; ++i) _roi[i] = 0;
  for (int i=0; i<event->nchans*MAXNROI; ++i) _ch_roi[i] = 0;

  // loop over channels
  for (int ch=0; ch<event->nchans; ++ch) {

    const std::vector<double> & wfm = event->GetChannel(ch)->baseline_subtracted_waveform;

    for (int n=0; n<_nroi; ++n) {
    
      int roi_start_index = event->TimeToSample(_roi_times[n][0]);
      int roi_end_index = event->TimeToSample(_roi_times[n][1]);

    
      double roi = -std::accumulate(wfm.begin()+roi_start_index,
                                    wfm.begin()+roi_end_index,0.0);
      _ch_roi[n*event->nchans + ch] = roi;
      _roi[n] += roi;
    }//loop over ROI
  }//loop over channels

  for (int n=0; n<_nroi; ++n) {
    event->roi.push_back(_roi[n]);
    for (int ch=0; ch<event->nchans; ++ch) {
      event->GetChannel(ch)->roi.push_back(_ch_roi[n*event->nchans+ch]);
    }
  }
    

  Module::Process();
  
}


void ROI::Finalize(TTree* master)
{
  Module::Finalize(master);
}
