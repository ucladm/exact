#include "EventNavigator.hh"
#include "TGNumberEntry.h"
#include <iostream>

int gEventID = 1;
EventProcessor* gEventProcessor = 0;
TGNumberEntryField* gEventNumberEntry = 0;

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
  gEventID  = gEventNumberEntry->GetIntNumber();
  if (gEventID < 1) {
    std::cout << "Event ID too low! Going to first event." << std::endl;
    gEventID = 1;
  }
  else if (gEventID > gEventProcessor->nevents) {
    std::cout << "Event ID beyond file! Going to last event." << std::endl;
    gEventID = gEventProcessor->nevents;
  }
  gEventProcessor->ProcessEvent(gEventID);
}

