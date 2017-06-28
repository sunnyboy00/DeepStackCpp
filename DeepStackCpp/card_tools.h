#pragma once
#include "game_settings.h"
#include "CustomSettings.h"
#include "constants.h"
#include <Eigen/Dense>


using Eigen::Matrix;
using Eigen::ArrayXi;

class card_tools
{
public:
	card_tools();

	// -- - Gives whether a set of cards is valid.
	// -- @param hand a vector of cards
	// -- @return `true` if the tensor contains valid cards and no card is repeated
	bool hand_is_possible(ArrayXi hand);

	//-- - Gives the private hands which are valid with a given board.
	//-- @param board a possibly empty vector of board cards
	//-- @return a vector with an entry for every possible hand(private card), which
	//--  is `1` if the hand shares no cards with the board and `0` otherwise
	ArrayXi get_possible_hand_indexes(ArrayXi board);

	//--- Gives the private hands which are invalid with a given board.
	//-- @param board a possibly empty vector of board cards
	//-- @return a vector with an entry for every possible hand(private card), which
	//-- is `1` if the hand shares at least one card with the board and `0` otherwise
	ArrayXi get_impossible_hand_indexes(ArrayXi board);
};

