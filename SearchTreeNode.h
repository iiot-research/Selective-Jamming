/*
 *  SearchTreeNode.h
 *  SCHEDULER_APRIL_14
 *
 *   *
 */

#ifndef __SEARCHTREENODE
#define __SEARCHTREENODE

#include<map>

using namespace std;

class SearchTreeNode
	{
	public:
		int l;						// level of the node in the tree
		int k;						//optimal indicator
		map<int, int> f;			// priority  to flow
		
		SearchTreeNode()
		{		
			l=0; k=0; 
		}; 
		~SearchTreeNode()
		{
			f.clear();
		}; 	
	};
#endif