#pragma once
#include "Node.h"
#include "assert.h"
#include "CustomSettings.h"
#include "Util.h"
#include "card_tools.h"
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

//-- - Uses the the CFR - D gadget to generate opponent ranges for re - solving.
//--
//--See[Solving Imperfect Information Games Using Decomposition](http://poker.cs.ualberta.ca/publications/aaai2014-cfrd.pdf)
class cfrd_gadget
{
public:
	cfrd_gadget(const ArrayX& board, const Range& player_range, const Range& opponent_cfvs);

	//-- - Uses one iteration of the gadget game to generate an opponent range for
	//	-- the current re-solving iteration.
	//	-- @param current_opponent_cfvs the vector of cfvs that the opponent receives
	//	-- with the current strategy in the re - solve game
	//	-- @param iteration the current iteration number of re-solving
	//	-- @return the opponent range vector for this iteration
	Range compute_opponent_range(const Range& current_opponent_cfvs);

private:
	const float regret_epsilon = 1.0f / 100000000;

	// Arbitrarily large number used for clamping regrets.
	const float max_number = 999999;

	CardArray _input_opponent_range;

	CardArray _input_opponent_value;

	CardArray _curent_opponent_values;

	CardArray _play_current_strategy;

	CardArray _terminate_current_strategy;

	CardArray _play_current_regret;
	CardArray _terminate_current_regret;

	CardArray _regret_sum;


	// Holds achieved CFVs for both players at each iteration so that we can compute regret
	CardArray _total_values;

	// Holds achieved CFVs for p2
	CardArray _total_values_p2;

	CardArray _terminate_regrets;
	CardArray _play_regrets;

	//masking out impossible hands
	CardArray _range_mask;

	card_tools _card_tools;
};

