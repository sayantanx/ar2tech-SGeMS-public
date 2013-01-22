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




#include "NodeDropped.h"

// member functions
NodeDropped::NodeDropped( vector<int>& grid_path, int nb_cut )
{
    path_size = grid_path.size();

    for(int i=0; i<path_size; i++)
        nodes.push_back( make_pair( grid_path[i], 0 ) );

    nb_cut_ = nb_cut;
    if ( nb_cut_<0 )      nb_cut_ =99999;

}


void NodeDropped::ResetPath( vector<int>& grid_path )
{
    path_size = grid_path.size();
    nodes.clear();

    for(int i=0; i<path_size; i++)
        nodes.push_back( make_pair( grid_path[i], 0 ) );
}

void NodeDropped::operator()(int loc, int nb_dropped) 
{
    nodes[loc].second = nb_dropped;
}


void NodeDropped::GetRevisitNodes( vector<int>& grid_path_new)
{
    grid_path_new.clear();

    if ( nb_cut_ < 99999 )
    {
        for (int i=0; i<path_size; i++)
        {
            if ( nodes[i].second > nb_cut_ ) 
                grid_path_new.push_back( nodes[i].first );
            else if ( nodes[i].second < 0 ) 
                grid_path_new.push_back( nodes[i].first );
        }
    }
}



void NodeDropped::setDNProp( RGrid* grid, GsTLGridProperty* prop )
{
    SGrid_cursor  cursor = SGrid_cursor( *(grid->cursor()) );
    int node_id;

    for (int i=0; i<path_size; i++)
    {
        node_id = cursor.node_id( nodes[i].first );
        prop->set_value( nodes[i].second, node_id );
    }
}
