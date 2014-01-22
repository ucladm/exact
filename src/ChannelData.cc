#include "ChannelData.hh"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLine.h"
#include "TBox.h"
#include "TPad.h"
#include "TList.h"
#include "TGaxis.h"
#include "TMarker.h"
#include <vector>

TGraph* ChannelData::GetTGraph() const
{
  if (raw_waveform.empty())
    return 0;
  int nsamps = raw_waveform.size();
  std::vector<double> x(nsamps);
  std::vector<double> const& y(raw_waveform);

  // set the x axis
  for(int i=0; i<nsamps; i++)
    x[i] = (i - trigger_index) * us_per_samp;

    
  TGraph* graph = new TGraph(nsamps, &x[0], &y[0]);
  char name[30];
  sprintf(name, "e%ich%i", event_id, channel_id);
  graph->SetName(name);
  graph->SetTitle(name);
  graph->GetXaxis()->SetTitle("time [us]");
  graph->GetYaxis()->SetTitle("amp [counts]");
  return graph;
}

void ChannelData::Draw()
{
  TGraph* chgraph = GetTGraph();
  if (!chgraph)
    return;

  chgraph->Draw("alp");
}
