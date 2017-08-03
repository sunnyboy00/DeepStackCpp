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

	//--per layer information about tree actions
	//--per layer actions are the max number of actions for any of the nodes on the layer

	// Number of bet actions per layer:
	// bets_count = total_actions - terminal_actions
	map<int, int> bets_count;

	// Total number of possible actions per layer. This includes: terminal actions (fold + check) and bets(with allin):
	// actions_count = bets_count + terminal_actions_count = bets_count + 2
	map<int, int> actions_count;

	// Terminal actions per layer:
	// terminal_actions = 2 (fold + check).
	map<int, int> terminal_actions_count;

	// Total bets except allin:
	// nonallinbets_count = bets_count - 1
	map<int, int> nonallinbets_count;

	// All actions per layer except terminal actions:
	// nonterminal_nodes_count = actions_count - terminal_actions_count = (bets_count + 2) - (2) = bets_count.
	map<int, int> nonterminal_nodes_count;

	// Bets per layer without allin bet:
	// nonterminal_nonallin_nodes_count = bets - 1
	map<int, int> nonterminal_nonallin_nodes_count;
	map<int, int> all_nodes_count;
	map<int, int> terminal_nodes_count;
	map<int, int> allin_nodes_count;
	map<int, int> inner_nodes_count;



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


	float regret_epsilon;
	ArrayXf acting_player;

	bool first_call_terminal;
	bool first_call_transition;
	bool first_call_check;
};

