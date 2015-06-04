
#ifndef EventProcessor_hh
#define EventProcessor_hh

#include "LVDAQHeader.hh"
#include "CfgReader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"
#include "ProcessedPlotter.hh"
#include "RootGraphix.hh"

#include <string>

class EventProcessor
{
public:
  EventProcessor(CfgReader const& cfg, std::string datafile);
  
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
