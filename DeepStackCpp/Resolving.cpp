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

	Node& root = builder.build_tree(build_tree_params);
}

LookaheadResult Resolving::resolve_first_node(Node& node, const Tf1& player_range, const Tf1& opponent_range)
{
	_create_lookahead_tree(node);
	_lookBuilder->build_from_tree(node);

	_lookahead.resolve_first_node(player_range, opponent_range);

	_resolve_results = _lookahead.get_results();
	return _resolve_results;
}

void Resolving::resolve(const Node& node, const Tf1& player_range, const Tf1& opponent_cfvs)
{

}

