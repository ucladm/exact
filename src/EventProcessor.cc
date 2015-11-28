#include "EventProcessor.hh"
#include "TTree.h"

using namespace std;





EventProcessor::EventProcessor(const Config & cfg)
  : daqHeader()
  , converter(cfg.lookup("Converter"))
  , baselineFinder(cfg.lookup("BaselineFinder"))
  , zeroSuppressor(cfg.lookup("ZeroSuppressor"))
  , sumChannel(cfg.lookup("SumChannel"))
  , integrator(cfg.lookup("Integrator"))
  , pulseFinder(cfg.lookup("PulseFinder"))
  , plotter(cfg.lookup("Plotter"))
{
  // Will repopulate this object for each event
  event = new EventData();  
}


void EventProcessor::SetDataFile(string datafile)
{
  // Load the raw data file
  if (!daqHeader.format_test())
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;

  // open raw data file
  daqHeader.load_file(datafile.c_str());
  if (!daqHeader.binary_file.is_open()) {
    std::cout << std::endl << "Can't open datafile: "
              << datafile.c_str() << std::endl;
  }
  daqHeader.read_header_content();
}

void EventProcessor::CloseDataFile()
{
  daqHeader.close_file();
}


void EventProcessor::Initialize()
{
  // Initialize modules as necessary
  if (converter.enabled) converter.Initialize();
  if (baselineFinder.enabled) baselineFinder.Initialize();
  if (zeroSuppressor.enabled) zeroSuppressor.Initialize();
  if (sumChannel.enabled) sumChannel.Initialize();
  if (integrator.enabled) integrator.Initialize();
  if (pulseFinder.enabled) pulseFinder.Initialize();
  if (plotter.enabled) plotter.Initialize();
}

void EventProcessor::ProcessEvent(int event_id)
{

  event->Clear();
  event->run_id = 0;
  event->event_id = event_id;

  // Run all the modules. ORDER MATTERS!
  if (converter.enabled)      converter.Process(event, daqHeader);
  if (baselineFinder.enabled) baselineFinder.Process(event);
  if (zeroSuppressor.enabled) zeroSuppressor.Process(event);
  if (sumChannel.enabled)     sumChannel.Process(event);
  if (integrator.enabled)     integrator.Process(event);
  if (pulseFinder.enabled)    pulseFinder.Process(event);
  if (plotter.enabled) plotter.Process(event);
}


void EventProcessor::Finalize()
{
  TTree* master = converter.GetTree();
  if (baselineFinder.enabled) baselineFinder.Finalize(master);
  if (zeroSuppressor.enabled) zeroSuppressor.Finalize(master);
  if (sumChannel.enabled)     sumChannel.Finalize(master);
  if (integrator.enabled)     integrator.Finalize(master);
  if (pulseFinder.enabled)    pulseFinder.Finalize(master);
  master->Write();
}
