/*
  Analysis code for event reconstruction in UCLA EXACT TPCs.

  Two parts to code:
  1. Event by event processing, including baseline finding, pulse finding, etc
  2. Apply cuts and generate histograms and average waveforms
  
  Compile with Makefile
  
  v0.1 AFan 2013-04-21


  2013-04-21
  implement multiple file processing later

 */


#include <iostream>
#include <string>
#include <fstream>
#include <time.h> 


#include "TROOT.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "DAQheader.hh"
#include "CfgReader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"

#include <string>

using namespace std;


int ProcessEvents(DAQheader& DAQ_header, string cfgFile)
//TTree* ProcessEvents(DAQheader& DAQ_header, string cfgFile)
{

    
  // Instantiate CfgReader
  CfgReader cfg;
  if (!cfg.readCfgFile(cfgFile)) {
    std::cout << "ERROR reading cfg file" << std::endl;
    return 0;
  }

  // Create TTree to hold all processd info and open a file
  // to hold the TTree.
  TTree* tree = new TTree("Events", "EventData");
  TFile* rootfile = new TFile("output.root", "RECREATE");


  // Instantiate EventData; will repopulate this object for each
  // event. Create a branch to hold all event data.
  EventData* event = new EventData();
  //tree->Branch("EventData", "EventData", &event);
  tree->Branch("run_id",              &(event->run_id));
  tree->Branch("event_id",            &(event->event_id));
  tree->Branch("nchans",              &(event->nchans));
  tree->Branch("nsamps",              &(event->nsamps));
  tree->Branch("us_per_samp",         &(event->us_per_samp));
  tree->Branch("trigger_index",       &(event->trigger_index));
  tree->Branch("trigger_index_offset",&(event->trigger_index_offset));
  tree->Branch("adc_gains",           &(event->adc_gains));
  tree->Branch("adc_offsets",         &(event->adc_offsets));
  tree->Branch("adc_ranges",          &(event->adc_ranges));
  tree->Branch("spe_means",           &(event->spe_means));
  tree->Branch("baseline_means",      &(event->baseline_means));
  tree->Branch("baseline_sigmas",     &(event->baseline_sigmas));
  //tree->Branch("baseline_validities", &(event->baseline_validities));
  tree->Branch("npulses",             &(event->npulses));
  tree->Branch("pulse_start_times",   &(event->pulse_start_times));
  tree->Branch("pulse_end_times",     &(event->pulse_end_times));
  tree->Branch("pulse_peak_times",    &(event->pulse_peak_times));
  tree->Branch("pulse_peak_amps",     &(event->pulse_peak_amps));
  tree->Branch("pulse_integrals",     &(event->pulse_integrals));

    tree->Branch("saturated",                               &(event->saturated));
    tree->Branch("ch_pulse_integrals",                      &(event->ch_pulse_integrals));
    tree->Branch("ch_5samp_extended_pulse_integrals",      &(event->ch_5samp_extended_pulse_integrals));
    tree->Branch("ch_10samp_extended_pulse_integrals",     &(event->ch_10samp_extended_pulse_integrals));

  
  
  // ------------------ INSTANTIATE ALL MODULES -------------------
  Converter converter(cfg);
  BaselineFinder baselineFinder(cfg);
  ZeroSuppressor zeroSuppressor(cfg);
  Integrator integrator(cfg);
  SumChannel sumChannel(cfg);
  PulseFinder pulseFinder(cfg);
  

  //---------------- INITIALIZE MODULES (AS NEEDED) ---------------

  
  // -------------------- LOOP OVER EVENTS ------------------------
  int min_evt = cfg.getParam<int>("tpcanalysis", "min", 1);
  int max_evt = cfg.getParam<int>("tpcanalysis", "max",
                                  DAQ_header.TotEventNbr);
  std::cout << "\nBeginning loop over events.\n" << std::endl;
  for (int evt=min_evt; evt<=max_evt; evt++) {
    if (evt%10000 == 0)
      std::cout << "Processing event " << evt << std::endl;
    event->Clear();
    event->run_id = 0;
    event->event_id = evt;
    
    // Run processing modules on event. ORDER MATTERS!
    converter.Process(event, DAQ_header); 
    baselineFinder.Process(event);
    zeroSuppressor.Process(event);
    sumChannel.Process(event);
    integrator.Process(event);
    pulseFinder.Process(event);


    
    tree->Fill();
    

  }// end loop over events

  //----------------- FINALIZE MODULES (AS NEEDED) ---------------


  // write TTree to file
  tree->Write();
  rootfile->Close();

  return 1;
  //return tree;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


int main(int args, char* argv[]) {

  if (args!=3) {
    std::cout << "Use correct command: ./tpcanalysis <cfg file> <rawdatafile>"
              << std::endl;
    return 1;
  }
  
  // initialize ROOT application
  TApplication *theApp = new TApplication("app", 0, 0);
  theApp->SetReturnFromRun(false);

  // initialize DAQheader
  DAQheader DAQ_header;
  if (DAQ_header.FormatTest()==false)
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;

  // open raw data file
  string datafile = argv[2];
  DAQ_header.LoadFileName(datafile.c_str());
  if (!DAQ_header.binary_file.is_open()) {
    std::cout << std::endl << "Can't open datafile: "
              << datafile.c_str() << std::endl;
    return 1;
  }
  DAQ_header.ReadHeaderContent();

  clock_t t = clock();
  
  ProcessEvents(DAQ_header, argv[1]);

  t = clock() - t;
  std::cout << "Processing time: "<<((float)t)/CLOCKS_PER_SEC<<" s." << std::endl;
  return 1;
}
