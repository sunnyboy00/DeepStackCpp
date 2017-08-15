#include "LookaheadBuilder.h"

#include <cstdio>
#include <iostream>
using namespace std;



LookaheadBuilder::LookaheadBuilder(lookahead& ahead)
{
	_lookahead = &ahead;
}


LookaheadBuilder::~LookaheadBuilder()
{
}

//void LookaheadBuilder::build_lookahead(Node& tree)
//{
//	build_from_tree(tree);
//}

void LookaheadBuilder::build_from_tree(Node& tree)
{
	_lookahead->tree = &tree;
	_lookahead->depth = tree.depth;

	vector<Node*> treeVec;
	treeVec.push_back(&tree);
	_compute_tree_structures(treeVec, 0);

	//--construct the initial data structures using the bet counts
	construct_data_structures();

		//--traverse the tree and fill the data structures(pot sizes, non - existing actions, ...)
		//--node, layer, action, parent_action, gp_id
	set_datastructures_from_tree_dfs(tree, 0, 0, 0, 0);

		//--set additional info
	assert(_lookahead->terminal_actions_count[0] == 1 || _lookahead->terminal_actions_count[0] == 2);

	_lookahead->first_call_terminal = _lookahead->tree->children[1]->terminal;
	_lookahead->first_call_transition = _lookahead->tree->children[1]->current_player == chance;
	_lookahead->first_call_check = (!_lookahead->first_call_terminal) && (!_lookahead->first_call_transition);


	//--we mask out fold as a possible action when check is for free, due to
	//--1) fewer actions means faster convergence
	//--2) we need to make sure prob of free fold is zero because ACPC dealer changes such action to check
	if (_lookahead->tree->bets[0] == _lookahead->tree->bets[1])
	{
		RemoveF1D(_lookahead->empty_action_mask[1], 0).setZero();
	}

	//--construct the neural net query boxes
	_construct_transition_boxes();

	_lookahead->_terminal_equity.set_board(tree.board);
}

void LookaheadBuilder::_compute_tree_structures(const vector<Node*>& current_layer, int current_depth)
{
	int layer_actions_count = 0;
	int layer_terminal_actions_count = 0;
	vector<Node*> next_layer;

	for (size_t n = 0; n < current_layer.size(); n++)
	{
		Node* node = current_layer[n];
		layer_actions_count = max(layer_actions_count, (int)node->children.size());

		int node_terminal_actions_count = 0;

		for (size_t c = 0; c < node->children.size(); c++)
		{
			if (node->children[c]->terminal || node->children[c]->current_player == chance)
			{
				node_terminal_actions_count++;
			}
		}

		layer_terminal_actions_count = max(layer_terminal_actions_count, node_terminal_actions_count);

		//--add children of the node to the next layer for later pass of BFS
		if (!node->terminal)
		{
			for (size_t c = 0; c < node->children.size(); c++)
			{
				next_layer.push_back(node->children[c]);
			}
		}
	}

	assert((layer_actions_count == 0) == (next_layer.size() == 0));
	assert((layer_actions_count == 0) == (current_depth == _lookahead->depth));

	//--set action and bet counts
	_lookahead->bets_count[current_depth] = layer_actions_count - layer_terminal_actions_count;

	//--if no alllin...
	if (layer_actions_count == 2)
	{
		assert(layer_actions_count == layer_terminal_actions_count);
		_lookahead->nonallinbets_count[current_depth] = 0;
	}
	else if (_lookahead->bets_count[current_depth] > 0)
	{
		_lookahead->nonallinbets_count[current_depth] = _lookahead->bets_count[current_depth] - 1; //--remove allin
	}

	_lookahead->terminal_actions_count[current_depth] = layer_terminal_actions_count;
	_lookahead->actions_count[current_depth] = layer_actions_count;

	if (next_layer.size() > 0)
	{
		assert(layer_actions_count >= 2);
		//--go deeper
		_compute_tree_structures(next_layer, current_depth + 1);
	}
}

