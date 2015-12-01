#include "AverageWaveforms.hh"

#include "TH1I.h"
#include "TFile.h"
#include "TString.h"

AverageWaveforms::AverageWaveforms(const Setting & cfg)
  : Module(cfg)
  , _plots()
  , _nevents(new TH1I("avgwfm_nevents", "avgwfm_nevents", NCHANS, 0, NCHANS))
  
{
}


void AverageWaveforms::Initialize()
{
  Module::Initialize();
}

void AverageWaveforms::Process(EventData* event)
{
  int nsamps = event->nsamps;

  // Loop over channels
  for (int ch = 0; ch<event->nchans; ++ch) {

    vector<double> const& waveform = event->GetChannel(ch)->baseline_subtracted_waveform;


    // Initialize the TGraphErrors
    if (_nevents->GetBinContent(ch+1) == 0) {

      TString name = TString::Format("avgwfm_ch%d", ch);
      TH1F* h = new TH1F(name, name, nsamps, event->SampleToTime(0), event->SampleToTime(nsamps));
      _plots.push_back(h);
    }

    // Add this waveform to the average
    for (int i=0; i<nsamps; ++i) {
      float newval = _plots[ch]->GetBinContent(i+1) + waveform[i] / event->GetChannel(ch)->spe_mean;
      _plots[ch]->SetBinContent(i+1, newval);
    }
    
    _nevents->Fill(ch);
  }

  Module::Process();
}


void AverageWaveforms::Finalize(TTree* master)
{
  
  // format the waveform; save it to file.
  for (int ch=0; ch<(int)_plots.size(); ++ch) {

    if (skip_channel(ch)) continue;
    
    TH1F* hwfm = _plots[ch];

    // normalize the waveform
    for (int i=0; i<hwfm->GetNbinsX(); ++i)
      hwfm->SetBinContent(i+1, hwfm->GetBinContent(i+1)/_nevents->GetBinContent(ch+1));
    
    hwfm->GetXaxis()->SetTitle("time [#mus]");
    hwfm->GetYaxis()->SetTitle("amplitude [arb]");
    hwfm->Write();
  }
  _nevents->Write();


  Module::Finalize(master);
}
