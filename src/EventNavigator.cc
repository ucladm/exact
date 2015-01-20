#include "EventNavigator.hh"
#include "TGNumberEntry.h"
#include <iostream>

int gEventID = 1;
EventProcessor* gEventProcessor = 0;
TGNumberEntry* gEventNumberEntry = 0;

void EventNavigator::Fwd()
{
  if (gEventID >= gEventProcessor->nevents)
    std::cout << "At end of file" << std::endl;
  
  else {
    ++gEventID;
    gEventProcessor->ProcessEvent(gEventID);
  }

}

void EventNavigator::Bck()
{
  if (gEventID <= 1)
    std::cout << "At beginning of file" << std::endl;
  else {
    --gEventID;
    gEventProcessor->ProcessEvent(gEventID);
  }
}

void EventNavigator::GotoEvent() {
  gEventID  = gEventNumberEntry->GetNumber();
  if (gEventID <= 1) {
    std::cout << "Event ID too low!" << std::endl;
    gEventID = 1;
  }
  else if (gEventID >= gEventProcessor->nevents) {
    std::cout << "Event ID beyond file!" << std::endl;
    gEventID = gEventProcessor->nevents;
  }
  gEventProcessor->ProcessEvent(gEventID);
}

