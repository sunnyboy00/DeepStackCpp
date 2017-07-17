#pragma once
#include <Eigen/Dense>
#include "assert.h"
#include "LeducEvaluator.h"
#include "game_settings.h"

using Eigen::ArrayXf;
using namespace std;

//-- - Evaluates player equities at terminal nodes of the game's public tree.

class terminal_equity
{
public:
	terminal_equity();

	//-- - Zeroes entries in an equity matrix that correspond to invalid hands.
	//--
	//--A hand is invalid if it shares any cards with the board.
	//--
	//-- @param equity_matrix the matrix to modify
	//-- @param board a possibly empty vector of board cards
	void _handle_blocking_cards(ArrayXXf& equity_matrix, const ArrayXf& board);

	//-- - Constructs the matrix that turns player ranges into showdown equity.
	//	--
	//	--Gives the matrix `A` such that for player ranges `x` and `y`, `x'Ay` is the equity
	//	-- for the first player when no player folds.
	//	--
	//	-- @param board_cards a non - empty vector of board cards
	//	-- @param call_matrix a tensor where the computed matrix is stored
	void get_last_round_call_matrix(const ArrayXf& board_cards, ArrayXXf& call_matrix);

private:

	LeducEvaluator _evaluator;

	card_tools _cardTools;
};

