#include "LookaheadBuilder.h"



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
	_lookahead->tree = tree;
	_lookahead->depth = tree.depth;

	vector<Node&> treeVec;
	treeVec.push_back(tree);
	_compute_tree_structures(treeVec, 1);

		//--construct the initial data structures using the bet counts
	construct_data_structures();

		//--traverse the tree and fill the datastructures(pot sizes, non - existin actions, ...)
		//--node, layer, action, parent_action, gp_id
	set_datastructures_from_tree_dfs(tree, 1, 1, 1, 1);

		//--set additional info
	assert(_lookahead->terminal_actions_count[1] == 1 || _lookahead->terminal_actions_count[1] == 2);

	_lookahead->first_call_terminal = _lookahead->tree.children[2].terminal;
	_lookahead->first_call_transition = _lookahead->tree.children[2].current_player == chance;
	_lookahead->first_call_check = (not _lookahead->first_call_terminal) && (not _lookahead->first_call_transition);


		//--we mask out fold as a possible action when check is for free, due to
		//--1) fewer actions means faster convergence
		//--2) we need to make sure prob of free fold is zero because ACPC dealer changes such action to check
	if (_lookahead->tree.bets[1] == _lookahead->tree.bets[2])
	{
		_lookahead->empty_action_mask[2][1]:fill(0);
	}

	//--construct the neural net query boxes
	_construct_transition_boxes();

	_lookahead->_terminal_equity.set_board(tree.board);
}

