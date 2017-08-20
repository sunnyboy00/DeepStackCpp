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
class cfrd_gadget_f
{
public:
	cfrd_gadget_f(const ArrayX& board, const Range_f& player_range, const Range_f& opponent_cfvs);

	//-- - Uses one iteration of the gadget game to generate an opponent range for
	//	-- the current re-solving iteration.
	//	-- @param current_opponent_cfvs the vector of cfvs that the opponent receives
	//	-- with the current strategy in the re - solve game
	//	-- @param iteration the current iteration number of re-solving
	//	-- @return the opponent range vector for this iteration
	Range_f compute_opponent_range(const Range_f& current_opponent_cfvs);

private:
	const float regret_epsilon = 1.0f / 100000000;

	// Arbitrarily large number used for clamping regrets.
	const float max_number = 999999;

	Range_f _input_opponent_range;

	Range_f _input_opponent_value;

	Range_f _curent_opponent_values;

	Range_f _play_current_strategy;

	Range_f _terminate_current_strategy;

	Range_f _play_current_regret;
	Range_f _terminate_current_regret;

	Range_f _regret_sum;


	// Holds achieved CFVs for both players at each iteration so that we can compute regret
	Range_f _total_values;

	// Holds achieved CFVs for p2
	Range_f _total_values_p2;

	Range_f _terminate_regrets;
	Range_f _play_regrets;

	//masking out impossible hands
	Range_f _range_mask;

	card_tools _card_tools;
};

