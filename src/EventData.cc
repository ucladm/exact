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


TGraph* EventData::GetTGraph(int ch)
{
  int idx = channel_index[ch];
  if (raw_waveform[idx].empty())
    return 0;
  int nsamps = raw_waveform[idx].size();
  std::vector<double> x(nsamps);
  std::vector<double> const& y(raw_waveform[idx]);

  // set the x axis
  for(int i=0; i<nsamps; i++)
    x[i] = (i - trigger_index) * us_per_samp;

    
  TGraph* graph = new TGraph(nsamps, &x[0], &y[0]);
  char name[30];
  sprintf(name, "r%ie%ich%i", run_id, event_id, channel_id[idx]);
  graph->SetName(name);
  graph->SetTitle(name);
  graph->GetXaxis()->SetTitle("time [us]");
  graph->GetYaxis()->SetTitle("amp [counts]");
  return graph;
}

void EventData::DrawChannel(int ch)
{
  TGraph* chgraph = GetTGraph(ch);
  if (!chgraph)
    return;

  chgraph->Draw("alp");
}