void LookaheadBuilder::set_datastructures_from_tree_dfs(Node & node, int layer, int action_id, int parent_id, int gp_id)
{
	//--fill the potsize
	assert(node.pot);

	RemoveF3D(_lookahead->pot_size[layer], action_id, parent_id, gp_id).setConstant(node.pot);
	node.lookahead_coordinates.resize(action_id, parent_id, gp_id);

	//--transition call cannot be allin call

#ifdef _DEBUG
	if (node.current_player == chance)
	{
		assert(parent_id <= _lookahead->nonallinbets_count[layer - 2]);
	}
#endif

	if ((layer < _lookahead->depth + 1) && !(node.terminal || node.current_player == chance))
	{
		//--parent is not an allin raise
		assert(parent_id <= _lookahead->nonallinbets_count[layer - 2]);

		int gp_nonallinbets_count = _lookahead->nonallinbets_count[layer - 2];
		int prev_layer_terminal_actions_count = _lookahead->terminal_actions_count[layer - 1];

		//--compute next coordinates for parent and grandparent
		int next_parent_id = action_id - prev_layer_terminal_actions_count; // We are substructing prev_layer_terminal_actions_count because that is prev_layer_terminal_actions_count the zero reference point. Actions with children begins after the prev_layer_terminal_actions_count.
		int next_gp_id = gp_id * gp_nonallinbets_count + (parent_id);

		//--do we need to mask some actions for that node ? 
		// (that is, does the node have fewer children than the max number of children for any node on this layer)
		if (node.children.size() < _lookahead->actions_count[layer])
		{
			ProcessNodeWithEmptyActions(node, layer, action_id, next_parent_id, next_gp_id);
		}
		else
		{
			//--node has full action count, easy to handle
			for (int child_id = 0; child_id < node.children.size(); child_id++)
			{
				Node* child_node = node.children[child_id];
				//--go deeper
				set_datastructures_from_tree_dfs(*child_node, layer + 1, child_id, next_parent_id, next_gp_id);
			}
		}
	}
}

void LookaheadBuilder::ProcessNodeWithEmptyActions(Node &node, int layer, int action_id, int next_parent_id, int next_gp_id)
{
	//--we need to mask non existing padded bets
	assert(layer > 0);

	int terminal_actions_count = _lookahead->terminal_actions_count[layer];
	assert(terminal_actions_count == 2);
	int actions_count = (int)node.children.size();
	int existing_bets_count = actions_count - terminal_actions_count;

#ifdef _DEBUG
	//--allin situations
	if (existing_bets_count == 0)
	{
		assert(action_id == _lookahead->actions_count[layer - 1] - 1); // Allin is always the last action. And action_id is zero based. Thats why -1;
	}
#endif

	for (int child_id = 0; child_id < terminal_actions_count; child_id++) // First two actions are terminal actions 
	{
		Node* child_node = node.children[child_id];
		//--go deeper
		set_datastructures_from_tree_dfs(*child_node, layer + 1, child_id, next_parent_id, next_gp_id);
	}

	//--we need to make sure that even though there are fewer actions, the last action / allin
	//  is has the same last index as if we had full number of actions
	//--we manually set the action_id as the last action(allin)
	for (int b = 0; b < existing_bets_count; b++) // After the first two actions there are bets 
	{
		assert(node.children.size() > b + 1);
		size_t childIndex = node.children.size() - b - 1;
		set_datastructures_from_tree_dfs(*node.children[childIndex],
			layer + 1,
			_lookahead->actions_count[layer] - b - 1, // Action id. We go in reverse direction here from the allin which is always possible. -1 because we want zero based action id.
			next_parent_id,
			next_gp_id);
	}

	// We are masking out as 0 all impossible actions except allin(empty actions).
	//size_t upperBound = _lookahead->empty_action_mask[layer + 1].dimension(0) - existing_bets_count;
	//for (int actionToMask = terminal_actions_count; actionToMask < upperBound; actionToMask++)
	//{
	//	RemoveF3D(_lookahead->empty_action_mask[layer + 1], actionToMask, next_parent_id, next_gp_id).setZero();
	//}

	Util::FillSlice(_lookahead->empty_action_mask[layer + 1],
	{{ 
		{ terminal_actions_count,-(existing_bets_count + 1) },
		{next_parent_id, next_parent_id},
		{next_gp_id, next_gp_id},
		{0, -1}
	}},
	0);
}

