#include "EventData.hh"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLine.h"
#include "TBox.h"
#include "TPad.h"
#include "TList.h"
#include "TGaxis.h"
#include "TMarker.h"
#include <complex> //abs
#include <algorithm> //min_element

double EventData::SampleToTime(int samp) const
{
  return (samp-trigger_index)*us_per_samp;
}

int EventData::TimeToSample(double time, bool checkrange) const
{
  int samp = (int)(time/us_per_samp)+trigger_index;
  if (samp > nsamps)
    samp = nsamps;
  else if (samp < 0)
    samp = 0;

  return samp;
}


TMultiGraph* EventData::GetTMultiGraph(int ch)
{

  int idx = channel_index[ch];
  char name[30];
  sprintf(name, "r%ie%ich%i", run_id, event_id, channel_id[idx]);
  TMultiGraph* mg = new TMultiGraph(name, name);
  
  if (raw_waveform[idx].empty())
    return 0;
  int nsamps = raw_waveform[idx].size();
  std::vector<double> x(nsamps);


  // set the x axis
  for(int i=0; i<nsamps; i++)
    x[i] = (i - trigger_index) * us_per_samp;


  std::vector<double> const& raw = raw_waveform[idx];
  TGraph* gr_raw = new TGraph(nsamps, &x[0], &raw[0]);
  gr_raw->SetTitle(name);
  gr_raw->SetName(name);
  mg->Add(gr_raw);

  mg->Draw("alp");
  mg->GetXaxis()->SetTitle("time [#mus]");
  mg->GetYaxis()->SetTitle("amp [arb]");

  
  //TODO allow this to be skipped if waveform isn't present]
  std::vector<double> const& baseline = baseline_subtracted_waveform[idx];
  TGraph* gr_baseline = new TGraph(nsamps, &x[0], &baseline[0]);
  gr_baseline->SetMarkerColor(kRed);
  gr_baseline->SetLineColor(kRed);
  mg->Add(gr_baseline);

  /*
  std::vector<double> const& integral_wfm = integral[idx];
  TGraph* gr_int = new TGraph(nsamps, &x[0], &integral_wfm[0]);
  gr_int->SetMarkerColor(kBlue);
  gr_int->SetLineColor(kBlue);
  mg->Add(gr_int);
  */
  // need to adjust size of integral so it fits
  std::vector<double> adjusted_integral(nsamps); 
  double integral_offset = 0; //(draw_baseline_subtracted ? 0 : bs_info.mean);
  double x1,x2,y1,y2;
  gPad->Update();
  gPad->GetRangeAxis(x1,y1,x2,y2);
  double raw_ratio = (y2 - integral_offset) / (integral_offset - y1);
  double integral_max = *std::max_element(integral[idx].begin(), integral[idx].end());
  double integral_min = *std::min_element(integral[idx].begin(), integral[idx].end());
  double integral_ratio = std::abs(integral_max) / std::abs(integral_min);
  double integral_scale;
  if (raw_ratio < integral_ratio)
    integral_scale = (y2 - integral_offset) / std::abs(integral_max) * 0.9;
  else
    integral_scale = (integral_offset - y1) / std::abs(integral_min) * 0.9;
  
  
  for(int i = 0; i<nsamps; i++)
    adjusted_integral[i] = integral_scale*integral[idx][i] + integral_offset;
  
  int integral_color = kBlue;
  
  TGraph* integral_gr = new TGraph(nsamps, &x[0], &adjusted_integral[0]);
  integral_gr->SetLineColor(integral_color);
  integral_gr->SetMarkerColor(integral_color);
  mg->Add(integral_gr);
  


  
  return mg;
}

