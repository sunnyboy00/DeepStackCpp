#pragma once
#include <vector>
#include <Eigen/Dense>

#include "Node.h"
#include "assert.h"
#include "arguments.h"
#include "Util.h"
#include "terminal_equity.h"
#include "cfrd_gadget.h"
#include "LookaheadResult.h"

using namespace std;
using namespace Eigen;

class lookahead
{
	// 	-- - A depth - limited lookahead of the game tree used for re - solving.
public:
	lookahead();
	~lookahead();
	terminal_equity _terminal_equity;


	// Game tree
	Node tree;

	// Lookahead depth to solve
	long long depth;

	const float regret_epsilon = 1.0f / 1000000000;

	//-----------------------------------------------------------------------
	//--Per layer information about tree actions
	//--per layer actions are the max number of actions for any of the nodes on the layer

	// Max number of actions per layer. That is max number of children for all layer nodes. 
	map<int, int> actions_count;

	// Max terminal actions per layer:
	// For the final layer: terminal_actions = 2 (fold + check).
	// For the non-final layer: terminal_actions = 2 + chance_nodes_count(1) = 3. (?)
	map<int, int> terminal_actions_count;

	// Bet actions per layer:
	// bets_count = max(total_actions) - max(terminal_actions)
	map<int, int> bets_count;

	// Total bets except allin:
	// nonallinbets_count = bets_count - 1
	map<int, int> nonallinbets_count;


	//--------------------------------------------------------------------
	// The node counts per layer: 

	// Nodes per layer without allin node:
	// nonterminal_nonallin_nodes_count = bets - 1
	map<int, int> nonterminal_nonallin_nodes_count;

	// ----------------------------------------------------------------
	map<int, Tf5> pot_size;

	//--lookahead main data structures
	//--all the structures are per - layer tensors, that is, each layer holds the data in n - dimensional tensors

	//[actions x parent_action x grandparent_id x batch x players x range]
	map<int, Tf5> ranges_data;
	map<int, Tf5> cfvs_data;
	map<int, Tf5> average_cfvs_data;
	map<int, Tf5> placeholder_data;

	//--data structures for one player[actions x parent_action x grandparent_id x 1 x range]
	map<int, Tf4> average_strategies_data;
	map<int, Tf4> current_strategy_data;
	map<int, Tf4> regrets_data;
	map<int, Tf4> current_regrets_data;
	map<int, Tf4> positive_regrets_data;
	//--used to mask empty actions
	map<int, Tf4> empty_action_mask;

	//--data structures for summing over the actions [1 x parent_action x grandparent_id x range]
	map<int, Tf4> regrets_sum;


	//--used to hold and swap inner(nonterminal) nodes when doing some transpose operations
	// --data structures for inner nodes (not terminal nor allin) [bets_count x parent_nonallinbetscount x gp_id x batch x players x range]
	map<int, Tf5> inner_nodes;
	map<int, Tf5> inner_nodes_p1;
	map<int, Tf5> swap_data;

	// Which player acts at particular depth. 0 - first player to act. 1 - second player to act.
	ArrayXf acting_player;

	bool first_call_terminal;
	bool first_call_transition;
	bool first_call_check;


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
	void resolve_first_node(const Tf1& player_range, const  Tf1& opponent_range);

	//-- - Re - solves the lookahead using an input range for the player and
	//--the @{cfrd_gadget | CFRDGadget
	//} to generate ranges for the opponent.
	//--
	//-- @{build_lookahead} must be called first.
	//--
	//-- @param player_range a range vector for the re - solving player
	//-- @param opponent_cfvs a vector of cfvs achieved by the opponent
	//-- before re - solving
	void resolve(Tf1& player_range, Tf1& opponent_cfvs);

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
	LookaheadResult get_results();


private:

	vector<Tf3> _next_street_boxes_inputs;

	vector<Tf3> _next_street_boxes_outputs;

	vector<Tf3> _next_street_boxes; // ??

	cfrd_gadget* _reconstruction_gadget;

	Tf1 _reconstruction_opponent_cfvs;

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
	void _compute_update_average_strategies(size_t iter);

	//-- - Using the players' reach probabilities, computes their counterfactual
	//--values at each lookahead state which is a terminal state of the game. Saves it in the cfvs_data.
	void _compute_terminal_equities_terminal_equity();

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
	void _compute_cumulate_average_cfvs(size_t iter);

	//-- - Normalizes the players' average strategies.
	//	--
	//	--Used at the end of re - solving so that we can track un - normalized average
	//	-- strategies, which are simpler to compute.
	void _compute_normalize_average_strategies();

	//-- - Using the players' counterfactual values, updates their total regrets
	// -- for every state in the lookahead.
	void _compute_regrets();

	//-- - Generates the opponent's range for the current re-solve iteration using
	//	--the @{cfrd_gadget | CFRDGadget}.
	//	-- @param iteration the current iteration number of re - solving
	void _set_opponent_starting_range();

	//-- - Normalizes the players' average counterfactual values.
	//--
	//--Used at the end of re - solving so that we can track un - normalized average
	//-- cfvs, which are simpler to compute.
	void _compute_normalize_average_cfvs();
};