void LookaheadBuilder::_compute_structure()
{
	assert(_lookahead->tree->street >= 1 && _lookahead->tree->street <= 2);

	//--which player acts at particular depth
	_lookahead->acting_player.resize(_lookahead->depth + 2);
	_lookahead->acting_player.setConstant(-1);

	_lookahead->acting_player[0] = 0; //--in lookahead, 0 does not stand for player IDs, it's just the first player to act. 
	for (int d = 1; d <= _lookahead->depth + 1; d++)
	{
		_lookahead->acting_player[d] = 1 - _lookahead->acting_player[d - 1];
	}

	// Setting previous fake layers data for the root node. 
	// We 'creating' two layers with one not allin bet. Just for the calculation of nodes count below. (?)
	_lookahead->bets_count[-2] = 1;
	_lookahead->bets_count[-1] = 1;

	_lookahead->nonallinbets_count[-2] = 1;
	_lookahead->nonallinbets_count[-1] = 1;

	_lookahead->terminal_actions_count[-2] = 0;
	_lookahead->terminal_actions_count[-1] = 0;

	_lookahead->actions_count[-2] = 1;
	_lookahead->actions_count[-1] = 1;

	//--compute the node counts:

	_lookahead->nonterminal_nonallin_nodes_count[-1] = 1; // Fake layer, mb chance?
	_lookahead->nonterminal_nonallin_nodes_count[0] = 1; // The root node
	_lookahead->nonterminal_nonallin_nodes_count[1] = _lookahead->bets_count[0] - 1;

	for (int d = 1; d <= _lookahead->depth; d++)
	{
		// nonterminal_nonallin_nodes_count = 
		// [count of all non terminal not allinn nodes in all layers before] *  [nonallinbets_count in layer before] * [nonallinbets_count in current layer].

		_lookahead->nonterminal_nonallin_nodes_count[d + 1] = 
			_lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->nonallinbets_count[d - 1] * _lookahead->nonallinbets_count[d];
	}
}


