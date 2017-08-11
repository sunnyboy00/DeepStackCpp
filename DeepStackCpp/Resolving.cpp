#include "Resolving.h"



Resolving::Resolving()
{
	_lookBuilder = new LookaheadBuilder(_lookahead);
}


Resolving::~Resolving()
{
	delete(_lookBuilder);
}

void Resolving::_create_lookahead_tree(Node & node)
{
	TreeBuilderParams build_tree_params;
	build_tree_params.root_node = &node;
	build_tree_params.limit_to_street = true;

	_lookahead_tree = builder.build_tree(build_tree_params);
}

LookaheadResult Resolving::resolve_first_node(Node& node, const Tf1& player_range, const Tf1& opponent_range)
{
	_create_lookahead_tree(node);
	_lookBuilder->build_from_tree(node);

	_lookahead.resolve_first_node(player_range, opponent_range);

	_resolve_results = _lookahead.get_results();
	return _resolve_results;
}

LookaheadResult Resolving::resolve(Node& node, Tf1& player_range, Tf1& opponent_cfvs)
{
	assert(_cardTools.is_valid_range(ToAmxx(player_range), node.board));
	_create_lookahead_tree(node);

	_lookBuilder->build_from_tree(_lookahead_tree);
	_lookahead.resolve(player_range, opponent_cfvs);
	_resolve_results = _lookahead.get_results();
	return _resolve_results;
}

ArrayXf Resolving::get_possible_actions()
{
	return _lookahead_tree.actions;
}

Tf1 Resolving::get_root_cfv()
{
	return _resolve_results.root_cfvs;
}

Tf2 Resolving::get_root_cfv_both_players()
{
	return _resolve_results.root_cfvs_both_players;
}

Tf1 Resolving::get_action_cfv(int action)
{
	int action_id = _action_to_action_id(action);
	return _resolve_results.children_cfvs.chip(action_id, 0);
}

Tf1 Resolving::get_chance_action_cfv(int action, Tf1 board)
{
	int action_id = _action_to_action_id(action);
	return _lookahead.get_chance_action_cfv(action_id, board);
}

Tf1 Resolving::get_action_strategy(int action)
{
	int action_id = _action_to_action_id(action);
	return _resolve_results.strategy.chip(action_id, 0);
}

int Resolving::_action_to_action_id(int action)
{
	ArrayXf actions = get_possible_actions();
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

