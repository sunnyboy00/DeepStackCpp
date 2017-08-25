#pragma once
#include <vector>
#include <Eigen/Dense>
#include <numeric>
#include "Node.h"
#include "assert.h"
#include "arguments.h"
#include "Util.h"
#include "terminal_equity.h"
#include "cfrd_gadget_f.h"
#include "LookaheadResult_f.h"

class TreeLookahead
{
public:

	//Action order:
	// - fold
	// - call
	// - not all int bets
	// - all inn bet.
	enum Actions {
		Fold = 0, Call = 1, FirstNotAllInnBet = 2, AllInnBet = -1
	};


	TreeLookahead(Node& root, long long skip_iters = cfr_skip_iters, long long iters = cfr_iters);

	~TreeLookahead();

//private:

	size_t _cfr_skip_iters;

	size_t _cfr_iters;

	Node _root;

	cfrd_gadget_f* _reconstruction_gadget;

	Range _reconstruction_opponent_cfvs;

	bool _reconstruction = false;

	//--dimensions in tensor
	static const int action_dimension = 0;
	static const int card_dimension = 1;

	// --for ease of implementation, we use small epsilon rather than zero when working with regrets
	const float regret_epsilon = 1.0f / 1000000000;

	map<ArrayX*, terminal_equity*> _cached_terminal_equities;

	// Contains sum of cfvs data for the root node that are accumulated after skip_iters iterations
	Ranges _average_root_cfvs_data;

	// Average average strategy data
	ArrayXX _average_root_strategy;

	// Current strategy
	ArrayXX _current_strategy;

	// Do wee need to swap players(if the first player to act in the lookahed is the second player)
	int _playersSwapOffset;


	//	--- Re - solves the lookahead using input ranges.
	//	--
	//	--Uses the input range for the opponent instead of a gadget range, so only
	//	-- appropriate for re - solving the root node of the game tree(where ranges
	//	-- are fixed).
	//	--
	//	-- @{build_lookahead
	//} must be called first.
	//--
	//-- @param player_range a range vector for the re - solving player
	//-- @param opponent_range a range vector for the opponent
	void resolve_first_node(const Range& player_range, const  Range& opponent_range);

	//-- - Re - solves the lookahead using an input range for the player and
	//--the @{cfrd_gadget | CFRDGadget
	//} to generate ranges for the opponent.
	//--
	//-- @{build_lookahead} must be called first.
	//--
	//-- @param player_range a range vector for the re - solving player
	//-- @param opponent_cfvs a vector of cfvs achieved by the opponent
	//-- before re - solving
	void resolve(const Range& player_range, const Range& opponent_cfvs);

	//-- - Gives the average counterfactual values for the opponent during re - solving
	//	-- after a chance event(the betting round changes and more cards are dealt).
	//	--
	//	--Used during continual re - solving to track opponent cfvs.The lookahead must
	//	-- first be re - solved with @{resolve} or @{resolve_first_node}.
	//	--
	//	-- @param action_index the action taken by the re - solving player at the start
	//	-- of the lookahead
	//	-- @param board a tensor of board cards, updated by the chance event
	//	-- @return a vector of cfvs
	Tf1 get_chance_action_cfv(int action_index, Tf1& board);

	//-- - Gets the results of re - solving the lookahead.
	//	--
	//	--The lookahead must first be re - solved with @{resolve} or
	//	-- @{resolve_first_node}.
	//	--
	//	-- @return a table containing the fields :
	//--
	//	-- * `strategy`: an AxK tensor containing the re - solve player's strategy at the
	//	--root of the lookahead, where A is the number of actions and K is the range size
	//	--
	//	-- * `achieved_cfvs`: a vector of the opponent's average counterfactual values at the 
	//	--root of the lookahead
	//	--
	//	-- * `children_cfvs`: an AxK tensor of opponent average counterfactual values after
	//	-- each action that the re - solve player can take at the root of the lookahead
	LookaheadResult_f get_results();

