#pragma once
#include <Eigen/Dense>
#include "assert.h"
#include "LeducEvaluator.h"
#include "game_settings.h"
#include "assert.h"

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

	//-- - Sets the board cards for the evaluator and creates its internal data structures.
	//-- @param board a possibly empty vector of board cards
	void set_board(const ArrayXf& board);

	//-- - Sets the evaluator's call matrix, which gives the equity for terminal
	//--nodes where no player has folded.
	//--
	//--For nodes in the last betting round, creates the matrix `A` such that for player ranges
	//-- `x` and `y`, `x'Ay` is the equity for the first player when no player folds. For nodes
	//-- in the first betting round, gives the weighted average of all such possible matrices.
	//--
	//-- @param board a possibly empty vector of board cards
	void _set_call_matrix(const ArrayXf& board);

	//-- - Sets the evaluator's fold matrix, which gives the equity for terminal
	//	--nodes where one player has folded.
	//	--
	//	--Creates the matrix `B` such that for player ranges `x` and `y`, `x'By` is the equity
	//	-- for the player who doesn't fold
	//	-- @param board a possibly empty vector of board cards
	void _set_fold_matrix(const ArrayXf& board);

	//-- - Computes(a batch of) counterfactual values that a player achieves at a terminal node
	//-- where no player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a batch of opponent ranges in an NxK tensor, where N is the batch size
	//-- and K is the range size
	//-- @param result a NxK tensor in which to save the cfvs
	template <typename Derived>
	void call_value(const ArrayBase<Derived> & ranges, ArrayBase<Derived> & result);

	//-- - Computes(a batch of) counterfactual values that a player achieves at a terminal node
	//-- where a player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a batch of opponent ranges in an NxK tensor, where N is the batch size
	//-- and K is the range size
	//-- @param result A NxK tensor in which to save the cfvs.Positive cfvs are returned, and
	//--must be negated if the player in question folded.
	template <typename Derived>
	void fold_value(const ArrayBase<Derived> & ranges, ArrayBase<Derived> & result);

	//	-- - Computes the counterfactual values that both players achieve at a terminal node
	//-- where either player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a 2xK tensor containing ranges for each player(where K is the range size)
	//-- @param result a 2xK tensor in which to store the cfvs for each player
	//-- @param folding_player which player folded
	void tree_node_fold_value(const ArrayXXf& ranges, ArrayXXf& result, int folding_player);

	//-- - Returns the matrix which gives showdown equity for any ranges.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @return For nodes in the last betting round, the matrix `A` such that for player ranges
	//-- `x` and `y`, `x'Ay` is the equity for the first player when no player folds.For nodes
	//-- in the first betting round, the weighted average of all such possible matrices.
	ArrayXXf get_call_matrix();

	//-- - Computes the counterfactual values that both players achieve at a terminal node
	//-- where no player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a 2xK tensor containing ranges for each player(where K is the range size)
	//-- @param result a 2xK tensor in which to store the cfvs for each player
	void tree_node_call_value(const ArrayXXf& ranges, ArrayXXf& result);


//private: ToDo:Remove after testing

	LeducEvaluator _evaluator;

	card_tools _cardTools;

	ArrayXXf _equity_matrix;

	ArrayXXf _fold_matrix;
};

//#include "terminal_equity.cpp"