void LookaheadBuilder::construct_data_structures()
{
	_compute_structure();

	//--create the data structure for the first two layers

	//--data structures[actions x parent_action x grandparent_id x batch x players x range]
	const Eigen::array<DenseIndex, 5> ranges_data_0_dims{ { 1, 1, 1, players_count, card_count } };
	const Eigen::array<DenseIndex, 5> ranges_data_1_dims{ { _lookahead->actions_count[0], 1, 1, players_count, card_count } };

	Util::ResizeAndFill(_lookahead->ranges_data[0], ranges_data_0_dims, 1.0f / card_count);
	Util::ResizeAndFill(_lookahead->ranges_data[1], ranges_data_1_dims, 1.0f / card_count);

	Util::ResizeAndFill(_lookahead->pot_size[0], ranges_data_0_dims);
	Util::ResizeAndFill(_lookahead->pot_size[1], ranges_data_1_dims);

	Util::ResizeAndFill(_lookahead->cfvs_data[0], ranges_data_0_dims);
	Util::ResizeAndFill(_lookahead->cfvs_data[1], ranges_data_1_dims);

	Util::ResizeAndFill(_lookahead->average_cfvs_data[0], ranges_data_0_dims);
	Util::ResizeAndFill(_lookahead->average_cfvs_data[1], ranges_data_1_dims);

	Util::ResizeAndFill(_lookahead->placeholder_data[0], ranges_data_0_dims);
	Util::ResizeAndFill(_lookahead->placeholder_data[1], ranges_data_1_dims);


	//--data structures for one player[actions x parent_action x grandparent_id x 1 x range]
	Eigen::array<DenseIndex, 4> one_pl_dims{ { _lookahead->actions_count[0], 1, 1, card_count } };

	Util::ResizeAndFill(_lookahead->average_strategies_data[1], one_pl_dims);
	Util::ResizeAndFill(_lookahead->current_strategy_data[1], one_pl_dims);
	Util::ResizeAndFill(_lookahead->regrets_data[1], one_pl_dims);
	Util::ResizeAndFill(_lookahead->current_regrets_data[1], one_pl_dims);
	Util::ResizeAndFill(_lookahead->positive_regrets_data[1], one_pl_dims);
	Util::ResizeAndFill(_lookahead->empty_action_mask[1], one_pl_dims, 1.0f);


	//--data structures for summing over the actions[1 x parent_action x grandparent_id x range]
	Util::ResizeAndFill(_lookahead->regrets_sum[0], { 1, 1, 1, card_count });

	Util::ResizeAndFill(_lookahead->regrets_sum[1], { 1, _lookahead->bets_count[0], 1, card_count });

	//--data structures for inner nodes(not terminal nor allin)[bets_count x parent_nonallinbetscount x gp_id x batch x players x range]
	Util::ResizeAndFill(_lookahead->inner_nodes[0], { 1, 1, 1, players_count, card_count });
	
	_lookahead->swap_data[0] = Util::Transpose(_lookahead->inner_nodes[0], { 0, 2, 1, 3, 4 });
	Util::ResizeAndFill(_lookahead->inner_nodes_p1[0], { 1, 1, 1, 1, card_count });

	if (_lookahead->depth > 1)
	{
		Eigen::array<DenseIndex, 5> inner_dims{ { _lookahead->bets_count[0], 1, 1, players_count, card_count } };
		Util::ResizeAndFill(_lookahead->inner_nodes[1], inner_dims);
		Util::ResizeAndFill(_lookahead->swap_data[1], inner_dims);
		_lookahead->swap_data[1] = Util::Transpose(_lookahead->inner_nodes[1], { 0, 2, 1, 3, 4 });
		Util::ResizeAndFill(_lookahead->inner_nodes_p1[1], { _lookahead->bets_count[0], 1, 1, 1, card_count });
	}


	//--create the data structures for the rest of the layers
	for (int d = 2; d <= _lookahead->depth; d++)
	{
		//--data structures[actions x parent_action x grandparent_id x batch x players x range]
		Eigen::array<DenseIndex, 5> deep_dims = { _lookahead->actions_count[d - 1],
			_lookahead->bets_count[d - 2],
			_lookahead->nonterminal_nonallin_nodes_count[d - 2],
			players_count, card_count };

		Util::ResizeAndFill(_lookahead->ranges_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->cfvs_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->placeholder_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->pot_size[d], deep_dims, (float)stack);

		// --data structures[actions x parent_action x grandparent_id x batch x 1 x range]
		Eigen::array<DenseIndex, 4> deep_player_dims = { _lookahead->actions_count[d - 1],
			_lookahead->bets_count[d - 2],
			_lookahead->nonterminal_nonallin_nodes_count[d - 2],
			card_count };

		Util::ResizeAndFill(_lookahead->average_strategies_data[d], deep_player_dims);
		Util::ResizeAndFill(_lookahead->current_strategy_data[d], deep_player_dims);
		Util::ResizeAndFill(_lookahead->regrets_data[d], deep_player_dims);
		Util::ResizeAndFill(_lookahead->current_regrets_data[d], deep_player_dims);
		Util::ResizeAndFill(_lookahead->empty_action_mask[d], deep_player_dims, 1.0f);
		Util::ResizeAndFill(_lookahead->positive_regrets_data[d], deep_player_dims);

		//--data structures[1 x parent_action x grandparent_id x batch x players x range]
		Util::ResizeAndFill(_lookahead->regrets_sum[d], { _lookahead->bets_count[d - 2],
			_lookahead->nonterminal_nonallin_nodes_count[d - 2],
			players_count,
			card_count });

		//--data structures for the layers except the last one
		if (d < _lookahead->depth)
		{
			Util::ResizeAndFill(_lookahead->inner_nodes[d], { _lookahead->bets_count[d - 1],
				_lookahead->nonallinbets_count[d - 2],
				_lookahead->nonterminal_nonallin_nodes_count[d - 2],
				players_count,
				card_count });

			Util::ResizeAndFill(_lookahead->inner_nodes_p1[d], { _lookahead->bets_count[d - 1],
				_lookahead->nonallinbets_count[d - 2],
				_lookahead->nonterminal_nonallin_nodes_count[d - 2],
				1,
				card_count });

			_lookahead->swap_data[d] = Util::Transpose(_lookahead->inner_nodes[d], { 0, 2, 1, 3, 4 });
		}
	}

}

void LookaheadBuilder::_construct_transition_boxes()
{
	if (_lookahead->tree->street == 2)
	{
		return;
	}

	//ToDo: Fix after adding NN

	//--load neural net if not already loaded
	//neural_net = neural_net or ValueNn()

	//_lookahead->next_street_boxes = {}
	//for (int d = 1; d < _lookahead->depth; d++)
	//{
	//	_lookahead->next_street_boxes[d] = NextRoundValue(neural_net);
	//	_lookahead->next_street_boxes[d]:start_computation(self.lookahead.pot_size[d][{2, {}, {}, 1, 1}]:clone() : view(-1));
	//}
}
