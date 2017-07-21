#include "TreeCFR.h"
#include <iostream>
#include <string>


TreeCFR::TreeCFR()
{
}

terminal_equity* TreeCFR::_get_terminal_equity(Node& node)
{
	auto it = _cached_terminal_equities.find(&node.board);

	terminal_equity* cached = nullptr;
	if (it == _cached_terminal_equities.end())
	{
		cached = new terminal_equity();
		cached->set_board(node.board);
		_cached_terminal_equities[&node.board] = cached;
	}
	else
	{
		cached = it->second;
	}

	return cached;
}

void TreeCFR::run_cfr(Node& root, const ArrayXXf& starting_ranges, size_t iter_count)
{
	assert(starting_ranges.size() > 0);
	iter_count = iter_count > 0 ? iter_count : cfr_iters;
	root.ranges_absolute = starting_ranges;

	for (size_t iter = 0; iter < iter_count; iter++)
	{
		cfrs_iter_dfs(root, iter);
	}
}

void TreeCFR::update_regrets(Node& node, const ArrayXXf& current_regrets)
{
	//--node.regrets:add(current_regrets)
	//	--local negative_regrets = node.regrets[node.regrets:lt(0)]
	//	--node.regrets[node.regrets:lt(0)] = negative_regrets
	node.regrets.array() += current_regrets;

	node.regrets = (node.regrets.array() >= regret_epsilon).select(
		node.regrets,
		MatrixXd::Constant(node.possitive_regrets.rows(), node.possitive_regrets.cols(), regret_epsilon));
}

void TreeCFR::update_average_strategy(Node& node, ArrayXXf& current_strategy, size_t iter)
{
	if (iter > cfr_skip_iters)
	{
		if (node.strategy.size() == 0)
		{
			int actions_count = (int)node.children.size();
			node.strategy = ArrayXXf(actions_count, card_count);
			node.strategy.fill(0);
		}

		if (node.iter_weight_sum.size() == 0)
		{
			int actions_count = (int)node.children.size();
			node.iter_weight_sum = ArrayXf(card_count);
			node.iter_weight_sum.fill(0);
		}

		ArrayXf iter_weight_contribution = node.ranges_absolute.row(node.current_player); // Copy?
		Util::ClipLow(node.iter_weight_contribution, regret_epsilon);
		node.iter_weight_sum += iter_weight_contribution;

		ArrayXf iter_weight = iter_weight_contribution / node.iter_weight_sum;
		iter_weight.conservativeResize(1, card_count);
		ArrayXXf expanded_weight = Util::ExpandAs(iter_weight, node.strategy);
		ArrayXXf old_strategy_scale = (expanded_weight * (-1)) + 1; //--same as 1 - expanded weight
		node.strategy *= old_strategy_scale;
		ArrayXXf strategy_addition = current_strategy  * expanded_weight;
		node.strategy += strategy_addition;
	}
}

