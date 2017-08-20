#define NOMINMAX

#include "tree_values.h"


tree_values::tree_values(){}

void tree_values::_fill_ranges_dfs(Node & node, ArrayXX & ranges_absolute)
{
	node.ranges = ranges_absolute;

	if (node.terminal)
	{
		return;
	}

	assert(node.strategy.size() > 0);

	const int actions_count = (int)node.children.size();
	const int currentPlayerIndex = node.current_player;
	const int opponentIndex = 1 - node.current_player;

	//--check that it's a legal strategy
	ArrayXX strategy_to_check = node.strategy;

	if (node.current_player != chance)
	{
		ArrayX checksum = strategy_to_check.colwise().sum();
		assert((strategy_to_check > 0.f).all());
		assert((strategy_to_check < 1.001f).all());
		assert((checksum > 0.999f).all());
		assert((checksum < 1.001f).all());
	}

	assert((node.ranges >= 0).all());
	assert((node.ranges < 1).all());

	//--check if the range consists only of cards that don't overlap with the board

#ifdef _DEBUG
	ArrayXX hands_mask = _cardTools.get_possible_hand_indexes(node.board);
	hands_mask.resize(1, card_count);
	hands_mask = Util::ExpandAs(hands_mask, node.ranges);

	assert((hands_mask * node.ranges).sum() == node.ranges.sum()); // Checking that multiplication with possible hands range did not changed anything.

	//impossible_hands_mask.conservativeResize(1, card_count);
	//Util::ExpandAs(impossible_hands_mask, node.ranges_absolute);
	//float impossible_range_sum = impossible_hands_mask.sum();
	//assert(impossible_range_sum == 0)
#endif

	//actions_count X [players_count, card_count]
	//vector<ArrayXX&> children_ranges_absolute(actions_count);
	ArrayXX child_range(players_count, card_count);

	//--chance player
	if (node.current_player == chance)
	{
		for (size_t i = 0; i < actions_count; i++)
		{
			child_range.row(0) = node.ranges.row(0) * node.strategy.row(i);
			child_range.row(1) = node.ranges.row(1) * node.strategy.row(i);

			//--go deeper
			_fill_ranges_dfs(*node.children[i], child_range);
		}
	}
			
	else //--player
	{

		for (size_t i = 0; i < actions_count; i++)
		{
			//--copy the range for the non-acting player  
			child_range.row(opponentIndex) = node.ranges.row(opponentIndex);

			//  --multiply the range for the acting player using his strategy    
			child_range.row(currentPlayerIndex) = node.ranges.row(currentPlayerIndex) * node.strategy.row(i);

			//--go deeper
			_fill_ranges_dfs(*node.children[i], child_range);
		}
	}
}

void tree_values::_compute_values_dfs(Node& node)
{
	const int opponent = 1 - node.current_player;
	const int current_player = node.current_player;

	//--compute values using terminal_equity in terminal nodes
	if (node.terminal)
	{
		assert(node.type == terminal_fold || node.type == terminal_call);

		_terminal_equity.set_board(node.board);

		ArrayXX values = ArrayXX::Zero(players_count, card_count);

		if (node.type == terminal_fold)
		{
			_terminal_equity.tree_node_fold_value(node.ranges, values, opponent);
		}
		else
		{
			_terminal_equity.tree_node_call_value(node.ranges, values);
		}

		//--multiply by the pot
		values *= node.pot;
		node.cf_values = ArrayXX(values);
		node.cf_values_br = ArrayXX(values);
	}
	else
	{

		const int actions_count = (int)node.children.size();
		//[players_count x card_count][actions_count]
		node.cf_values = ArrayXX::Zero(players_count, card_count);
		node.cf_values_br = ArrayXX::Zero(players_count, card_count); 

		for (size_t i = 0; i < actions_count; i++)
		{
			Node* child_node = node.children[i];
			_compute_values_dfs(*child_node);

			if (current_player == chance) // ToDo: move check outside the loop and check performance
			{
				node.cf_values += child_node->cf_values;
				node.cf_values_br += child_node->cf_values_br;
			}
			else
			{
				node.cf_values.row(current_player) += child_node->cf_values.row(current_player) * node.strategy.row(i);
				node.cf_values.row(opponent) += child_node->cf_values.row(opponent);

				node.cf_values_br.row(opponent) += child_node->cf_values_br.row(opponent);

				if (i == 0)
				{
					node.cf_values_br.row(current_player) = child_node->cf_values_br.row(current_player);
				}
				else
				{
					node.cf_values_br.row(current_player) = node.cf_values_br.row(current_player).max(child_node->cf_values_br.row(current_player));
				}
			}
		}
	}

	//--counterfactual values weighted by the reach prob
	node.cfv_infset = ArrayX(players_count);
	node.cfv_infset.row(P1) = node.cf_values.row(P1).matrix().dot(node.ranges.row(P1).matrix());
	node.cfv_infset.row(P2) = node.cf_values.row(P2).matrix().dot(node.ranges.row(P2).matrix());

	//--compute CFV - BR values weighted by the reach prob
	node.cfv_br_infset = ArrayX(players_count);
	node.cfv_br_infset.row(P1) = node.cf_values_br.row(P1).matrix().dot(node.ranges.row(P1).matrix());
	node.cfv_br_infset.row(P2) = node.cf_values_br.row(P2).matrix().dot(node.ranges.row(P2).matrix());
	
	node.epsilon = node.cfv_br_infset - node.cfv_infset;
	node.exploitability = node.epsilon.mean();
}

void tree_values::compute_values(Node& root, ArrayXX* starting_ranges)
{
	ArrayXX range;

	//--1.0 set the starting range
	if (starting_ranges == nullptr)
	{
		range = ArrayXX::Constant(players_count, card_count, 1.0f / card_count);
	}
	else
	{
		range = *starting_ranges;
	}

	//--2.0 check the starting ranges
#ifdef _DEBUG

	ArrayX checksum = range.rowwise().sum();
	assert(abs(checksum(P1) - 1) < 0.0001 && "starting range does not sum to 1");
	assert(abs(checksum(P2) - 1) < 0.0001 && "starting range does not sum to 1");
	assert((range >= 0).all());
#endif

	//--3.0 compute the values
	_fill_ranges_dfs(root, range);
	_compute_values_dfs(root);
}
