#include "AverageWaveforms.hh"

#include "TH1.h"
#include "TH1I.h"
#include "TFile.h"

AverageWaveforms::AverageWaveforms(CfgReader const& cfg):
  module_name("AverageWaveforms"),
  _enabled(cfg.getParam<bool>(module_name, "enabled", false))
  //  _output_filename(cfg.getParam<std::string>(module_name, "output_filename", "avgwfms.root"))
{ }


void AverageWaveforms::Initialize()
{ }

int AverageWaveforms::Process(EventData* event)
{
  if (!_enabled)
    return 0;

  if (_plots.size() == 0) {
    _plots.resize(event->nchans);
    _nevents.resize(event->nchans);
  }

  int nsamps = event->nsamps;

  // Loop over channels
  for (int ch = 0; ch<event->nchans; ++ch) {
    vector<double> const& waveform = event->GetChannel(ch)->baseline_subtracted_waveform;


    // Initialize the TGraphErrors
    if (_nevents[ch] == 0) {
      double* x = new double[nsamps];
      double* y = new double[nsamps];
      double* ex = new double[nsamps];
      double* ey = new double[nsamps];

      // Initialize the x axis
      for (int i=0; i<nsamps; ++i)
        x[i] = event->SampleToTime(i);

      _plots[ch] = new TGraphErrors(nsamps, x, y, ex, ey);
    }

    // Add this waveform to the average
    double* yaxis = _plots[ch]->GetY();
    for (int i=0; i<nsamps; ++i)
      yaxis[i] += waveform[i];

    
    _nevents[ch]++;
  }
  
  return 1;

}


void AverageWaveforms::Finalize(TFile* f)
{
  //TFile* outfile = new TFile("output.root", "RECREATE");
  // save the number of events in each channel in a histogram
  TH1I* h = new TH1I("avgwfms_nevents", "avgwfms_nevents; channel ID; # events in avg",
                     _nevents.size(), 0, _nevents.size());
  
  // format the waveform; save it to file.
  for (int ch=0; ch<(int)_plots.size(); ++ch) {
    TGraphErrors* gr = _plots[ch];

    // normalize the waveform
    double* y = gr->GetY();
    for (int i=0; i<gr->GetN(); ++i) {
      y[i] /= _nevents[ch];
    }

    // provide name, title, axis labels
    char name[25];
    sprintf(name, "average_ch%d", ch);
    gr->SetName(name);
    gr->SetTitle(name);
    gr->GetXaxis()->SetTitle("time [#mus]");
    gr->GetYaxis()->SetTitle("amplitude [arb]");
    gr->Write();
    
    h->SetBinContent(ch, _nevents[ch]);
  }
  h->SetStats(kFALSE);
  f->cd();
  h->Write();
  //  outfile->Close();

}
