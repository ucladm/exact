#include "EventDataWriter.hh"
#include "ChannelData.hh"
#include <iostream>

EventDataWriter::EventDataWriter(CfgReader const& cfg):
  module_name("EventDataWriter"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", false))
{


}


void EventDataWriter::Initialize()
{
  _tree = new TTree("Events", "Reconstructed data tree");
  EventData* ptr = new EventData();

  // event level metadata
  _tree->Branch("run_id",                &(ptr->run_id),                   "run_id/I");
  _tree->Branch("event_id",              &(ptr->event_id),                 "event_id/I");
  _tree->Branch("nchans",                &(ptr->nchans),                   "nchans/I");
  _tree->Branch("nsamps",                &(ptr->nsamps),                   "nsamps/I");
  _tree->Branch("us_per_samp",           &(ptr->us_per_samp),              "us_per_samp/D");
  _tree->Branch("trigger_index",         &(ptr->trigger_index),            "trigger_index/I");
  _tree->Branch("trigger_index_offset",  &(ptr->trigger_index_offset),     "trigger_index_offset/I");

  const int max_nchans = 8;
  
  int channel_id[max_nchans];
  double spe_mean[max_nchans];
  bool saturated[max_nchans];
  double baseline_mean[max_nchans];
  
  _tree->Branch("channel_id", channel_id, "channel_id[nchans]/I");
  _tree->Branch("spe_mean", spe_mean, "spe_mean[nchans]/D");
  _tree->Branch("saturated", saturated, "saturated[nchans]/O");
  _tree->Branch("baseline_mean", baseline_mean, "baseline_mean[nchans]/D");

  _tree->Branch("npulses", &(ptr->npulses), "npulses/I");
 
}

int EventDataWriter::Process(EventData* event)
{
  EventData* ptr = event;

  // Event metadata
  _tree->SetBranchAddress("run_id",              &(ptr->run_id));
  _tree->SetBranchAddress("event_id",            &(ptr->event_id));
  _tree->SetBranchAddress("nchans",              &(ptr->nchans));
  _tree->SetBranchAddress("nsamps",              &(ptr->nsamps));
  _tree->SetBranchAddress("us_per_samp",         &(ptr->us_per_samp));
  _tree->SetBranchAddress("trigger_index",       &(ptr->trigger_index));
  _tree->SetBranchAddress("trigger_index_offset",&(ptr->trigger_index_offset));

  // The rest of the branches must be filled by looping over the ChannelData or PulseData objects.

  const int nchans = ptr->nchans;

  int channel_id[nchans];
  double spe_mean[nchans];
  bool saturated[nchans];
  double baseline_mean[nchans];
  
  // loop over channels
  for (int chID = 0; chID<nchans; ++chID) {

    if (chID != event->channels[chID]->channel_id) {
      std::cout << "[" << module_name << "] "
                << "unexpected channel ID!"<<std::endl;
      return 0;
    }

    ChannelData* ch = event->GetChannel(chID);

    channel_id[chID] = ch->channel_id;
    spe_mean[chID] = ch->spe_mean;
    saturated[chID] = ch->saturated;
    baseline_mean[chID] = ch->baseline_mean;
  } // loop over channels
  
  _tree->SetBranchAddress("channel_id", channel_id);
  _tree->SetBranchAddress("spe_mean", spe_mean);
  _tree->SetBranchAddress("saturated", saturated);
  _tree->SetBranchAddress("baseline_mean", baseline_mean);


  _tree->SetBranchAddress("npulses", &(ptr->npulses));

  _tree->Fill();
  
  return 0;
}

void EventDataWriter::Finalize(TFile* outfile)
{
  outfile->cd();
  _tree->Write();
}
