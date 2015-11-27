/*
  2015-11-25 AFan

  Base class for all modules.

 */


#ifndef MODULE_HH
#define MODULE_HH

#include <iostream>
#include <libconfig.h++>
#include "TTree.h"
#include "TString.h"

using namespace libconfig;


class Module
{
public:
  Module(const Setting & cfg);
  ~Module();
  void Initialize();
  void Process();
  void Finalize(TTree* master);

  std::string module_name;
  bool enabled;

  TTree* GetTree();

protected:
  TTree* tree;
  
  
};


#endif
