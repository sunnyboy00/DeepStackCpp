#include "Resolving.h"



Resolving::Resolving()
{
}


Resolving::~Resolving()
{
	if (_lookahead_tree != nullptr)
	{
		delete(_lookahead_tree);
		_lookahead_tree = nullptr;
	}

	if (_lookahead != nullptr)
	{
		delete(_lookahead);
		_lookahead = nullptr;
	}
}

void Resolving::_create_lookahead_tree(Node & node)
{
	TreeBuilderParams build_tree_params;
	build_tree_params.root_node = &node;
	build_tree_params.limit_to_street = true;

	_lookahead_tree = builder.build_tree(build_tree_params);
}

LookaheadResult Resolving::resolve_first_node(Node& node, const Range& player_range, const ArrayX& opponent_range)
{
	_create_lookahead_tree(node);
	_lookahead = new TreeLookahed(*_lookahead_tree);
	_lookahead->resolve_first_node(player_range, opponent_range);
	_resolve_results = _lookahead->get_results();
	return _resolve_results;
}

//LookaheadResult Resolving::resolve(Node& node, ArrayX& player_range, ArrayX& opponent_cfvs)
//{
//	assert(_cardTools.is_valid_range(ToAmxx(player_range), node.board));
//	_create_lookahead_tree(node);
//
//	_lookBuilder->build_from_tree(_lookahead_tree);
//	_lookahead->resolve(player_range, opponent_cfvs);
//	_resolve_results = _lookahead->get_results();
//	return _resolve_results;
//}

LookaheadResult Resolving::resolve(Node& node, ArrayX& player_range, ArrayX& opponent_cfvs, long long cfr_skip_iters, long long iters)
{
	assert(_cardTools.is_valid_range(ToAmx(player_range), node.board));
	_create_lookahead_tree(node);
	_lookahead = new TreeLookahed(*_lookahead_tree);
	_lookahead->_cfr_skip_iters = cfr_skip_iters;
	_lookahead->_cfr_iters = iters;
	_lookahead->resolve(player_range, opponent_cfvs);
	_resolve_results = _lookahead->get_results();
	return _resolve_results;
}

ArrayX Resolving::get_possible_actions()
{
	return _lookahead_tree->actions;
}

ArrayX Resolving::get_root_cfv()
{
	return _resolve_results.root_cfvs;
}

ArrayXX Resolving::get_root_cfv_both_players()
{
	return _resolve_results.root_cfvs_both_players;
}

ArrayX Resolving::get_action_cfv(int action)
{
	int action_id = _action_to_action_id(action);
	return _resolve_results.children_cfvs.row(action_id);
}

ArrayX Resolving::get_chance_action_cfv(int action, ArrayX board)
{
	int action_id = _action_to_action_id(action);
	return _lookahead->get_chance_action_cfv(action_id, board);
}

ArrayX Resolving::get_action_strategy(int action)
{
	int action_id = _action_to_action_id(action);
	return _resolve_results.strategy.row(action_id);
}

int Resolving::_action_to_action_id(int action)
{
	ArrayX actions = get_possible_actions();
	int action_id = -1;

	for (int i = 0; i < actions.size(); i++)
	{
		if(action == actions(i))
		{
			action_id = i;
		}
	}

	assert(action_id != -1);

	return action_id;
}

