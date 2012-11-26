
//  globalVariables.h
//  Nov 12, 2012
//  Project: showRobot
//  YL


#ifndef __GLOBAl_VARIABLES__
#define __GLOBAL_VARIABLES__

//This is a dirty workaround
#undef Success

#include <dm.h>
#include <dmArticulation.hpp>

#include "SimulationThread.h"
#include "MainFrame.h"
//#include "OTDataLogger.h"


//#define EIGEN_NO_DEBUG





extern dmArticulation *G_robot;
extern MainFrame *frame;

extern SimulationThread * simThread;
//extern OTDataLogger * logger;

extern wxMutex dataMutex;




#endif
