/*
  Analysis code for event reconstruction in UCLA EXACT TPCs.

  Two parts to code:
  1. Event by event processing, including baseline finding, pulse finding, etc
  2. Apply cuts and generate histograms and average waveforms
  
  Compile with Makefile
  
  v0.1 AFan 2013-04-21

  v0.2 AFan 2014-02-07
  - Multiple files implemented.
  - Use command line switches
  - Allow event lists

  v0.3 AFan 2015-01-10
  - Write outputs to better tree format

*/


#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <unistd.h> //getopt


#include "TROOT.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

#include "LVDAQHeader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"
//#include "ROI.hh"
//#include "AverageWaveforms.hh"
//#include "EventDataWriter.hh"

// new modules for revamp
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

int ProcessEvents(string fileList, string cfgFile, string outputfile,
                  bool use_eventlist, string eventlist)
{

  // Instantiate DAQheader
  LVDAQHeader daq_header;
  if (!daq_header.format_test())
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;
    
  Config cfg;
  cfg.readFile(cfgFile.c_str());

  ifstream ifs;
  if (use_eventlist)
    ifs.open(eventlist.c_str());


  TFile* rootfile = new TFile(outputfile.c_str(), "recreate");
  
  // Instantiate EventData; will repopulate this object for each
  // event. Create a branch for each variable we want to save.
  EventData* event = new EventData();
  

  
  
  // ------------------ INSTANTIATE ALL MODULES -------------------
  Converter converter(cfg.lookup("Converter"));
  BaselineFinder baselineFinder(cfg.lookup("BaselineFinder"));
  ZeroSuppressor zeroSuppressor(cfg.lookup("ZeroSuppressor"));
  SumChannel sumChannel(cfg.lookup("SumChannel"));
  Integrator integrator(cfg.lookup("Integrator"));
  PulseFinder pulseFinder(cfg.lookup("PulseFinder"));

  //---------------- INITIALIZE MODULES (AS NEEDED) ---------------
  //avgwfms.Initialize();
  //eventDataWriter.Initialize();

  if (converter.enabled) converter.Initialize();
  if (baselineFinder.enabled) baselineFinder.Initialize();
  if (zeroSuppressor.enabled) zeroSuppressor.Initialize();
  if (sumChannel.enabled) sumChannel.Initialize();
  if (integrator.enabled) integrator.Initialize();
  if (pulseFinder.enabled) pulseFinder.Initialize();
  
  // -------------------- LOOP OVER FILES -------------------------


  // Do a quick loop through the files to find the total number of events.
  std::ifstream infile(fileList.c_str());
  std::string datafile;
  int total_events=0;
  while (infile >> datafile) {
    daq_header.load_file(datafile.c_str());
    if (!daq_header.binary_file.is_open()) {
      std::cout << "\nCan't open datafile: "<< datafile.c_str() << std::endl;
      return 1;
    }
    daq_header.read_header_content();
    total_events+=daq_header.ntriggers;
    daq_header.binary_file.close();
  }
  infile.close();

  // Determine start and end of event loop.
  //int min_evt = cfg.getParam<int>("tpcanalysis", "min", 0);
  //int max_evt = cfg.getParam<int>("tpcanalysis", "max", total_events);
  int min_evt = 0;
  int max_evt = 100;
  int evt = 0;
  int subfile = -1;
  int nevents = 0;
  std::cout << "\nBeginning loop over events.\n" << std::endl;

  int current_event;
  ifs >> current_event;
  
  // Now loop through files...for real this time.
  infile.open(fileList.c_str());
  while (infile >> datafile) {

    if (evt>max_evt)
      break;

    // Open new data file. We've already looped through the files
    // once, so we know we can open them.
    daq_header.load_file(datafile.c_str());
    std::cout << "Opening file "<<datafile<<std::endl;
    ++subfile;
    daq_header.read_header_content();

  
    // -------------------- LOOP OVER EVENTS ------------------------

    for (int n=0; n<daq_header.ntriggers; ++n) {
      if (evt<min_evt) {
        evt++;
        continue;
      }
      if (evt>max_evt)
        break;

      // If using event list, step the event loop forward until find matching event
      if (use_eventlist) {
        if (n != current_event)
          continue;
        else {
          std::cout << "Processing event " << n << std::endl;
          ifs >> current_event;
        }
      }
                
      
      if (!use_eventlist && evt%10000 == 0)
        std::cout << "Processing event " << n << std::endl;
      event->Clear();
      //event->run_id = subfile;
      event->event_id = n; //evt;

      /////////////////////////////////////////////////////////////
      // Run processing modules on event. ORDER MATTERS!
      //converter.Process(event, daq_header);
      //baselineFinder.Process(event);
      //zeroSuppressor.Process(event);
      //sumChannel.Process(event);
      //integrator.Process(event);
      //pulseFinder.Process(event);
      //roi.Process(event);
      //avgwfms.Process(event);
      //eventDataWriter.Process(event);

      if (converter.enabled)      converter.Process(event, daq_header);
      if (baselineFinder.enabled) baselineFinder.Process(event);
      if (zeroSuppressor.enabled) zeroSuppressor.Process(event);
      if (sumChannel.enabled)     sumChannel.Process(event);
      if (integrator.enabled)     integrator.Process(event);
      if (pulseFinder.enabled)    pulseFinder.Process(event);
      
      /////////////////////////////////////////////////////////////

      evt++;
      nevents++;

    }// end loop over events

  }// end loop over files
  
  //----------------- FINALIZE MODULES (AS NEEDED) ---------------
  TTree* master = converter.GetTree();
  if (baselineFinder.enabled) baselineFinder.Finalize(master);
  if (zeroSuppressor.enabled) zeroSuppressor.Finalize(master);
  if (sumChannel.enabled)     sumChannel.Finalize(master);
  if (integrator.enabled)     integrator.Finalize(master);
  if (pulseFinder.enabled)    pulseFinder.Finalize(master);
  
  master->Write();
  
  rootfile->Close();

  return nevents;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


int main(int argc, char* argv[]) {


  std::string cfgfile;
  std::string inputlist;
  std::string eventlist;
  bool use_eventlist = false;
  std::string outputfile = "output.root";

  int opt;
  while ((opt=getopt(argc, argv, "hc:i:o:e:")) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "Usage: [-c cfgfile] [-i inputlist] [-o outputfile]" << std::endl;
      exit(EXIT_SUCCESS);
    case 'c':
      cfgfile = optarg;
      break;
    case 'i':
      inputlist = optarg;
      break;
    case 'o':
      outputfile = optarg;
      break;
    case 'e':
      use_eventlist = true;
      eventlist = optarg;
      break;
    default:
      std::cout << "Usage: [-c cfgfile] [-i inputlist] [-o outputfile]" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // initialize ROOT application
  TApplication *theApp = new TApplication("app", 0, 0);
  theApp->SetReturnFromRun(false);


  
  clock_t t = clock();
  
  int nevents = ProcessEvents(inputlist, cfgfile, outputfile,
                              use_eventlist, eventlist);

  t = clock() - t;
  std::cout << "Processed "<<nevents<<" events in "<<((float)t)/CLOCKS_PER_SEC<<" s." << std::endl;
  
  return 1;
}
