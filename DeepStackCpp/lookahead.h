#pragma once
#include "Node.h"
#include "assert.h"
#include "arguments.h"
#include "Util.h"
#include <Eigen/Dense>
#include "terminal_equity.h"

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

	// Depth
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
};