void LookaheadBuilder::_compute_tree_structures(vector<Node&> current_layer, int current_depth)
{
	int layer_actions_count = 0;
	int layer_terminal_actions_count = 0;
	vector<Node&> next_layer;

	for (size_t n = 0; n < current_layer.size(); n++)
	{
		Node node = current_layer[n];
		layer_actions_count = max(layer_actions_count, node.children.size());

		int node_terminal_actions_count = 0;

		for (size_t c = 0; c < current_layer[n].children.size(); c++)
		{
			if (node.children[c]->terminal || node.children[c]->current_player == chance)
			{
				node_terminal_actions_count++;
			}
		}

		layer_terminal_actions_count = max(layer_terminal_actions_count, node_terminal_actions_count);

		//--add children of the node to the next layer for later pass of BFS
		if (!node.terminal)
		{
			for (size_t c = 0; c < node.children.size(); c++)
			{
				next_layer.push_back(*node.children[c]);
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
	else
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
	////--fill the potsize
	//assert(node.pot);
	//_lookahead->pot_size[layer][{action_id, parent_id, gp_id, {}, {}}] = node.pot;

	//node.lookahead_coordinates = arguments.Tensor({ action_id, parent_id, gp_id });

	//	//--transition call cannot be allin call
	//	if node.current_player == constants.players.chance then
	//		assert(parent_id <= _lookahead->nonallinbets_count[layer - 2])
	//		end

	//		if layer < _lookahead->depth + 1 then
	//			local gp_nonallinbets_count = _lookahead->nonallinbets_count[layer - 2]
	//			local prev_layer_terminal_actions_count = _lookahead->terminal_actions_count[layer - 1]
	//			local gp_terminal_actions_count = _lookahead->terminal_actions_count[layer - 2]
	//			local prev_layer_bets_count = 0

	//			prev_layer_bets_count = _lookahead->bets_count[layer - 1]

	//			//--compute next coordinates for parent and grandparent
	//			local next_parent_id = action_id - prev_layer_terminal_actions_count
	//			local next_gp_id = (gp_id - 1) * gp_nonallinbets_count + (parent_id)

	//			if (not node.terminal) and (node.current_player ~= constants.players.chance) then

	//				//--parent is not an allin raise
	//				assert(parent_id <= _lookahead->nonallinbets_count[layer - 2])

	//				//--do we need to mask some actions for that node ? (that is, does the node have fewer children than the max number of children for any node on this layer)
	//				local node_with_empty_actions = (#node.children < _lookahead->actions_count[layer])

	//				if node_with_empty_actions then
	//					//--we need to mask nonexisting padded bets
	//					assert(layer > 1)

	//					local terminal_actions_count = _lookahead->terminal_actions_count[layer]
	//					assert(terminal_actions_count == 2)

	//					local existing_bets_count = #node.children - terminal_actions_count

	//					//--allin situations
	//					if existing_bets_count == 0 then
	//						assert(action_id == _lookahead->actions_count[layer - 1])
	//						end

	//						for child_id = 1, terminal_actions_count do
	//							local child_node = node.children[child_id]
	//							//--go deeper
	//							self : set_datastructures_from_tree_dfs(child_node, layer + 1, child_id, next_parent_id, next_gp_id)
	//							end

	//							//--we need to make sure that even though there are fewer actions, the last action / allin is has the same last index as if we had full number of actions
	//							//--we manually set the action_id as the last action(allin)
	//							for b = 1, existing_bets_count do
	//								self : set_datastructures_from_tree_dfs(node.children[#node.children - b + 1], layer + 1, _lookahead->actions_count[layer] - b + 1, next_parent_id, next_gp_id)
	//								end

	//								//--mask out empty actions
	//								_lookahead->empty_action_mask[layer + 1][{ {terminal_actions_count + 1, -(existing_bets_count + 1)}, next_parent_id, next_gp_id, {}}] = 0

	//							else
	//								//--node has full action count, easy to handle
	//							for child_id = 1, #node.children do
	//								local child_node = node.children[child_id]
	//								//--go deeper
	//								self : set_datastructures_from_tree_dfs(child_node, layer + 1, child_id, next_parent_id, next_gp_id)
	//								end
	//								end
	//								end
	//								end
}

void LookaheadBuilder::_compute_structure()
{
	assert(_lookahead->tree.street >= 1 && _lookahead->tree.street <= 2);

	_lookahead->regret_epsilon = 1.0f / 1000000000;

	//--which player acts at particular depth
	_lookahead->acting_player.resize(_lookahead->depth + 1);
	_lookahead->acting_player.fill(-1);

	_lookahead->acting_player[1] = 0; //--in lookahead, 0 does not stand for player IDs, it's just the first player to act. 


	for (int d = 1; d < _lookahead->depth; d++)
	{
		_lookahead->acting_player[d] = 1 - _lookahead->acting_player[d - 1];
	}


	_lookahead->bets_count[-1] = 1;
	_lookahead->bets_count[0] = 1;
	_lookahead->nonallinbets_count[-1] = 1;
	_lookahead->nonallinbets_count[0] = 1;
	_lookahead->terminal_actions_count[-1] = 0;
	_lookahead->terminal_actions_count[0] = 0;
	_lookahead->actions_count[-1] = 1;
	_lookahead->actions_count[0] = 1;

	//--compute the node counts
	_lookahead->nonterminal_nodes_count[1] = 1;
	_lookahead->nonterminal_nodes_count[2] = _lookahead->bets_count[1];
	_lookahead->nonterminal_nonallin_nodes_count[0] = 1;
	_lookahead->nonterminal_nonallin_nodes_count[1] = 1;
	_lookahead->nonterminal_nonallin_nodes_count[2] = _lookahead->nonterminal_nodes_count[2] - 1;
	_lookahead->all_nodes_count[1] = 1;
	_lookahead->all_nodes_count[2] = _lookahead->actions_count[1];
	_lookahead->terminal_nodes_count[1] = 0;
	_lookahead->terminal_nodes_count[2] = 2;
	_lookahead->allin_nodes_count[1] = 0;
	_lookahead->allin_nodes_count[2] = 1;
	_lookahead->inner_nodes_count[1] = 1;
	_lookahead->inner_nodes_count[2] = 1;

	for (int d = 1; d < _lookahead->depth; d++)
	{
		_lookahead->all_nodes_count[d + 1] = _lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->bets_count[d - 1] * _lookahead->actions_count[d];
		_lookahead->allin_nodes_count[d + 1] = _lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->bets_count[d - 1] * 1;
		_lookahead->nonterminal_nodes_count[d + 1] = _lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->nonallinbets_count[d - 1] * _lookahead->bets_count[d];
		_lookahead->nonterminal_nonallin_nodes_count[d + 1] = _lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->nonallinbets_count[d - 1] * _lookahead->nonallinbets_count[d];
		_lookahead->terminal_nodes_count[d + 1] = _lookahead->nonterminal_nonallin_nodes_count[d - 1] * _lookahead->bets_count[d - 1] * _lookahead->terminal_actions_count[d];
	}
}


void LookaheadBuilder::construct_data_structures()
{

	_compute_structure();

	//--create the data structure for the first two layers

	//--data structures[actions x parent_action x grandparent_id x batch x players x range]
	Eigen::array<DenseIndex, 5> ranges_data_0_dims{ { 1, 1, 1, players_count, card_count } };
	Eigen::array<DenseIndex, 5> ranges_data_1_dims{ { _lookahead->actions_count[0], 1, 1, players_count, card_count } };

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
	
	_lookahead->swap_data[0] = Util::Transpose(_lookahead->inner_nodes[0], 2, 3);
	Util::ResizeAndFill(_lookahead->inner_nodes_p1[0], { 1, 1, 1, 1, card_count });

	if (_lookahead->depth > 1)
	{
		Eigen::array<DenseIndex, 5> inner_dims{ { _lookahead->bets_count[0], 1, 1, players_count, card_count } };

		Util::ResizeAndFill(_lookahead->inner_nodes[1], inner_dims);
		Util::ResizeAndFill(_lookahead->swap_data[1], inner_dims);
		_lookahead->swap_data[1] = Util::Transpose(_lookahead->inner_nodes[1], 2, 3);
		Util::ResizeAndFill(_lookahead->inner_nodes_p1[1], { _lookahead->bets_count[0], 1, 1, 1, card_count });
	}


		//--create the data structures for the rest of the layers
	for (size_t d = 2; d < _lookahead->depth; d++)
	{
		//--data structures[actions x parent_action x grandparent_id x batch x players x range]
		Eigen::array<DenseIndex, 5> deep_dims = { _lookahead->actions_count[d - 1],
			_lookahead->bets_count[d - 2],
			_lookahead->nonterminal_nonallin_nodes_count[d - 2],
			players_count, card_count };

		Util::ResizeAndFill(_lookahead->ranges_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->cfvs_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->placeholder_data[d], deep_dims);
		Util::ResizeAndFill(_lookahead->pot_size[d], deep_dims, stack);


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


			_lookahead->swap_data[d] = Util::Transpose(_lookahead->inner_nodes[d], 2, 3);
		}
	}

}