/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/



#ifndef __GSTLAPPLI_GEOSTAT_NODES_DROPPED_H__
#define __GSTLAPPLI_GEOSTAT_NODES_DROPPED_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>

#include <algorithm>
#include <vector>

using namespace std;

typedef pair<int, int> NodeType;

struct  node_drop_less_than 
{
	bool operator() ( NodeType p1,  NodeType p2) 
	{    
		return ( p1.second <= p2.second );
	}  
};

/*
 * class to recorded the number of nodes dropped during simulation
 * will return the nodes which are needed to be resimulated
 */
class GEOSTAT_DECL NodeDropped {
 public:
     NodeDropped( vector<int>& grid_path, int nb_cut=-1);

     //void NodeDropped( vector<int>& grid_path, int cutoff);

     void ResetPath( vector<int>& grid_path );
     void operator()(int loc, int nb_dropped);
     void GetRevisitNodes( vector<int>& grid_path_new);
     
     //void GetRevisitNodes( vector<int>& grid_path_new);

     void setDNProp( RGrid* grid, Grid_continuous_property* prop );

private:
    int nb_cut_;
    int path_size;
    vector< NodeType > nodes;
};


#endif