/*
 *  Flow.h
 *  ReliableScheduler
 *
 *  
 *
 */

#ifndef __FLOW
#define __FLOW

#include <iostream>
#include "ScheduleEntry.h"
#include "conio.h"
using namespace std;


class Flow
{
    int iMasterID;                  //id of the master flow
	int iID;					    //task id
	int iSource;                    // Source node
    int iACKSource;
	int iDestination;               // Destination node
	int lPeriod;					// period
	int lDeadLine;					//relative deadline
	int Li;							//worst case computation  when this is the hghes priority flow
	int iMaxExec;					//longest execution time=total transmissions
	int iAnalyticalEndToEndDelay;	//worst case response time  analyzed by test	
	int iSimulationEndToEndDelay;	//worst case response time  analyzed by test
    int iSIMSensingDelay;
    PHASETYPE iPhase;
    bool iEmergency;
    int iPendingSender;
    int iNumPackets;
    int iDeliveredPackets;
    int iReleaseTime;
    STATUSTYPE iStatus;
    bool Delivered;
    float iDeliverRatio;
    CRITTYPE iCrit;
    int Delay_EDF_RTAS;
    int Delay_EDF_Basic;
    int Delay_EDF_Precise;
    int Delay_EDF_Master;
    int Delay_FP_Poly;
    int Delay_FP_TMC;
    int Delay_FP_RTAS;
    
	
public:
	Flow();
	Flow(int ID, int src, int dest, int period, int deadline, bool Emergency, CRITTYPE crit)
	{
		iID= ID; 
		iSource = src; 
		iDestination= dest;
		lPeriod= period; 
		lDeadLine= deadline;
		iMaxExec=0;
        iEmergency=Emergency;
        iCrit=crit;
		Li=0; iAnalyticalEndToEndDelay=0; iSimulationEndToEndDelay=0; 
	};
    bool AtAP;
    
    map<int, vector<int> > mpSourceRoute;
    map<int, map<int, vector<int> > > mpUplinkRoute;           //[routeid][hop count][sender, receiver]
    map<int, map<int, vector<int> > > mpDownlinkRoute;           //[routeid][hop count][sender, receiver]
    map<int, map<int, vector<ScheduleEntry> > > mpSchedule;
    map<int, map<int, vector<int> > > mpLinkTrans;
    vector<int> vDelivery;
    vector<int> vDelay;
    
	
    //map<int, vector<int> > mpUplinkGraph;           //[routeid][node on path]
    //map<int, vector<int> > mpDownlinkGraph;         //[routeid][node on path]
    
    map<int, map<int, vector<int> > > mpACKRoute;                //[routeid][hop count][sender, receiver]
    map<int, map<int, vector<int> > > mpREVACKRoute;             //[routeid][hop count][sender, receiver]
    map<int, map<int, int> > mpCodesignUplinkRoute;     //[emergency sensor id][sender][receiver] Route to send from emergency sensor to regular sensor;
    map<int, map<int, int> > mpCodesignDownlinkRoute;   //[emergency actuator id][sender][receiver] Route to send from regular actuator actuator to emergency sensor;
	map<int, int> mDedicatedLinksUpDown;			//from->to
	map<int, map<int, int> > mSharedLinksUpDown;	//[from][to]=total passing transmissions through this link
	map<int, map<int, int> >mAllLinksUpDown;		//all links in graph route from->to; will be used to find small delta; count weight also
	
    map<int, map<int, ScheduleEntry> > mpTransmissions;
	vector<vector<int> > vvUpPaths;					// all paths in Uplink
	vector<vector<int> > vvDownPaths;					// all paths in Uplink
	map<int, bool> mNodes;
	
	int GetMasterID(){return iMasterID;};
    void SetMasterID(int mid){iMasterID=mid;};
	int GetSource(){return iSource; };
	int GetDestination(){return iDestination; };
	int GetPeriod(){return lPeriod; };
	int GetDeadLine(){return lDeadLine; };
    int GetHops(){return mpSourceRoute.size();};
	int GetMaxEcec(){return iMaxExec; };	
	void IncMaxExec(){iMaxExec++; };
	void SetAnalyticalEndToEndDelay(int x) { iAnalyticalEndToEndDelay=x;   }; 
	int GetAnalyticalEndToEndDelay() { return iAnalyticalEndToEndDelay;   }; 	
	void SetSimulationEndToEndDelay(int x) { iSimulationEndToEndDelay=x;   }; 
	int GetSimulationEndToEndDelay() { return iSimulationEndToEndDelay;   }; 
	int GetLi() {return Li; };
	void SetLi(int L){ Li=L; };
    bool GetEmergency(){return iEmergency;};
    void SetEmergency(bool iemer){iEmergency=iemer;};
    int GetSIMSensingDelay(){return iSIMSensingDelay;};
    void SetSIMSensingDelay(int delay){iSIMSensingDelay=delay;};
    PHASETYPE GetPhase(){return iPhase;};
    void SetPhase(PHASETYPE phase){iPhase=phase;};
    int GetPendingSender(){return iPendingSender;};
    void SetPendingSender(int sender){iPendingSender=sender;};
    STATUSTYPE GetStatus(){return iStatus;};
    void SetStatus(STATUSTYPE status){iStatus=status;};
    int GetNumPackets(){return iNumPackets;};
    void SetNumPackets(int numpackets){iNumPackets=numpackets;};
    int GetDeliveredPackets(){return iDeliveredPackets;};
    void SetDeliveredPackets(int numpackets){iDeliveredPackets=numpackets;};
	int GetReleaseTime(){return iReleaseTime;};
    void SetReleaseTime(int release){iReleaseTime=release;};
    int GetACKSource(){return iACKSource;};
    void SetACKSource(int acksource){iACKSource=acksource;};
    bool GetWhetherDelivered(){return Delivered;};
    void SetWhetherDelivered(bool deliver){Delivered=deliver;};
    CRITTYPE GetCrit(){return iCrit;};
    void SetCrit(CRITTYPE crit){iCrit=crit;};
    void SetAnaDelay_EDF_Basic(int delay){Delay_EDF_Basic=delay;};
    void SetAnaDelay_EDF_Precise(int delay){Delay_EDF_Precise=delay;};
    int GetAnaDelay_EDF_Basic(){return Delay_EDF_Basic;};
    int GetAnaDelay_EDF_Precise(){return Delay_EDF_Precise;};
    void SetAnaDelay_EDF_Master(int delay){Delay_EDF_Master=delay;};
    int GetAnaDelay_EDF_Master(){return Delay_EDF_Master;};
    void SetAnaDelay_EDF_RTAS(int delay){Delay_EDF_RTAS=delay;};
    int GetAnaDelay_EDF_RTAS(){return Delay_EDF_RTAS;};
    void SetAnaDelay_FP_Poly(int delay){Delay_FP_Poly=delay;};
    int GetAnaDelay_FP_Poly(){return Delay_FP_Poly;};
    void SetAnaDelay_FP_TMC(int delay){Delay_FP_TMC=delay;};
    int GetAnaDelay_FP_TMC(){return Delay_FP_TMC;};
    void SetAnaDelay_FP_RTAS(int delay){Delay_FP_RTAS=delay;};
    int GetAnaDelay_FP_RTAS(){return Delay_FP_RTAS;};
    void SetDeliverRatio(float ratio){iDeliverRatio=ratio;};
    float GetDeliverRatio(){return iDeliverRatio;};
    
	~Flow();	
};
#endif