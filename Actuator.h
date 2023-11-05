/*
 *  Actuator.h
 *  ReliableScheduler
 *
 *  
 *
 */
#ifndef __ACTUATOR
#define __ACTUATOR
#include <vector>
#include <map>

using namespace std; 
class Actuator
{
	int ID;
public:	
	map<int, vector<int> > mvDownLinkGraph;	//downlink graph: key=node or router; vector[0]=primary down parent; vector[1]=secondary down parent
	Actuator(int id){	ID=id; mvDownLinkGraph.clear(); };
	int GetID(){return ID; };

	~Actuator(){};
};

#endif //__ACTUATOR

