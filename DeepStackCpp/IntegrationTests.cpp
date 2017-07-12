#pragma once
#include "IntegrationTests.h"
#include "TreeBuilderParams.h"
#include "card_to_string_conversion.h"
#include "Node.h"
#include "Constants.h"
#include <Eigen/Dense>
#include "tree_builder.h"
#include "TreeVisualizer.h"

void IntegrationTests::test_tree_builder()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets = Eigen::Array2f();
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
}

void IntegrationTests::test_tree_visualiser()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets = Eigen::Array2f();
	params.root_node->bets << 300, 300;

	tree_builder builder;
	Node& tree = builder.build_tree(params);

	TreeVisualizer visualizer;
	visualizer.graphviz(tree, "simple_tree");
}
