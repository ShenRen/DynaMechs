/*
 *  SimulationThread.h
 *  DynaMechs
 *
 *  Created by Patrick Wensing on 6/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SIMULATION_THREAD_H__
#define __SIMULATION_THREAD_H__

#include "wx/thread.h"
#include <dm.h>
#include <dmIntegEuler.hpp>
#include "HumanoidDataLogger.h"

class SimulationThread : public wxThread {
public:	
	SimulationThread();
	~SimulationThread();
	virtual void *Entry();
	
	void unPause();
	void lockRobot();
	void unlockRobot();
	void requestStop();
	
	volatile Float idt, cdt, last_control_time, sim_time;
	dmIntegEuler *G_integrator;
	
	HumanoidDataLogger * simRobot;
	
	volatile bool paused_flag;
	
private:
	wxMutex mutex;
	wxCondition * unPauseCondition;
	
	
	volatile bool stopRequested;
	
};


#endif