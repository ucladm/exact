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
  

  // channel arrays

  const int max_nchans = 8;
  int      channel_id     [max_nchans];
  double   spe_mean       [max_nchans];
  bool     ch_saturated   [max_nchans];
  double   baseline_mean  [max_nchans];
  double   baseline_sigma [max_nchans];
  bool     baseline_valid [max_nchans];
  double   ch_roi         [max_nchans];
  
  _tree->Branch("channel_id",      channel_id,       "channel_id[nchans]/I");
  _tree->Branch("spe_mean",        spe_mean,         "spe_mean[nchans]/D");
  _tree->Branch("ch_saturated",    ch_saturated,     "ch_saturated[nchans]/O");
  _tree->Branch("baseline_mean",   baseline_mean,    "baseline_mean[nchans]/D");
  _tree->Branch("baseline_sigma",  baseline_sigma,   "baseline_sigma[nchans]/D");
  _tree->Branch("baseline_valid",  baseline_valid,   "baseline_valid[nchans]/O");
  _tree->Branch("ch_roi",          ch_roi,           "ch_roi[nchans]/D");
  

  // pulse arrays
  const int max_npulses = 10;
  int      pulse_id        [max_npulses];
  double   start_time      [max_npulses];
  double   end_time        [max_npulses];
  double   peak_time       [max_npulses];
  double   peak_amp        [max_npulses];
  bool     p_saturated     [max_npulses];
  double   integral        [max_npulses];
  double   fixed_int1      [max_npulses];
  double   fixed_int2      [max_npulses];
  double   f90             [max_npulses];
  
  _tree->Branch("npulses",     &(ptr->npulses),  "npulses/I");
  _tree->Branch("pulse_id",      pulse_id,       "pulse_id[npulses]/I");
  _tree->Branch("start_time",    start_time,     "start_time[npulses]/D");
  _tree->Branch("end_time",      end_time,       "end_time[npulses]/D");
  _tree->Branch("peak_time",     peak_time,      "peak_time[npulses]/D");
  _tree->Branch("peak_amp",      peak_amp,       "peak_amp[npulses]/D");
  _tree->Branch("p_saturated",   p_saturated,    "p_saturated[npulses]/O");
  _tree->Branch("integral",      integral,       "integral[npulses]/D");
  _tree->Branch("fixed_int1",    fixed_int1,     "fixed_int1[npulses]/D");
  _tree->Branch("fixed_int2",    fixed_int2,     "fixed_int2[npulses]/D");
  _tree->Branch("f90",           f90,            "f90[npulses]/D");

  _tree->Branch("roi",           &(ptr->roi),    "roi/D");
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

  // Fill some branches by looping over ChannelData objects

  const int nchans = ptr->nchans;

  int     channel_id     [nchans];
  double  spe_mean       [nchans];
  bool    ch_saturated   [nchans];
  double  baseline_mean  [nchans];
  double  baseline_sigma [nchans];
  bool    baseline_valid [nchans];
  double  ch_roi         [nchans];
  
  // loop over channels
  for (int chID = 0; chID<nchans; ++chID) {

    if (chID != event->GetChannel(chID)->channel_id) {
      std::cout << "[" << module_name << "] "
                << "unexpected channel ID!"<<std::endl;
      return 0;
    }

    ChannelData* ch = event->GetChannel(chID);

    channel_id[chID]      = ch->channel_id;
    spe_mean[chID]        = ch->spe_mean;
    ch_saturated[chID]    = ch->saturated;
    baseline_mean[chID]   = ch->baseline_mean;
    baseline_sigma[chID]  = ch->baseline_sigma;
    baseline_valid[chID]  = ch->baseline_valid;
    ch_roi[chID]          = ch->roi;
    
  } // loop over channels
  
  _tree->SetBranchAddress("channel_id",      channel_id);
  _tree->SetBranchAddress("spe_mean",        spe_mean);
  _tree->SetBranchAddress("ch_saturated",    ch_saturated);
  _tree->SetBranchAddress("baseline_mean",   baseline_mean);
  _tree->SetBranchAddress("baseline_sigma",  baseline_sigma);
  _tree->SetBranchAddress("baseline_valid",  baseline_valid);
  _tree->SetBranchAddress("ch_roi",          ch_roi);

  // Fill some branches by looping over PulseData objects
  const int npulses = ptr->npulses;
  int      pulse_id     [npulses];
  double   start_time   [npulses];
  double   end_time     [npulses];
  double   peak_time    [npulses];
  double   peak_amp     [npulses];
  bool     p_saturated  [npulses];
  double   integral     [npulses];
  double   fixed_int1   [npulses];
  double   fixed_int2   [npulses];
  double   f90          [npulses];

  // loop over pulses
  for (int i=0; i<npulses; ++i) {
    PulseData & p = event->pulses[i];
    pulse_id[i]    = p.pulse_id;
    start_time[i]  = p.start_time;
    end_time[i]    = p.end_time;
    peak_time[i]   = p.peak_time;
    peak_amp[i]    = p.peak_amp;
    p_saturated[i] = p.saturated;
    integral[i]    = p.integral;
    fixed_int1[i]  = p.fixed_int1;
    fixed_int2[i]  = p.fixed_int2;
    f90[i]         = p.f90;
  } //loop over pulses

  _tree->SetBranchAddress("npulses",     &(ptr->npulses));
  _tree->SetBranchAddress("pulse_id",    pulse_id);
  _tree->SetBranchAddress("start_time",  start_time);
  _tree->SetBranchAddress("end_time",    end_time);
  _tree->SetBranchAddress("peak_time",   peak_time);
  _tree->SetBranchAddress("peak_amp",    peak_amp);
  _tree->SetBranchAddress("p_saturated", p_saturated);
  _tree->SetBranchAddress("integral",    integral);
  _tree->SetBranchAddress("fixed_int1",  fixed_int1);
  _tree->SetBranchAddress("fixed_int2",  fixed_int2);
  _tree->SetBranchAddress("f90",         f90);

  _tree->SetBranchAddress("roi",         &(ptr->roi));

  _tree->Fill();
  
  return 0;
}

void EventDataWriter::Finalize(TFile* outfile)
{
  outfile->cd();
  _tree->Write();
}
