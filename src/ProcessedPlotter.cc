#include "ProcessedPlotter.hh"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TList.h"
#include "TString.h"

#include "TEveManager.h"
#include "TEveBrowser.h"

#include <algorithm>


using namespace std;

int colors[] = {kBlack, kRed, kGreen, kCyan, kBlue, kMagenta, kYellow, kGray+2,
		kOrange-3, kGreen+3, kCyan+3, kMagenta-5, kRed-2};
int ncolors = sizeof(colors)/sizeof(int);


TCanvas* gCanvas = 0;



ProcessedPlotter::ProcessedPlotter(const Setting & cfg) : Module (cfg)
{
  cfg.lookupValue("draw_sum", _draw_sum);
}

void ProcessedPlotter::Initialize()
{
  // create an embedded canvas
  gEve->GetBrowser()->StartEmbedding(1);
  gCanvas = new TCanvas;
  gEve->GetBrowser()->StopEmbedding("EventViewer");

}


void ProcessedPlotter::Process(EventData* event)
{
  gCanvas->Clear();
  char title[30];
  sprintf(title, "Run %d - Event %d", event->run_id, event->event_id);
  gCanvas->SetTitle(title);

  vector<int> chansToDraw;
  for (int ch=0; ch<event->nchans; ++ch) {
    if (!skip_channel(ch)) chansToDraw.push_back(ch);
  }
  const int nChansToDraw = chansToDraw.size();
  const int nPadsToDraw = nChansToDraw + _draw_sum;

  int width = floor(sqrt(nPadsToDraw*1.77));
  int height = ceil(nPadsToDraw/floor(sqrt(nPadsToDraw*1.77)));
  gCanvas->Divide(width,height);


  for (int i=0; i<nChansToDraw; ++i) {
    const int ch = chansToDraw[i];
    gCanvas->cd( (nPadsToDraw == 1 ? 0 : i+1 ) );
    PlotChannel(event, ch);
    gPad->Modified();
  }//loop over channels to draw

  if (_draw_sum) {
    gCanvas->cd( nPadsToDraw );
    PlotSumChannel(event);
    gPad->Modified();
  }



  gCanvas->cd(0);
  gCanvas->Update();

}

//void ProcessedPlotter::Finalize(TTree* master)
//{}


void ProcessedPlotter::PlotChannel(EventData* event, int chID)
{
  ChannelData* channel = event->GetChannel(chID);
  
  TString name = TString::Format("r%ie%ich%i", event->run_id, event->event_id, chID);
  
  TMultiGraph* mg = new TMultiGraph(name, name);

  //-------------------------------
  // set the x axis
  const int nsamps = event->nsamps;
  vector<double> x(nsamps);
  for(int i=0; i<nsamps; i++)
    x[i] = (i - event->trigger_index) * event->us_per_samp;

  //-------------------------------
  // draw raw waveform
  vector<double> const& raw = channel->raw_waveform;
  TGraph* gr_raw = new TGraph(nsamps, &x[0], &raw[0]);
  gr_raw->SetTitle(name);
  gr_raw->SetName(name);
  //gr_raw->SetMarkerStyle(7);
  mg->Add(gr_raw);
    
  mg->Draw("alp");
  mg->GetXaxis()->SetTitle("time [#mus]");
  mg->GetYaxis()->SetTitle("amp [counts]");

  //-------------------------------
  // draw baseline
  vector<double> baseline = channel->baseline_subtracted_waveform;
  for (size_t i=0; i<baseline.size(); i++)
    baseline[i] = raw[i] - baseline[i];
  TGraph* gr_baseline = new TGraph(nsamps, &x[0], &baseline[0]);
  gr_baseline->SetMarkerColor(kRed);
  gr_baseline->SetLineColor(kRed);
  mg->Add(gr_baseline);

  //-------------------------------
  // draw integral
  // need to adjust vertical scale so it fits
  std::vector<double> adjusted_integral = DrawIntegral(channel);
  TGraph* integral_gr = new TGraph(nsamps, &x[0], &adjusted_integral[0]);
  int integral_color = kBlue;
  integral_gr->SetLineColor(integral_color);
  integral_gr->SetMarkerColor(integral_color);
  mg->Add(integral_gr);

  //double base = channel->baseline_mean;
  //DrawPulses(event, base);
  
}

