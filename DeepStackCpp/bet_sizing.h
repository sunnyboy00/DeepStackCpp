#pragma once
#include <Eigen/Dense>
#include "Node.h"
#include "arguments.h"
#include "game_settings.h"
#include "CustomSettings.h"
#include "Windows.h"
#include <algorithm> 
#include <memory>

using namespace std;

class bet_sizing_manager
{
public:
	bet_sizing_manager();

	// Constructor
	// @param pot_fractions a list of fractions of the pot which are allowed
	// as bets, sorted in ascending order
	bet_sizing_manager(const Tf1& pot_fractions);

	// Gives the bets which are legal at a game state.
	// @param node a representation of the current game state, with fields :
	//
	// * `bets`: the number of chips currently committed by each player
	//
	// * `current_player`: the currently acting player
	// @return an Nx2 tensor where N is the number of new possible game states,
	// containing N sets of new commitment levels for each player.
	TX2f get_possible_bets(const Node& node);

	// Sets new _pot_fractions value
	void SetPotFraction(const Tf1& potFractions);


private:
	Tf1 _pot_fractions;
};

