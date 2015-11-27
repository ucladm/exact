
#ifndef EventProcessor_hh
#define EventProcessor_hh

//#include "Module.hh"
#include "LVDAQHeader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"
#include "ProcessedPlotter.hh"
//#include "RootGraphix.hh"

#include <string>
#include <libconfig.h++>

class EventProcessor
{
public:
  EventProcessor(const Config & cfg, std::string datafile);
  
  void Initialize();
  void ProcessEvent(int event_id);

  int nevents;
  
private:
  
  LVDAQHeader daqHeader;
  Converter converter;
  BaselineFinder baselineFinder;
  ZeroSuppressor zeroSuppressor;
  SumChannel sumChannel;
  Integrator integrator;
  PulseFinder pulseFinder;
  ProcessedPlotter plotter;

  EventData* event;

};

#endif