void ProcessedPlotter::PlotSumChannel(EventData* event)
{
  ChannelData* channel(new ChannelData(event->sumchannel));
  
  TString name = TString::Format("r%ie%ichSUM", event->run_id, event->event_id);
  
  TMultiGraph* mg = new TMultiGraph(name, name);

  //-------------------------------
  // set the x axis
  const int nsamps = event->nsamps;
  vector<double> x(nsamps);
  for(int i=0; i<nsamps; i++)
    x[i] = (i - event->trigger_index) * event->us_per_samp;

  //-------------------------------
  // draw raw waveform
  vector<double> const& raw = channel->raw_waveform;
  TGraph* gr_raw = new TGraph(nsamps, &x[0], &raw[0]);
  gr_raw->SetTitle(name);
  gr_raw->SetName(name);
  //gr_raw->SetMarkerStyle(7);
  mg->Add(gr_raw);
    
  mg->Draw("alp");
  mg->GetXaxis()->SetTitle("time [#mus]");
  mg->GetYaxis()->SetTitle("amp [counts]");

  //-------------------------------
  // draw baseline
  vector<double> baseline = channel->raw_waveform;
  for (size_t i=0; i<baseline.size(); i++)
    baseline[i] = raw[i] - baseline[i];
  TGraph* gr_baseline = new TGraph(nsamps, &x[0], &baseline[0]);
  gr_baseline->SetMarkerColor(kRed);
  gr_baseline->SetLineColor(kRed);
  mg->Add(gr_baseline);

  //-------------------------------
  // draw integral
  // need to adjust vertical scale so it fits
  std::vector<double> adjusted_integral = DrawIntegral(channel);
  TGraph* integral_gr = new TGraph(nsamps, &x[0], &adjusted_integral[0]);
  int integral_color = kBlue;
  integral_gr->SetLineColor(integral_color);
  integral_gr->SetMarkerColor(integral_color);
  mg->Add(integral_gr);

  double base = 0;
  DrawPulses(event, base);
}




std::vector<double> ProcessedPlotter::DrawIntegral(ChannelData* channel)
{
  const int nsamps = channel->raw_waveform.size();
  vector<double> adjusted_integral(nsamps);
  const double baseline_mean = channel->baseline_mean; //(draw_baseline_subtracted ? 0 : bs_info.mean);
  double x1,x2,y1,y2;
  gPad->Update();
  gPad->GetRangeAxis(x1,y1,x2,y2);
  const double raw_ratio = (y2 - baseline_mean) / (baseline_mean - y1);
  const double integral_max = *std::max_element(channel->integral_waveform.begin(), channel->integral_waveform.end())+1.E-6;// slight offsets to avoid possible division by zero
  const double integral_min = *std::min_element(channel->integral_waveform.begin(), channel->integral_waveform.end())-1.E-6; 
  const double integral_ratio = std::abs(integral_max) / std::abs(integral_min);

  double integral_scale;
  if (raw_ratio < integral_ratio)
    integral_scale = (y2 - baseline_mean) / std::abs(integral_max) * 0.9;
  else
    integral_scale = (baseline_mean - y1) / std::abs(integral_min) * 0.9;
  
  const double integral_offset = baseline_mean;
  for(int i = 0; i<nsamps; i++)
    adjusted_integral[i] = integral_scale*channel->integral_waveform[i] + integral_offset;

  return adjusted_integral;

}

void ProcessedPlotter::DrawPulses(EventData* event, double base)
{
  for (int i=0; i<event->npulses; i++) {
    double peak_y = base + event->pulses[i].peak_amp;
    TBox* pbox = new TBox( event->pulses[i].start_time, base,
                           event->pulses[i].end_time, peak_y );
    pbox->SetBit(TObject::kCanDelete,true);
    pbox->SetLineColor(kGreen);
    pbox->SetFillStyle(0);
    pbox->Draw();
    //TLine* pline = new TLine( x[pulse_param.peak_index], base,
    //                          x[pulse_param.peak_index], peak_y);
    //pline->SetBit(TObject::kCanDelete,true);
    //pline->SetLineColor(kMagenta);
    //pline->Draw();
  }//end loop over pulses

}
