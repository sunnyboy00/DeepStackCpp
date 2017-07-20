//#include "TreeCFR.h"
//
//
//
//TreeCFR::TreeCFR()
//{
//}
//
//terminal_equity* TreeCFR::_get_terminal_equity(Node& node)
//{
//	auto it = _cached_terminal_equities.find(&node.board);
//
//	terminal_equity* cached = nullptr;
//	if (it == _cached_terminal_equities.end())
//	{
//		cached = new terminal_equity();
//		cached->set_board(node.board);
//		_cached_terminal_equities[&node.board] = cached;
//	}
//	else
//	{
//		cached = it->second;
//	}
//
//	return cached;
//}
//
//void TreeCFR::cfrs_iter_dfs(Node& node, long long iter)
//{
//	assert(node.current_player == P1 || node.current_player == P2 || node.current_player == chance);
//
//	int currentPlayerNorm = node.current_player - 1;
//	int opponentIndexNorm = 2 - node.current_player;
//
//	//--dimensions in tensor
//	static const int action_dimension = 0;
//	static const int card_dimension = 1;
//
//	//--compute values using terminal_equity in terminal nodes
//	if (node.terminal)
//	{
//
//		terminal_equity* termEquity = _get_terminal_equity(node);
//
//		//MatrixXf values = MatrixXf(node.ranges_absolute);
//		memcpy(node.cf_values.data(), node.ranges_absolute.data(), node.ranges_absolute.size() * sizeof(float));
//
//		node.cf_values.fill(0); //ToDo: performance do we need this?
//
//		if (node.node_type == terminal_fold)
//		{
//			termEquity->tree_node_fold_value(node.ranges_absolute, node.cf_values, opponentIndexNorm);
//		}
//		else
//		{
//			termEquity->tree_node_call_value(node.ranges_absolute, node.cf_values);
//		}
//
//		//--multiply by the pot
//		node.cf_values *= node.pot;
//		node.cf_values.conservativeResize(node.ranges_absolute.rows(), node.ranges_absolute.cols());
//			
//	}
//	else
//	{
//
//		int actions_count = node.children.size();
//
//		ArrayXXf current_strategy;
//
//		if (node.current_player == chance)
//		{
//			current_strategy = node.strategy;
//		}
//		else
//		{
//			//--we have to compute current strategy at the beginning of each iteration
//
//			//--initialize regrets in the first iteration
//			if (node.regrets.size() == 0)
//			{
//				node.regrets = MatrixXf(actions_count, card_count);
//				node.regrets.fill(regret_epsilon);
//			}
//
//			if (node.regrets.size() == 0)
//			{
//				node.possitive_regrets = MatrixXf(actions_count, card_count);
//				node.possitive_regrets.fill(regret_epsilon);
//			}
//
//			//--compute positive regrets so that we can compute the current strategy from them
//			memcpy(node.possitive_regrets.data(), node.regrets.data(), node.regrets.size() * sizeof(float));
//			node.possitive_regrets = (node.possitive_regrets.array() >= regret_epsilon).select(
//				node.possitive_regrets,
//				MatrixXd::Zero(node.possitive_regrets.rows(), node.possitive_regrets.cols())
//			);
//
//			//--compute the current strategy
//			float regrets_sum = node.possitive_regrets.row(action_dimension).sum();
//			current_strategy = ArrayXXf(node.possitive_regrets);
//			current_strategy /= regrets_sum;
//		}
//
//		//--current cfv[[actions, players, ranges]]
//		Tensor<float, 3> cf_values_allactions(actions_count, players_count, card_count);
//		cf_values_allactions.setZero();
//
//		Array2Xf children_ranges_absolute;
//
//		if (node.current_player == chance)
//		{
//			Array2Xf ranges_mul_matrix = node.ranges_absolute.row(0).replicate(actions_count, 1);
//			children_ranges_absolute.row(0) = current_strategy * ranges_mul_matrix;
//
//			ranges_mul_matrix = node.ranges_absolute.row(1).replicate(actions_count, 1);
//			children_ranges_absolute.row(1) = current_strategy * ranges_mul_matrix;
//		}
//		else
//		{
//			Array2Xf ranges_mul_matrix = node.ranges_absolute.row(currentPlayerNorm).replicate(actions_count, 1);
//			children_ranges_absolute.row(currentPlayerNorm) = current_strategy * ranges_mul_matrix;
//			children_ranges_absolute.row(currentPlayerNorm) = node.ranges_absolute.row(opponentIndexNorm).replicate(actions_count, 1); // I have removed clone ToDo: check
//		}
//
//		for (size_t i = 0; i < node.children.size(); i++)
//		{
//			Node* child_node = node.children[i];
//			//--set new absolute ranges(after the action) for the child
//			child_node->ranges_absolute = Array2Xf(node.ranges_absolute);
//
//			child_node->ranges_absolute.row(0) = children_ranges_absolute.row(0)(i);
//			child_node->ranges_absolute.row(1) = children_ranges_absolute.row(1)(i);
//			cfrs_iter_dfs(*child_node, iter);
//			cf_values_allactions.chip(i, 0) = child_node->cf_values;
//		}
//
//		node.cf_values = MatrixXf(players_count, card_count);
//		node.cf_values.fill(0);
//
//		if (node.current_player != chance)
//		{
//			current_strategy.conservativeResize(actions_count, card_count);
//			//ArrayXXf strategy_mul_matrix = current_strategy; //ToDo: remove or add copy
//			Eigen::Tensor<float, 2> playerCf = cf_values_allactions.chip(0, currentPlayerNorm);
//			Map<ArrayXXf> plCfAr(playerCf.data(), playerCf.dimension(0), playerCf.dimension(1)); //Or just convert (ArrayXXf)?
//			ArrayXXf tempRes = current_strategy * plCfAr; // strategy_mul_matrix * plCfAr
//			node.cf_values.row(currentPlayerNorm) = tempRes.colwise().sum();
//
//			Eigen::Tensor<float, 2> opCf = cf_values_allactions.chip(0, opponentIndexNorm);
//			Map<ArrayXXf> opCfAr(opCf.data(), opCf.dimension(0), opCf.dimension(1)); 
//			node.cf_values.row(opponentIndexNorm) = opCfAr.colwise().sum();
//		}
//		else
//		{
//			node.cf_values[1] = (cf_values_allactions[{ {}, 1, {}}]) : sum(1)
//				node.cf_values[2] = (cf_values_allactions[{ {}, 2, {}}]) : sum(1)
//		}
//		
//								if node.current_player ~= constants.players.chance {
//									//--computing regrets
//									local current_regrets = cf_values_allactions[{ {}, { node.current_player }, {}}] : reshape(actions_count, game_settings.card_count) : clone()
//									current_regrets : csub(node.cf_values[node.current_player]:view(1, game_settings.card_count) : expandAs(current_regrets))
//
//									self : update_regrets(node, current_regrets)
//
//									//--accumulating average strategy
//									self : update_average_strategy(node, current_strategy, iter)
//		}
//	}
//}
//
