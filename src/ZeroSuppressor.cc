#include "ZeroSuppressor.hh"
#include "ChannelData.hh"
#include <cmath>
#include <algorithm>
#include <vector>


ZeroSuppressor::ZeroSuppressor(const Setting & cfg) : Module(cfg)
{
  cfg.lookupValue("threshold", threshold);
  cfg.lookupValue("edge_threshold", edge_threshold);

  val = -1;
}

void ZeroSuppressor::Initialize()
{
  Module::Initialize();
  tree->Branch("val", &val, "val/I");
}

void ZeroSuppressor::Process(EventData* event)
{


  
  // Loop over the channels
  for (int idx = 0; idx<event->nchans; ++idx) {
    
    ChannelData* channel = event->GetChannel(idx);
    
    vector<double> const& bs_wfm = channel->baseline_subtracted_waveform;
    vector<double> & zs_wfm = channel->zero_suppressed_waveform;

    // initialize the waveform to zeros
    zs_wfm.clear();
    zs_wfm.resize(bs_wfm.size());

    if (!channel->baseline_valid)
      continue;

    // Look for regions that are past threshold. If in such a region,
    // (1) search back to the last sample that is within edge_threshold, and
    // (2) search forward to the first sample that is within edge_threshold.
    // These define endpoints for un-zero-suppressed regions; everything
    // else is zero-suppressed.
    int start_samp=0, end_samp=0;
    int samp=0;
    while (samp < event->nsamps) {
      if (std::fabs(bs_wfm[samp]) > threshold) {


        // now search back to last sample that is within _edge_threshold
        int tmp_samp = samp;
        while ( tmp_samp >= 0 && std::fabs(bs_wfm[tmp_samp]) > edge_threshold ) {
          --tmp_samp;
          start_samp = tmp_samp;
        }
        
        // and search forward to first sample that is within _edge_threshold
        tmp_samp = samp;
        while ( tmp_samp < event->nsamps && std::fabs(bs_wfm[tmp_samp]) > edge_threshold ) {
          ++tmp_samp;
          end_samp = tmp_samp;
        }

        if (end_samp >= event->nsamps) end_samp = event->nsamps;
        
        // now copy the non-zero-suppressed region to the waveform
        for (tmp_samp=start_samp; tmp_samp<=end_samp; tmp_samp++)
          zs_wfm[tmp_samp] = bs_wfm[tmp_samp];
        
        // continue the algorithm from the last non-zero-suppressed point
        samp = end_samp+1;
      }
      else //this sample is below threshold -- keep zs_wfm at the initialized zero
        ++samp;
      
    }// end loop over samps

    
  }// end loop over channels




  

  
  // This must be the last call within this function.
  Module::Process();
}


void ZeroSuppressor::Finalize(TTree* master)
{
  Module::Finalize(master);
}
