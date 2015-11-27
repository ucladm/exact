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

  v0.4 AFan 2015-11-27
  - Port framework to use base Module class
  - use libconfig for cfg files
  - move module execution to EventProcessor

*/


#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <unistd.h> //getopt
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

#include "TROOT.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

#include "EventProcessor.hh"

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

  // Output file
  TFile* rootfile = new TFile(outputfile.c_str(), "recreate");


  // Object that handles processing of all modules.
  EventProcessor processor(cfg);
  processor.Initialize();
  
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
  const Setting & tpcCfg = cfg.lookup("tpcanalysis");
  int min_evt, max_evt;
  tpcCfg.lookupValue("min", min_evt);
  tpcCfg.lookupValue("max", max_evt);
  if (max_evt == -1) max_evt = total_events;
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

    processor.SetDataFile(datafile);
    ++subfile;
  
    // -------------------- LOOP OVER EVENTS ------------------------

    for (int n=0; n<daq_header.ntriggers; ++n) {
      if (evt<min_evt) {
        evt++;
        continue;
      }
      if (evt>=max_evt)
        break;

      // If using event list, step the event loop forward until find matching event
      if (use_eventlist) {
        if (n != current_event) continue;
        else {
          cout << "Processing event " << n << endl;
          ifs >> current_event;
        }
      }
      if (!use_eventlist && evt%10000 == 0) cout << "Processing event " << evt << endl;

      processor.ProcessEvent(n);
      
      evt++;
      nevents++;

    }// end loop over events

  }// end loop over files

  rootfile->cd();
  
  processor.Finalize();
  
  rootfile->Close();

  return nevents;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


int main(int argc, char* argv[]) {


  std::string cfgfile;
  std::string datafile;
  std::string inputlist;
  std::string eventlist;
  bool use_eventlist = false;
  std::string outputfile = "tpcanalysis.root";

  int opt;
  while ((opt=getopt(argc, argv, "hc:i:I:o:e:")) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "Usage: -c <cfg file> -i <input file> [-o <output file>]" << std::endl;
      exit(EXIT_SUCCESS);
    case 'c':
      cfgfile = optarg;
      break;
    case 'i':
      datafile = optarg;
    case 'I':
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
      std::cout << "Usage: -c <cfg file> -i <input file> [-o <output file>]" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // initialize ROOT application
  TApplication *theApp = new TApplication("app", 0, 0);
  theApp->SetReturnFromRun(false);


  
  clock_t t = clock();
  
  int nevents = ProcessEvents(inputlist, cfgfile, outputfile, use_eventlist, eventlist);

  t = clock() - t;
  std::cout << "Processed "<<nevents<<" events in "<<((float)t)/CLOCKS_PER_SEC<<" s." << std::endl;
  
  return 1;
}
