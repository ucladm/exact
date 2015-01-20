#include "ProcessedPlotter.hh"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TList.h"

#include "TEveManager.h"
#include "TEveBrowser.h"


int colors[] = {kBlack, kRed, kGreen, kCyan, kBlue, kMagenta, kYellow, kGray+2,
		kOrange-3, kGreen+3, kCyan+3, kMagenta-5, kRed-2};
int ncolors = sizeof(colors)/sizeof(int);


TCanvas* gCanvas = 0;



ProcessedPlotter::ProcessedPlotter(CfgReader const& cfg):
  module_name("ProcessedPlotter"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", 0)),
  _chans_per_pad(cfg.getParam<int>(module_name, "chans_per_pad", 1))
{ }

void ProcessedPlotter::Initialize()
{
  // create an embedded canvas
  gEve->GetBrowser()->StartEmbedding(1);
  gCanvas = new TCanvas;
  gEve->GetBrowser()->StopEmbedding("EventViewer");
  
}


int ProcessedPlotter::Process(EventData* event)
{
  if (!_enabled)
    return 1;
  gCanvas->Clear();
  char title[30];
  sprintf(title, "Run %d - Event %d", event->run_id, event->event_id);
  gCanvas->SetTitle(title);

  int nchans = event->nchans;

  int cpp = _chans_per_pad;
  if(cpp < 1)
    cpp = (nchans > 0 ? nchans : 1);
  int total_pads = (nchans+cpp-1)/cpp+1; //extra pad for sum channel
      
  if(total_pads == 0)
    return 0;
  else if(total_pads == 1) {}
  else if(total_pads == 2)
    gCanvas->Divide(2,1);
  else if(total_pads < 5)
    gCanvas->Divide(2,2);
  else if(total_pads < 7)
    gCanvas->Divide(3,2);
  else if(total_pads < 10)
    gCanvas->Divide(3,3);
  else if(total_pads < 13)
    gCanvas->Divide(4,3);
  else if(total_pads < 17)
    gCanvas->Divide(4,4);
  else
    gCanvas->Divide(5,4);


  //TODO clean up: remove chans_per_pad
  for (int pad=0; pad<total_pads; pad++) {
    gCanvas->cd( (total_pads == 1 ? 0 : pad+1 ) );
    if( cpp == 1 || nchans == 1 ) {
      if (pad != total_pads-1) {
        event->GetTMultiGraph(event->GetChannel(pad)->channel_id);
        gPad->Modified();
      }
      else {
        event->GetTMultiGraph_sumch();
        gPad->Modified();
      }
    }
      
    
  }

  gCanvas->cd(0);
  gCanvas->Update();

  return 1;
}
