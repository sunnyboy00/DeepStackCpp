#include "TreeCFR.h"



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

inline Map<ArrayXXf> TreeCFR::Reduce3dTensor(Tensor<float, 3>& tensor, int dim)
{
	Tensor<float, 2> playerCf = tensor.chip(0, dim);
	Map<ArrayXXf> plCfAr(playerCf.data(), playerCf.dimension(0), playerCf.dimension(1)); //Or just convert (ArrayXXf)?
	return plCfAr;
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
	int opponentIndexNorm = 2 - node.current_player;

	//--dimensions in tensor
	static const int action_dimension = 0;
	static const int card_dimension = 1;

	//--compute values using terminal_equity in terminal nodes
	if (node.terminal)
	{

		terminal_equity* termEquity = _get_terminal_equity(node);

		//MatrixXf values = MatrixXf(node.ranges_absolute);
		memcpy(node.cf_values.data(), node.ranges_absolute.data(), node.ranges_absolute.size() * sizeof(float));

		node.cf_values.fill(0); //ToDo: performance do we need this?

		if (node.node_type == terminal_fold)
		{
			termEquity->tree_node_fold_value(node.ranges_absolute, node.cf_values, opponentIndexNorm);
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
		cf_values_allactions.setZero();

		Array2Xf children_ranges_absolute;

		if (node.current_player == chance)
		{
			Array2Xf ranges_mul_matrix = node.ranges_absolute.row(0).replicate(actions_count, 1);
			children_ranges_absolute.row(0) = current_strategy * ranges_mul_matrix;

			ranges_mul_matrix = node.ranges_absolute.row(1).replicate(actions_count, 1);
			children_ranges_absolute.row(1) = current_strategy * ranges_mul_matrix;
		}
		else
		{
			ArrayXXf ranges_mul_matrix = node.ranges_absolute.row(currentPlayerNorm).replicate(actions_count, 1);
			children_ranges_absolute.row(currentPlayerNorm) = current_strategy * ranges_mul_matrix;
			auto sm = node.ranges_absolute.row(0);
			auto res = node.ranges_absolute.row(opponentIndexNorm).replicate(actions_count, 1);
			children_ranges_absolute.row(currentPlayerNorm) = ArrayXXf(res); // I have removed clone ToDo: check
		}

		for (size_t i = 0; i < node.children.size(); i++)
		{
			Node* child_node = node.children[i];
			//--set new absolute ranges(after the action) for the child
			child_node->ranges_absolute = ArrayXXf(node.ranges_absolute);

			child_node->ranges_absolute.row(0) = children_ranges_absolute.row(0)(i);
			child_node->ranges_absolute.row(1) = children_ranges_absolute.row(1)(i);
			cfrs_iter_dfs(*child_node, iter);
			cf_values_allactions.chip(i, 0) = Util::ToTensor(child_node->cf_values);
		}

		node.cf_values = MatrixXf(players_count, card_count);
		node.cf_values.fill(0);

		if (node.current_player != chance)
		{
			current_strategy.conservativeResize(actions_count, card_count);
			//ArrayXXf strategy_mul_matrix = current_strategy; //ToDo: remove or add copy

			Map<ArrayXXf> plCfAr = Reduce3dTensor(cf_values_allactions, currentPlayerNorm);
			ArrayXXf tempRes = current_strategy * plCfAr; // strategy_mul_matrix * plCfAr
			node.cf_values.row(currentPlayerNorm) = tempRes.colwise().sum();

			Map<ArrayXXf> opCfAr = Reduce3dTensor(cf_values_allactions, opponentIndexNorm);
			node.cf_values.row(opponentIndexNorm) = opCfAr.colwise().sum();
		}
		else
		{
			node.cf_values.row(0) = Reduce3dTensor(cf_values_allactions, 0).colwise().sum();
			node.cf_values.row(1) = Reduce3dTensor(cf_values_allactions, 1).colwise().sum();
		}

		if (node.current_player != chance)
		{
			//--computing regrets

			Map<ArrayXXf> current_regrets = Reduce3dTensor(cf_values_allactions, currentPlayerNorm);
			current_regrets.resize(actions_count, card_count);

			VectorXf tempRes = node.cf_values.row(currentPlayerNorm);
			tempRes.resize(1, card_count);
			MatrixXf matrixToDiv = tempRes.replicate(current_regrets.rows(), current_regrets.cols());
			current_regrets -= matrixToDiv.array();
			update_regrets(node, current_regrets);

			//--accumulating average strategy
			update_average_strategy(node, current_strategy, iter);
		}
	}
}




