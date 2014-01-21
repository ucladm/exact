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
#include "testAnalysis.hh"
#include "ChannelData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "Integrator.hh"

#include <string>

using namespace std;


int ProcessEvents(DAQheader& DAQ_header, string cfgFile )
{

    
  // Instantiate CfgReader
  CfgReader cfg;
  if (!cfg.readCfgFile(cfgFile)) {
    std::cout << "ERROR reading cfg file" << std::endl;
    return 0;
  }

  // Create TTree to hold all processd info and open a file
  // to hold the TTree.
  TTree* tree = new TTree("Events", "Event Data");
  TFile* rootfile = new TFile("output.root", "RECREATE");


  // Instantiate EventData; will repopulate this object for each
  // event. Create a branch to hold all event data.
  EventData* event = new EventData();
  tree->Branch("EventData", "EventData", &event);


  // --------- INSTANTIATE AND INITIALIZE ALL MODULES ------------
  Converter converter;
  BaselineFinder baselineFinder("BaselineFinder");
  Integrator integrator;
  
  converter.Initialize(cfg);
  baselineFinder.Initialize(cfg);
  integrator.Initialize(cfg);
  
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
    
    // Run processing modules on event
    converter.Process(event, DAQ_header);
    baselineFinder.Process(event);
    integrator.Process(event);
    
    tree->Fill();
    

  }// end loop over events

  //----------------- FINALIZE MODULES (AS NEEDED) ---------------


  // write TTree to file
  rootfile->Write();

  return 1;
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

  
  ProcessEvents(DAQ_header, argv[1]);


  double b = 2.;
  cout << b << " ";
  analysis::testFunc(b);


  
  
  /*
  TCanvas* c1 = new TCanvas("c1","c1");
  myTH1F->Draw();

  while (true) {
    
    theApp->Run(kTRUE);
    theApp->Delete();
    char option;
    cout << "enter q" << endl;
    cin >> option;
    
    break;
    
  }
  //theApp->Run();
  */

  
  
  return 1;
}
