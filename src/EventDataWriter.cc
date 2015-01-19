#include "EventDataWriter.hh"

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

  _tree->Branch("run_id",                &(ptr->run_id),                   "run_id/I");
  _tree->Branch("event_id",              &(ptr->event_id),                 "event_id/I");
  _tree->Branch("nchans",                &(ptr->nchans),                   "nchans/I");
  _tree->Branch("nsamps",                &(ptr->nsamps),                   "nsamps/I");
  _tree->Branch("us_per_samp",           &(ptr->us_per_samp),              "us_per_samp/D");
  _tree->Branch("trigger_index",         &(ptr->trigger_index),            "trigger_index/I");
  _tree->Branch("trigger_index_offset",  &(ptr->trigger_index_offset),     "trigger_index_offset/I");
  _tree->Branch("adc_gains",             &(ptr->adc_gains[0]),             "adc_gains[nchans]/D");
  _tree->Branch("adc_offsets",           &(ptr->adc_offsets[0]),           "adc_offsets[nchans]/D");
  //_tree->Branch("adc_ranges",            &(ptr->adc_ranges),               "adc_ranges[NCHANS]/F");
  //_tree->Branch("spe_means",             &(ptr->spe_means),                "spe_means[NCHANS]/F");
  //_tree->Branch("baseline_means",        &(ptr->baseline_means),           "baseline_means[NCHANS]/F");
  //_tree->Branch("baseline_sigmas",       &(ptr->baseline_sigmas),          "baseline_sigmas[NCHANS]/F");
  ////_tree->Branch("baseline_validities",   &(ptr->baseline_validities),      "baseline_validities[NCHANS]/I");
  _tree->Branch("npulses",               &(ptr->npulses),                  "npulses/I");
  _tree->Branch("pulse_start_times",     &(ptr->pulse_start_times[0]),     "pulse_start_times[npulses]/D");
  //_tree->Branch("pulse_end_times",       &(ptr->pulse_end_times),          "pulse_end_times[npulses]/F");
  //_tree->Branch("pulse_peak_times",      &(ptr->pulse_peak_times),         "pulse_peak_times[npulses]/F");
  //_tree->Branch("pulse_peak_amps",       &(ptr->pulse_peak_amps),          "pulse_peak_amps[npulses]/F");
  _tree->Branch("pulse_integrals",       &(ptr->pulse_integrals[0]),          "pulse_integrals[npulses]/D");

  //_tree->Branch("saturated",             &(ptr->saturated),                "saturated[npulses]/I");
  //_tree->Branch("ch_pulse_integrals",    &(ptr->ch_pulse_integrals));
  //_tree->Branch("ch_5samp_extended_pulse_integrals",      &(ptr->ch_5samp_extended_pulse_integrals));
  //_tree->Branch("ch_10samp_extended_pulse_integrals",     &(ptr->ch_10samp_extended_pulse_integrals));

  //delete ptr;
}

int EventDataWriter::Process(EventData* event)
{
  EventData* ptr = event;
  _tree->SetBranchAddress("run_id",              &(ptr->run_id));
  _tree->SetBranchAddress("event_id",            &(ptr->event_id));
  _tree->SetBranchAddress("nchans",              &(ptr->nchans));
  _tree->SetBranchAddress("adc_gains",           &(ptr->adc_gains[0]));
  _tree->SetBranchAddress("adc_offsets",         &(ptr->adc_offsets[0]));
  _tree->SetBranchAddress("npulses",             &(ptr->npulses));
  _tree->SetBranchAddress("pulse_start_times",   &(ptr->pulse_start_times[0]));
  _tree->SetBranchAddress("pulse_integrals",     &(ptr->pulse_integrals[0]));



  _tree->Fill();
  
  return 0;
}

void EventDataWriter::Finalize(TFile* outfile)
{
  outfile->cd();
  _tree->Write();
}
