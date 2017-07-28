#pragma once
#include "Node.h"
#include "assert.h"
#include "Util.h"
#include <Eigen/Dense>
#include "terminal_equity.h"

#define Tf5 Tensor<float, 5>
#define Tf4 Tensor<float, 4>

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
	Node& tree;

	// Depth
	int depth;

	//--per layer information about tree actions
	//--per layer actions are the max number of actions for any of the nodes on the layer
	map<int, int> bets_count;
	map<int, int> actions_count;
	map<int, int> terminal_actions_count;
	map<int, int> nonallinbets_count;

	vector<int> nonterminal_nodes_count;
	vector<int> nonterminal_nonallin_nodes_count;
	vector<int> all_nodes_count;
	vector<int> terminal_nodes_count;
	vector<int> allin_nodes_count;
	vector<int> inner_nodes_count;

	vector<Tf5> pot_size;

	//--lookahead main data structures
	//--all the structures are per - layer tensors, that is, each layer holds the data in n - dimensional tensors

	//[actions x parent_action x grandparent_id x batch x players x range]
	vector<Tf5> ranges_data;
	vector<Tf5> cfvs_data;
	vector<Tf5> average_cfvs_data;
	vector<Tf5> placeholder_data;
	vector<Tf5> placeholder_data;

	//--data structures for one player[actions x parent_action x grandparent_id x 1 x range]
	vector<Tf4> average_strategies_data;
	vector<Tf4> current_strategy_data;
	vector<Tf4> regrets_data;
	vector<Tf4> current_regrets_data;
	vector<Tf4> positive_regrets_data;
	//--used to mask empty actions
	vector<Tf4> empty_action_mask;

	//--data structures for summing over the actions [1 x parent_action x grandparent_id x range]
	vector<Tf4> regrets_sum;

	
	//--used to hold and swap inner(nonterminal) nodes when doing some transpose operations
	// --data structures for inner nodes (not terminal nor allin) [bets_count x parent_nonallinbetscount x gp_id x batch x players x range]
	vector<Tf5> inner_nodes;
	vector<Tf5> inner_nodes_p1;
	vector<Tf5> swap_data;


	float regret_epsilon;
	ArrayXf acting_player;
};