void TreeCFR::cfrs_iter_dfs(Node& node, size_t iter)
{
	assert(node.current_player == P1 || node.current_player == P2 || node.current_player == chance);

	int currentPlayerNorm = node.current_player - 1;
	int opponnent = 3 - node.current_player;
	int opponentIndexNorm = opponnent - 1;

	//--dimensions in tensor
	static const int action_dimension = 0;
	static const int card_dimension = 1;

	//--compute values using terminal_equity in terminal nodes
	if (node.terminal)
	{

		terminal_equity* termEquity = _get_terminal_equity(node);

		node.cf_values = MatrixXf(node.ranges_absolute);
		node.cf_values.fill(0);

		if (node.node_type == terminal_fold)
		{
			termEquity->tree_node_fold_value(node.ranges_absolute, node.cf_values, opponnent);
		}
		else
		{
			termEquity->tree_node_call_value(node.ranges_absolute, node.cf_values);
		}

		//--multiply by the pot
		node.cf_values *= node.pot;
		node.cf_values.conservativeResize(node.ranges_absolute.rows(), node.ranges_absolute.cols());

	}
	else
	{
		int actions_count = (int)node.children.size();

		ArrayXXf current_strategy;

		if (node.current_player == chance)
		{
			current_strategy = node.strategy;
		}
		else
		{
			//--we have to compute current strategy at the beginning of each iteration

			//--initialize regrets in the first iteration
			if (node.regrets.size() == 0)
			{
				node.regrets = MatrixXf(actions_count, card_count);
				node.regrets.fill(regret_epsilon);
			}

			if (node.possitive_regrets.size() == 0)
			{
				node.possitive_regrets = MatrixXf(actions_count, card_count);
				node.possitive_regrets.fill(regret_epsilon);
			}

			//--compute positive regrets so that we can compute the current strategy from them
			//memcpy(node.possitive_regrets.data(), node.regrets.data(), node.regrets.size() * sizeof(float));
			Util::CopyTo(node.possitive_regrets, node.regrets);
			Util::ClipLow(node.possitive_regrets, regret_epsilon);

			//--compute the current strategy
			ArrayXXf regrets_sum = node.possitive_regrets.colwise().sum().row(action_dimension);
			current_strategy = ArrayXXf(node.possitive_regrets);
			current_strategy /= Util::ExpandAs(regrets_sum, current_strategy);
		}

		//--current cfv[[actions, players, ranges]]
		Tensor<float, 3> cf_values_allactions(actions_count, players_count, card_count);
		const int PLAYERS_DIM = 1;

		cf_values_allactions.setZero();

		map <int, ArrayXXf> children_ranges_absolute;

		if (node.current_player == chance)
		{
			ArrayXXf ranges_mul_matrix = node.ranges_absolute.row(0).replicate(actions_count, 1);
			children_ranges_absolute[0] = current_strategy * ranges_mul_matrix;

			ranges_mul_matrix = node.ranges_absolute.row(1).replicate(actions_count, 1);
			children_ranges_absolute[1] = current_strategy * ranges_mul_matrix;
		}
		else
		{
			ArrayXXf ranges_mul_matrix = node.ranges_absolute.row(currentPlayerNorm).replicate(actions_count, 1);
			children_ranges_absolute[currentPlayerNorm] = current_strategy.array() * ranges_mul_matrix.array();
			children_ranges_absolute[opponentIndexNorm] = node.ranges_absolute.row(opponentIndexNorm).replicate(actions_count, 1); // I have removed clone ToDo: check
		}

		for (size_t i = 0; i < node.children.size(); i++)
		{
			Node* child_node = node.children[i];
			//--set new absolute ranges(after the action) for the child
			child_node->ranges_absolute = ArrayXXf(node.ranges_absolute);

			child_node->ranges_absolute.row(0) = children_ranges_absolute[0](i);
			child_node->ranges_absolute.row(1) = children_ranges_absolute[1](i);
			cfrs_iter_dfs(*child_node, iter);
			cf_values_allactions.chip(i, 0) = Util::ToTensor(child_node->cf_values);
		}

		node.cf_values = MatrixXf(players_count, card_count);
		node.cf_values.fill(0);

		Tensor<float, 2> tempVar;

		if (node.current_player != chance)
		{
			current_strategy.conservativeResize(actions_count, card_count);
			//ArrayXXf strategy_mul_matrix = current_strategy; //ToDo: remove or add copy
			Map<ArrayXXf> plCfAr = Util::TensorToArray2d(cf_values_allactions, currentPlayerNorm, PLAYERS_DIM, tempVar);
			ArrayXXf tempRes = current_strategy * plCfAr; // strategy_mul_matrix * plCfAr
			node.cf_values.row(currentPlayerNorm) = tempRes.colwise().sum();

			Map<ArrayXXf> opCfAr = Util::TensorToArray2d(cf_values_allactions, opponentIndexNorm, PLAYERS_DIM, tempVar);
			node.cf_values.row(opponentIndexNorm) = opCfAr.colwise().sum();
		}
		else
		{
			node.cf_values.row(0) = Util::TensorToArray2d(cf_values_allactions, 0, PLAYERS_DIM, tempVar).colwise().sum();
			node.cf_values.row(1) = Util::TensorToArray2d(cf_values_allactions, 1, PLAYERS_DIM, tempVar).colwise().sum();
		}

		if (node.current_player != chance)
		{
			//--computing regrets

			Map<ArrayXXf> current_regrets = Util::TensorToArray2d(cf_values_allactions, currentPlayerNorm, PLAYERS_DIM, tempVar);
			current_regrets.resize(actions_count, card_count);

			ArrayXXf tempRes = node.cf_values.row(currentPlayerNorm);
			tempRes.resize(1, card_count);
			MatrixXf matrixToDiv = tempRes.replicate(current_regrets.rows(), 1);
			current_regrets -= matrixToDiv.array();
			update_regrets(node, current_regrets);

			//--accumulating average strategy
			update_average_strategy(node, current_strategy, iter);
		}
	}
}




