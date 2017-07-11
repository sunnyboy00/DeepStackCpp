#pragma once
#include <string>

using namespace std;

struct GraphvizConnection
{
	// The graphical node the edge comes from
	string Id_from;
	
	// The graphical node the edge comes to
	string Id_to;

	// The graphical node connection id
	long long Id;
};