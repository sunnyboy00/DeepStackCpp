#include "tree_values.h"



tree_values::tree_values()
{
}


tree_values::~tree_values()
{
}

void tree_values::_fill_ranges_dfs(Node & node, ArrayXXf & ranges_absolute)
{
	node.ranges_absolute = ranges_absolute;

	if (node.terminal)
	{
		return;
	}

	assert(node.strategy.size() > 0);

	const int actions_count = (int)node.children.size();
	const int currentPlayerIndex = node.current_player;
	const int opponentIndex = 1 - node.current_player;

	//--check that it's a legal strategy
	ArrayXXf strategy_to_check = node.strategy;

	if (node.current_player != chance)
	{
		ArrayXf checksum = strategy_to_check.colwise().sum();
		assert((strategy_to_check > 0.f).all());
		assert((strategy_to_check < 1.001f).all());
		assert((checksum > 0.999f).all());
		assert((checksum < 1.001f).all());
	}

	assert((node.ranges_absolute >= 0).all());
	assert((node.ranges_absolute < 1).all());

	//--check if the range consists only of cards that don't overlap with the board

#ifdef _DEBUG
	ArrayXXf hands_mask = _cardTools.get_possible_hand_indexes(node.board);
	hands_mask.resize(1, card_count);
	hands_mask = Util::ExpandAs(hands_mask, node.ranges_absolute);

	assert((hands_mask * node.ranges_absolute).sum() == node.ranges_absolute.sum()); // Checking that multiplication with possible hands range did not changed anything.

	//impossible_hands_mask.conservativeResize(1, card_count);
	//Util::ExpandAs(impossible_hands_mask, node.ranges_absolute);
	//float impossible_range_sum = impossible_hands_mask.sum();
	//assert(impossible_range_sum == 0)
#endif

	//actions_count X [players_count, card_count]
	//vector<ArrayXXf&> children_ranges_absolute(actions_count);
	ArrayXXf child_range(players_count, card_count);

	//--chance player
	if (node.current_player == chance)
	{
		for (size_t i = 0; i < actions_count; i++)
		{
			child_range.row(0) = node.ranges_absolute.row(0) * node.strategy.row(i);
			child_range.row(1) = node.ranges_absolute.row(1) * node.strategy.row(i);

			//--go deeper
			_fill_ranges_dfs(*node.children[i], child_range);
		}
	}
			
	else //--player
	{

		for (size_t i = 0; i < actions_count; i++)
		{
			//--copy the range for the non-acting player  
			child_range.row(opponentIndex) = node.ranges_absolute.row(opponentIndex);

			//  --multiply the range for the acting player using his strategy    
			child_range.row(currentPlayerIndex) = node.ranges_absolute.row(currentPlayerIndex) * node.strategy.row(i);

			//--go deeper
			_fill_ranges_dfs(*node.children[i], child_range);
		}
	}
}

void tree_values::_compute_values_dfs(Node& node)
{
	const int opponent = 1 - node.current_player;

	//--compute values using terminal_equity in terminal nodes
	if (node.terminal)
	{
		assert(node.type == terminal_fold || node.type == terminal_call);

		_terminal_equity.set_board(node.board);

		ArrayXXf values = ArrayXXf::Zero(players_count, card_count);

		if (node.type == terminal_fold)
		{
			_terminal_equity.tree_node_fold_value(node.ranges_absolute, values, opponent);
		}
		else
		{
			_terminal_equity.tree_node_call_value(node.ranges_absolute, values);
		}

		//--multiply by the pot
		values = values * node.pot;
		node.cf_values = ArrayXXf(node.ranges_absolute);
		node.cf_values_br = ArrayXXf(node.ranges_absolute);
		//node.cf_values = values:viewAs(node.ranges_absolute)
		//node.cf_values_br = values : viewAs(node.ranges_absolute)
	}
	else
	{

		const int actions_count = (int)node.children.size();

		const int ranges_size = card_count; //node.ranges_absolute:size(2)

			//--[[actions, players, ranges]]
			//local cf_values_allactions = arguments.Tensor(#node.children, 2, ranges_size) : fill(0)
			//local cf_values_br_allactions = arguments.Tensor(#node.children, 2, ranges_size) : fill(0)

			//[actions_count x card_count][players_count]
			Array<float, Dynamic, card_count>  cf_values_allactions[players_count];
			Array<float, Dynamic, card_count>  cf_values_br_allactions[players_count];

			//for (size_t i = 0; i < actions_count; i++)
			//{
			//	Node* child_node = node.children[i];
			//	_compute_values_dfs(*child_node);

			//	cf_values_allactions[i] = child_node.cf_values
			//		cf_values_br_allactions[i] = child_node.cf_values_br
			//		end
			//}

	}
}
