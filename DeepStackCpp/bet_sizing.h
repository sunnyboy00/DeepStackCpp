#pragma once
#include <Eigen/Dense>
#include "GameState.h"
#include "arguments.h"
#include <algorithm> 
#include <memory>

using Eigen::VectorXf;
using Eigen::ArrayX2f;
using namespace std;

class bet_sizing
{
public:
	//-- - Constructor
	//-- @param pot_fractions a list of fractions of the pot which are allowed
	//-- as bets, sorted in ascending order
	bet_sizing(VectorXf* pot_fractions);
	// Destructor
	~bet_sizing();
	//-- - Gives the bets which are legal at a game state.
	//-- @param node a representation of the current game state, with fields :
	//--
	//-- * `bets`: the number of chips currently committed by each player
	//--
	//-- * `current_player`: the currently acting player
	//-- @return an Nx2 tensor where N is the number of new possible game states,
	//--containing N sets of new commitment levels for each player
	unique_ptr<ArrayX2f> get_possible_bets(GameState& node);


private:
	VectorXf* _pot_fractions;
};

