#include "catch.hpp"
#include "tree_builder.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include <string>

TEST_CASE("_get_children_nodes_transition_call")
{
	card_to_string_conversion converter;
	Node root_node;
	root_node.board = converter.string_to_board("");
	root_node.street = 1;
	root_node.current_player = P1;
	root_node.bets = Eigen::Array2f();
	root_node.bets << 100, 200;

	tree_builder builder;
	vector<Node*> result = builder._get_children_nodes_transition_call(root_node);
	Node* newNode = result[0];
	REQUIRE(newNode->current_player == chance);
	delete(newNode);
}

//TEST_CASE("_get_children_nodes_chance_node")
//{
//	card_to_string_conversion converter;
//	Node root_node;
//	root_node.board = converter.string_to_board("");
//	root_node.street = 1;
//	root_node.current_player = chance;
//	root_node.bets = Eigen::Array2f();
//	root_node.bets << 100, 100;
//
//	tree_builder builder;
//	vector<Node*> result = builder._get_children_nodes_chance_node(root_node);
//	REQUIRE(result.size() == 6);
//}