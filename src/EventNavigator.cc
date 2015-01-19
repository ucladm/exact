#include "EventNavigator.hh"
#include <iostream>

int gEventID = 1;
EventProcessor* gEventProcessor = 0;

extern EventProcessor* gEventProcessor;

void EventNavigator::Fwd()
{
  if (gEventID >= gEventProcessor->nevents)
    std::cout << "At end of file" << std::endl;
  else 
    ++gEventID;
  gEventProcessor->ProcessEvent(gEventID);
}

void EventNavigator::Bck()
{
  if (gEventID <= 1)
    std::cout << "At beginning of file" << std::endl;
  else
    --gEventID;
  gEventProcessor->ProcessEvent(gEventID);
}

