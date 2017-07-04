#include "tree_builder.h"


tree_builder::tree_builder()
{
	_card_tools = card_tools();
	_card_to_string = card_to_string_conversion();

}

vector<unique_ptr<Node>> tree_builder::_get_children_nodes_transition_call(Node& parent_node)
{
	unique_ptr<Node> chance_node = make_unique<Node>(parent_node);
	chance_node->current_player = chance;
	vector<unique_ptr<Node>> out = { std::move(chance_node) };
	return out;
}

vector<unique_ptr<Node>> tree_builder::_get_children_nodes_chance_node(Node & parent_node)
{
	assert(parent_node.current_player == chance);

	if (_limit_to_street)
	{
		return vector<unique_ptr<Node>>();
	}

	MatrixXf next_boards = _card_tools.get_second_round_boards();
	long long next_boards_count = next_boards.rows();

	long long subtree_height = -1;
	auto children = vector<unique_ptr<Node>>();
	//--1.0 iterate over the next possible boards to build the corresponding subtrees
	for (unsigned long long i = 1; next_boards_count; i++)
	{
		CardArray next_board = next_boards.row(i);
		std::string next_board_string = _card_to_string.cards_to_string(next_board);
		
		//Node child;

		//child.node_type = inner_node;
		//child.parent = parent_node;
		//	child.current_player = constants.players.P1;
		//	child.street = parent_node.street + 1;
		//		child.board = next_board;
		//		child.board_string = next_board_string;
		//			child.bets = parent_node.bets:clone();

		//			table.insert(children, child);
	}
}


