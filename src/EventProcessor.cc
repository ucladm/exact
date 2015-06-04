#include "EventProcessor.hh"

using namespace std;





EventProcessor::EventProcessor(CfgReader const& cfg, string datafile)
  : daqHeader()
  , converter(cfg)
  , baselineFinder(cfg)
  , zeroSuppressor(cfg)
  , sumChannel(cfg)
  , integrator(cfg)
  , pulseFinder(cfg)
  , plotter(cfg)
{

  // Load the raw data file
  if (!daqHeader.format_test())
    std::cout << "ALARM! Variable size doesn't match!" << std::endl;

  // open raw data file
  //string datafile = argv[2];
  daqHeader.load_file(datafile.c_str());
  if (!daqHeader.binary_file.is_open()) {
    std::cout << std::endl << "Can't open datafile: "
              << datafile.c_str() << std::endl;
  }
  daqHeader.read_header_content();
  nevents = daqHeader.ntriggers;


  // Will repopulate this object for each event
  event = new EventData();  
}





void EventProcessor::Initialize()
{
  // Initialize modules as necessary
  plotter.Initialize();
}

void EventProcessor::ProcessEvent(int event_id)
{

  event->Clear();
  event->run_id = 0;
  event->event_id = event_id;

  std::cout << "Processing event "<< event->event_id << std::endl;
  
  // Run all the modules. ORDER MATTERS!
  converter.Process(event, daqHeader);
  baselineFinder.Process(event);
  zeroSuppressor.Process(event);
  sumChannel.Process(event);
  integrator.Process(event);
  pulseFinder.Process(event);
  plotter.Process(event);
}
