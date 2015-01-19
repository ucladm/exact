/*
  Event viewer code for data from UCLA EXACT TPCs. Can overlay analysis

  Two parts to code:
  1. Event by event processing, including baseline finding, pulse finding, etc
  2. Apply cuts and generate histograms and average waveforms
  
  Compile with Makefile
  
  v0.1 AFan 2013-04-21
    - base code adapted from tpcanalysis.cc
  v0.2 AFan 2013-08-17
    - TApplication works as expected now:
    - GUI is responsive, cmd line entry works, event stepping works
  v0.3 AFan 2014-01-21
    - Better event viewer. Uses multithreading.

 */


#include <iostream>
#include <string>
#include <fstream>



#include "TApplication.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSystem.h"
#include "TTimer.h"
#include "TRootCanvas.h"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include "TSystem.h"
#include "TGButton.h"


#include "DAQheader.hh"
#include "CfgReader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"
#include "ProcessedPlotter.hh"
#include "RootGraphix.hh"
#include "EventProcessor.hh"
#include "EventNavigator.hh"

using namespace std;

extern EventProcessor* gEventProcessor;

void make_gui()
{
  TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);
  

  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("XX GUI");
  frmMain->SetCleanup(kDeepCleanup);

  
  TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
  {
      
    TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
    TGPictureButton* b = 0;
    EventNavigator    *fh = new EventNavigator;

    b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
    hf->AddFrame(b);
    b->Connect("Clicked()", "EventNavigator", fh, "Bck()");

    b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
    hf->AddFrame(b);
    b->Connect("Clicked()", "EventNavigator", fh, "Fwd()");
  }
  frmMain->AddFrame(hf);
  
  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Event Control", 0);
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


int main(int args, char* argv[]) {

  if (args!=3) {
    std::cout << "Use correct command: ./eventviewer <cfg file> <rawdatafile>"
              << std::endl;
    return 1;
  }
  
  // initialize ROOT application
  TApplication *theApp = new TApplication("app", 0, 0);
  theApp->SetReturnFromRun(true);
  //theApp->ProcessFile(".rootstart.C");
  
  TEveManager::Create();

  make_gui();
  
  string cfgFile = argv[1];
  
  // Instantiate CfgReader
  CfgReader cfg;
  if (!cfg.readCfgFile(cfgFile)) {
    std::cout << "ERROR reading cfg file" << std::endl;
    return 0;
  }


  
  string datafile = argv[2];

  gEventProcessor = new EventProcessor(cfg, datafile);

  gEventProcessor->Initialize();
  
  gEventProcessor->ProcessEvent(1);
  
  theApp->Run();
  
  return 0;
}
