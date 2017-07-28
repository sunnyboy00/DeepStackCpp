#include "Resolving.h"



Resolving::Resolving()
{
}


Resolving::~Resolving()
{
}

void Resolving::_create_lookahead_tree(Node & node)
{
	TreeBuilderParams build_tree_params;
	build_tree_params.root_node = &node;
	build_tree_params.limit_to_street = true;

	Node& root = builder.build_tree(build_tree_params);
}

void Resolving::resolve_first_node(Node& node, CardArray player_range, CardArray opponent_range)
{
//self:_create_lookahead_tree(node)
//
//	self.lookahead = Lookahead()
//	self.lookahead : build_lookahead(self.lookahead_tree)
//
//	self.lookahead : resolve_first_node(player_range, opponent_range)
//
//	self.resolve_results = self.lookahead : get_results()
//	return self.resolve_results
}
