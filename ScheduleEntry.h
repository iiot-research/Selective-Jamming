/*
 *  ScheduleEntry.h
 *  
 * 
 *  
 *
 */

#ifndef __SCHEDULEENTRY
#define __SCHEDULEENTRY
#include "conio.h"
using namespace std;

class ScheduleEntry
{
public:
	int iFlowID;
	int iSender;
	int iReceiver;
	int iRouteID;
    int iHopCount;
	bool isShared;		// true if shared
    ACCESSTYPE iAccess;
    PHASETYPE iPhase;
    bool iSuccessfulTransmission;
	ScheduleEntry(){};
	ScheduleEntry(int flow, int sender, int receiver, bool  shared, ACCESSTYPE iacc,  PHASETYPE iphase, int routid,int HopCount){iFlowID=flow; iSender=sender; iReceiver=receiver; isShared=shared; iAccess=iacc; iPhase=iphase;iRouteID=routid; iHopCount=HopCount;};
	~ScheduleEntry(){};	
};

#endif