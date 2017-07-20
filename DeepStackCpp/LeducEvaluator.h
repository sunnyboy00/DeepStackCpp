#pragma once

#include "card_tools.h"
#include "card_to_string_conversion.h"
#include "assert.h"
#include <Eigen/Dense>
#include <algorithm>
#include "game_settings.h"

using namespace std;
using namespace Eigen;

class LeducEvaluator
{
public:
	static const int DEFAULT_IMPOSSIBLE_HAND_VALUE = -1;

	LeducEvaluator();

	//-- - Gives a strength representation for a hand containing two cards.
	//	-- @param hand_ranks the rank of each card in the hand
	//	-- @return the strength value of the hand.
	long long evaluate_two_card_hand(Array2f hand_ranks);

	//-- - Gives a strength representation for a hand containing three cards.
	//	-- @param hand_ranks the rank of each card in the hand
	//	-- @return the strength value of the hand.
	long long evaluate_three_card_hand(Array2f hand_ranks);

	//-- - Gives a strength representation for a two or three card hand.
	//	-- @param hand a vector of two or three cards
	//	-- @param[opt] impossible_hand_value the value to return if the hand is invalid
	//	-- @return the strength value of the hand, or `impossible_hand_value` if the
	//	-- hand is invalid. Warning: Looks like this is not about hand but about hand card + board card.
	ArrayXf evaluate(ArrayXf hand, ArrayXf impossible_hand_value);

	//-- - Gives strength representations for all private hands on the given board.
	//	-- @param board a possibly empty vector of board cards
	//	-- on the board
	//	-- @return a vector containing a strength value or `impossible_hand_value` for
	//	--every private hand.
	ArrayXf batch_eval(ArrayXf board, ArrayXf impossible_hand_value);

	ArrayXf batch_eval(ArrayXf board);

private:

	card_tools _card_tools;
	card_to_string_conversion _card_to_string;
};

