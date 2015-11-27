#ifndef LinkDef_h
#define LinkDef_h

/*
  based mostly on genroot.

  
  This file contains directives for all classes/functions that need to have
  a dictionary generated for ROOT. This is necessary to store
  instances of classes in a tree.

  Note that any class which will be be stored in an stl container needs to have
  that container specifically defined as well.

 */

#include <vector>

using namespace std;

#ifdef __MAKECINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class PulseData+;
#pragma link C++ class ChannelData+;
#pragma link C++ class EventData+;
//#pragma link C++ class EventNavigator+;
#endif


#endif
