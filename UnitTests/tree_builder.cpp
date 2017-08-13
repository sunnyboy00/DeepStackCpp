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
	root_node.bets = T2f();
	root_node.bets << 100, 200;

	tree_builder builder;
	vector<Node*> result = builder._get_children_nodes_transition_call(root_node);
	Node* newNode = result[0];
	REQUIRE(newNode->current_player == chance);
	delete(newNode);
}

TEST_CASE("_get_children_nodes_chance_node")
{
	card_to_string_conversion converter;
	Node root_node;
	root_node.board = converter.string_to_board("");
	root_node.street = 1;
	root_node.current_player = chance;
	root_node.bets = T2f();
	root_node.bets << 100, 100;

	tree_builder builder;
	vector<Node*> result = builder._get_children_nodes_chance_node(root_node);
	REQUIRE(result.size() == 6);
}

TEST_CASE("_get_children_player_node")
{
	card_to_string_conversion converter;
	Node root_node;
	root_node.board = converter.string_to_board("");
	root_node.street = 1;
	root_node.current_player = P2;
	root_node.bets = T2f();
	root_node.bets << 100, 100;

	tree_builder builder;
	vector<Node*> result = builder._get_children_player_node(root_node);
	REQUIRE(result.size() == 4);

	root_node.board = converter.string_to_board("Qs");
	root_node.street = 2;
	root_node.current_player = P1;
	root_node.bets = T2f();
	root_node.bets << 100, 200;

	result = builder._get_children_player_node(root_node);
	REQUIRE(result.size() == 4);
}

TEST_CASE("_get_children_nodes")
{
	card_to_string_conversion converter;
	Node root_node;
	root_node.board = converter.string_to_board("");
	root_node.street = 1;
	root_node.current_player = P1;
	root_node.bets << 300, 300;

	tree_builder builder;
	builder._build_tree_dfs(root_node);
	REQUIRE(TRUE);
}
