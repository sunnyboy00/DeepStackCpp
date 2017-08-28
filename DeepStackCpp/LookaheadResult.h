#pragma once
#include "CustomSettings.h"

#include <unsupported/Eigen/CXX11/Tensor>


// The result of a lookahead
struct LookaheadResult
{
	//-- An AxK tensor containing the re - solve player's strategy at the
	//-- root of the lookahead, where A is the number of actions and K is the range size
	ArrayXX strategy;

	// A vector of the opponent's average counterfactual values at the 
	// root of the lookahead
	ArrayX achieved_cfvs;

	// A vector of our average counterfactual values at the 
	// root of the lookahead
	ArrayX root_cfvs;

	// A vector of average counterfactual values at the 
	// root of the lookahead for both players
	ArrayXX root_cfvs_both_players;

	//-- An AxK tensor of opponent average counterfactual values after
	//-- each action that the re - solve player can take at the root of the lookahead
	ArrayXX children_cfvs;
};