#include "RootGraphix.hh"
#include "TApplication.h"
#include "TSystem.h"
#include "TVirtualMutex.h"
#include "TCanvas.h"
#include "TCanvasImp.h"
#include "TRootCanvas.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TGTableLayout.h"
#include "TRootEmbeddedCanvas.h"

#include <iostream>
#include <cmath>

const UInt_t fKeepRunning = 0x100000;

void* RunRootGraphix(void* mutexptr)
{
  TMutex* mutex = (TMutex*)(mutexptr);
  while(mutex->TestBit(fKeepRunning)){
    gSystem->Sleep(100);
    TLockGuard lock(mutex);
    if(gSystem->ProcessEvents())
      break;
  }
  return 0;
}

RootGraphix::RootGraphix():
  _mutex(),
  _thread(RunRootGraphix)
{
  _mutex.SetBit(fKeepRunning, true);
  
}

void RootGraphix::Initialize()
{
  _thread.Run(&_mutex);
  return;
}

void RootGraphix::Finalize()
{
  if(_thread.GetState() == TThread::kRunningState){
    _mutex.SetBit(fKeepRunning,false);
    _thread.Join();
  }
  if(_mainframe){
    _canvases.clear();
    _mainframe->Cleanup();
    delete _mainframe;
    _mainframe = 0;
  }

  for(size_t i=0; i < _canvases.size(); i++)
    delete _canvases[i];
  _canvases.clear();
  
  return;
}

RootGraphix::Lock RootGraphix::AcquireLock()
{
  return std::auto_ptr<TLockGuard>(new TLockGuard(&_mutex));
}

TCanvas* RootGraphix::GetCanvas(const char* title, bool preventclose, 
				bool hidemenu)
{
  
  int n = _canvases.size();
  char name[100];
  sprintf(name, "canvas%d",n);
  TCanvas* canvas;
  Lock glock = AcquireLock();
  if(!_single_window){
    UInt_t wx = gClient->GetDisplayWidth();
    UInt_t wy = gClient->GetDisplayHeight();
    int topx = wx/2 * (n % 2) + 10*(n/4);
    int topy = wy/2 * (int)(n%4 > 1) + 10*(n/4);
    
    canvas = new TCanvas(name, (title == 0 ? name : title),
			 topx, topy, (int)(wx*0.49), (int)(wy*0.49));
    TRootCanvas* imp = (TRootCanvas*)(canvas->GetCanvasImp());
    if(imp && preventclose)
      imp->DontCallClose();
    if(imp && hidemenu)
      imp->ShowMenuBar(false);
  }
  else{
    if(!_mainframe){
      _mainframe = new TGMainFrame;
      _mainframe->DontCallClose();
      _mainframe->SetName("RootGraphixMainFrame");
      _mainframe->SetWindowName("daqman analysis display");
    }
    int nframes = n+1;
    int nx=1, ny=1;
    while(nx*ny<nframes){
      if(nx > ny)
	++ny;
      else
	++nx;
    }
    TGTableLayout* ml = new TGTableLayout(_mainframe,ny,nx,true);
    _mainframe->SetLayoutManager(ml);
    const int hints = kLHintsExpandX | kLHintsExpandY | 
      kLHintsFillX | kLHintsFillY |
      kLHintsShrinkX | kLHintsShrinkY | 
      kLHintsCenterX | kLHintsCenterY;
    
    //rearrange all of the existing frames
    int row=1, col=1;
    TList* subframes = _mainframe->GetList();
    if(subframes){
      TGFrameElement* el=0;
      TIter next(subframes);
      while( (el=(TGFrameElement*)next()) ){
	if(el->fLayout)
	  delete el->fLayout;
	el->fLayout = new TGTableLayoutHints(col-1,col,row-1,row,hints);
	if(col < nx)
	   ++col;
	else{
	  col=1;
	  ++row;
	}
      }
    }
    
    TRootEmbeddedCanvas* ec = new TRootEmbeddedCanvas(name,_mainframe);
    _mainframe->AddFrame(ec, new TGTableLayoutHints(col-1,col,row-1,row,hints));
    _mainframe->MapSubwindows();
    _mainframe->MapWindow();
    _mainframe->Resize();
    _mainframe->Resize(_window_w,_window_h);

    canvas = ec->GetCanvas();
  }
  _canvases.push_back(canvas);
  return canvas;
}
