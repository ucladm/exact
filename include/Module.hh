/*
  2015-11-25 AFan

  Base class for all modules.

 */


#ifndef MODULE_HH
#define MODULE_HH

#include <iostream>
#include "TTree.h"
#include "TString.h"


class Module
{
public:
  Module(TString name);
  ~Module();
  void Process();
  void Initialize();
  void Finalize(TTree* master);

  TString module_name;

  TTree* GetTree();

protected:
  TTree* tree;
  
};


#endif
