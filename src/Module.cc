#include "Module.hh"

Module::Module(const Setting & cfg)
  : tree(NULL)
{
  cfg.lookupValue("name", module_name);
  enabled = cfg.lookupValue("enabled", enabled) ? enabled : true;
  suppress_tree = cfg.lookupValue("suppress_tree", suppress_tree) ? suppress_tree : false;
}

Module::~Module()
{ }

void Module::Initialize()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Initialize().
  tree = new TTree(module_name.c_str(), module_name.c_str());
}


void Module::Process()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Process() as FINAL step;
  if (!suppress_tree) tree->Fill();
}


void Module::Finalize(TTree* master)
{
  // This function is to be overridden by each module.
  // Within override, call Module::Finalize().
  if (!suppress_tree) {
    if (tree->GetEntries() == master->GetEntries()) {
      master->AddFriend(tree);
      tree->Write();
    }
    else
      std::cout << "ERROR: tree for "<<module_name<<" has mis-matching number "
                << "of entries! This is a serious error."<<std::endl;
  }
  
}


TTree* Module::GetTree()
{
  return tree;
}
