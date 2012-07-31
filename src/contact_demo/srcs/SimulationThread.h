
//  SimulationThread.h
//  July 7, 2012
//  YL

#ifndef __SIMULATION_THREAD_H__
#define __SIMULATION_THREAD_H__

#include "wx/thread.h"
#include <dm.h>
#include <dmIntegEuler.hpp>


class SimulationThread : public wxThread {
public:	
	SimulationThread();
	~SimulationThread();
	virtual void *Entry();
	
	void unPause();
	void requestStop();
	
	volatile Float idt, cdt, last_control_time, sim_time;
	dmIntegEuler *G_integrator;
	
	
	volatile bool paused_flag;
	SpatialVector box_ext_f; // in box coordinate

	wxMutex mutexProtectSharedData;	
	wxMutex re_mutex;
	wxCondition * refreshCondition;
	
private:
	wxMutex mutex;
	wxCondition * unPauseCondition;
	Float Integ_count;


	
	volatile bool stopRequested;
	
};


#endif
