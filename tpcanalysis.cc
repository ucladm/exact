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

int ProcessDataFile(string datafile, const Config & cfg, TFile* rootfile, int min_evt, int max_evt)
{
  // Instantiate DAQheader
  LVDAQHeader daq_header;
  if (!daq_header.format_test())
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;

  // Object that handles processing of all modules.
  EventProcessor processor(cfg);
  processor.Initialize();

  cout << "Opening data file: "<<datafile<<endl;
  processor.SetDataFile(datafile);

  if (max_evt == -1) max_evt = processor.GetDAQHeader().ntriggers;
  const int total_events = max_evt - min_evt;
  
  cout << "\nBeginning event loop.\n" << endl;

  // -------------------- LOOP OVER EVENTS ------------------------
  int nevents = 0;
  for (int n=0; n<total_events; ++n) {
    if (n<min_evt) continue;
    if (n>=max_evt) break;
    if (n%10000 == 0) cout << "Processing event " << n << "/"<<total_events<<endl;

    //////////////////////////
    processor.ProcessEvent(n);
    //////////////////////////
    
    nevents++;

  }// end loop over events
  rootfile->cd();
  
  processor.Finalize();

  processor.CloseDataFile();

  return nevents;
}

int ProcessFileList(string fileList, const Config & cfg, TFile* rootfile, int min_evt, int max_evt)
{

  // Instantiate DAQheader
  LVDAQHeader daq_header;
  if (!daq_header.format_test())
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;

  // Object that handles processing of all modules.
  EventProcessor processor(cfg);
  processor.Initialize();
  
  // -------------------- LOOP OVER FILES -------------------------
  // Do a quick loop through the files to find the total number of events.
  std::ifstream infile(fileList.c_str());
  std::string datafile;
  int total_events=0;
  while (infile >> datafile) {
    processor.SetDataFile(datafile);
    total_events+=processor.GetDAQHeader().ntriggers;
    processor.CloseDataFile();
  }
  infile.close();
  
  if (max_evt == -1) max_evt = total_events;
  int subfile = -1;
  int evt = 0;
  int nevents = 0;
  std::cout << "\nBeginning event loop.\n" << std::endl;

  // Now loop through files...for real this time.
  infile.open(fileList.c_str());
  while (infile >> datafile) {

    if (evt>max_evt)
      break;

    cout << "Opening data file: "<<datafile<<endl;
    processor.SetDataFile(datafile);
    ++subfile;
  
    // -------------------- LOOP OVER EVENTS ------------------------
    for (int n=0; n<processor.GetDAQHeader().ntriggers; ++n) {
      if (evt<min_evt) {
        evt++;
        continue;
      }
      if (evt>=max_evt)
        break;
      
      if (evt%10000 == 0) cout << "Processing event " << evt << "/"<<total_events<<endl;

      //////////////////////////
      processor.ProcessEvent(n);
      //////////////////////////
      
      evt++;
      nevents++;

    }// end loop over events

    processor.CloseDataFile();
    
  }// end loop over files

  rootfile->cd();
  
  processor.Finalize();

  return nevents;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


int main(int argc, char* argv[]) {


  if (argc<=1) {
    std::cout << "Usage: -c <cfg file> -i <input file> [-o <output file>] [-I <input LIST>]" << std::endl;
    return 1;
  }


  std::string cfgfile;
  bool process_datafile = false;
  std::string datafile;
  bool process_inputlist = false;
  std::string inputlist;
  //std::string eventlist;
  //bool use_eventlist = false;
  std::string outputfile = "tpcanalysis.root";

  int opt;
  while ((opt=getopt(argc, argv, "hc:i:I:o:e:")) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "Usage: -c <cfg file> -i <input file> [-o <output file>] [-I <input LIST>]" << std::endl;
      exit(EXIT_SUCCESS);
    case 'c':
      cfgfile = optarg;
      break;
    case 'i':
      process_datafile = true;
      datafile = optarg;
      break;
    case 'I':
      process_inputlist = true;
      inputlist = optarg;
      break;
    case 'o':
      outputfile = optarg;
      break;
    case 'e':
      //use_eventlist = true;
      //eventlist = optarg;
      break;
    default:
      std::cout << "Usage: -c <cfg file> -i <input file> [-o <output file>] [-I <input LIST>]" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  if (process_datafile && process_inputlist) {
    std::cout << "Use either -i or -I but not both."<<std::endl;
    exit(EXIT_FAILURE);
  }
  
  
  // initialize ROOT application
  TApplication *theApp = new TApplication("app", 0, 0);
  theApp->SetReturnFromRun(false);


  
  clock_t t = clock();


  // Output file
  TFile* rootfile = new TFile(outputfile.c_str(), "recreate");

  Config cfg;
  cfg.readFile(cfgfile.c_str());

  // Determine start and end of event loop.
  const Setting & tpcCfg = cfg.lookup("tpcanalysis");
  int min_evt, max_evt;
  tpcCfg.lookupValue("min", min_evt);
  tpcCfg.lookupValue("max", max_evt);
  
  int nevents = 0;
  if (process_inputlist) nevents = ProcessFileList(inputlist, cfg, rootfile, min_evt, max_evt);
  else if (process_datafile) nevents = ProcessDataFile(datafile, cfg, rootfile, min_evt, max_evt);

  rootfile->Close();

  t = clock() - t;
  std::cout << "Processed "<<nevents<<" events in "<<((float)t)/CLOCKS_PER_SEC<<" s." << std::endl;
  
  return 1;
}