	//-- - Re - solves the lookahead.
	void _compute();

	//-- - Uses regret matching to generate the players' current strategies.
	//-- @local
	void _compute_current_strategies();

	//-- - Using the players' current strategies, computes their probabilities of
	//--reaching each state of the lookahead.
	void _compute_ranges();


	//-- - Using the players' reach probabilities, calls the neural net to compute the
	//--players' counterfactual values at the depth-limited states of the lookahead.
	void _compute_terminal_equities_next_street_box();

	//-- - Updates the players' average strategies with their current strategies.
	//-- @param iter the current iteration number of re - solving
	void _compute_update_average_strategies(ArrayXX& current_strategy);

	//-- - Using the players' reach probabilities, computes their counterfactual
	//--values at each lookahead state which is a terminal state of the game. Saves it in the cfvs_data.
	void _compute_terminal_equities_terminal_equity();

	// Filling terminal equities for the second street
	void _processSecondStreetTermEq(int d, Tf2& csvfs_res);

	// Filling terminal equities for the first street
	void _processFirstStreetTermEq(int d, Tf2& csvfs_res);

	//-- - Using the players' reach probabilities, computes their counterfactual
	//	--values at all terminal states of the lookahead.
	//	--
	//	--These include terminal states of the game and depth - limited states.
	void _compute_terminal_equities();

	//-- - Using the players' reach probabilities and terminal counterfactual
	//--values, computes their cfvs at all states of the lookahead.
	void _compute_cfvs();

	//-- - Updates the players' average counterfactual values with their cfvs from the
	//--current iteration.
	//-- @param iter the current iteration number of re - solving
	void _compute_cumulate_average_cfvs();

	//-- - Normalizes the players' average strategies.
	//	--
	//	--Used at the end of re - solving so that we can track un - normalized average
	//	-- strategies, which are simpler to compute.
	void _compute_normalize_average_strategies();

	//-- - Using the players' counterfactual values, updates their total regrets
	// -- for every state in the lookahead.
	void _compute_regrets();

	int _getCurrentPlayer(const Node& node);

	int _getCurrentOpponent(const Node& node);

	//-- - Gets an evaluator for player equities at a terminal node.
	//--
	//--Caches the result to minimize creation of @{terminal_equity | TerminalEquity
	//}
	//--objects.
	//-- @param node the terminal node to evaluate
	//-- @return a @{terminal_equity | TerminalEquity} evaluator for the node
	terminal_equity* _get_terminal_equity(Node& node);

	//-- - Recursively walks the tree, applying the CFR algorithm.
	//	-- @param node the current node in the tree
	//	-- @param iter the current iteration number
	//	-- @local
	void cfrs_iter_dfs(Node& node, size_t iter);

	void _fillCFvaluesForNonTerminalNode(Node &node, size_t iter);

	ArrayXX ComputeRegrets(Node &node, CFVS(&cf_values_allactions)[players_count]);

	void _fillChanceChildRanges(Node &node, Ranges(&children_ranges_absolute)[players_count]);

	void _fillChildRanges(Node & node, Ranges(&children_ranges_absolute)[players_count]);

	//-- - Update a node's total regrets with the current iteration regrets.
	//-- @param node the node to update
	//-- @param current_regrets the regrets from the current iteration of CFR
	void update_regrets(Node& node, const ArrayXX& current_regrets);

	// Fill cf_values for terminal nodes
	void _fillCFvaluesForTerminalNode(Node &node);

	//-- - Generates the opponent's range for the current re-solve iteration using
	//	--the @{cfrd_gadget | CFRDGadget}.
	//	-- @param iteration the current iteration number of re - solving
	void _set_opponent_starting_range();

	//-- - Normalizes the players' average counterfactual values.
	//--
	//--Used at the end of re - solving so that we can track un - normalized average
	//-- cfvs, which are simpler to compute.
	void _compute_normalize_average_cfvs();

	void _fillCurrentStrategy(Node & node);
};

