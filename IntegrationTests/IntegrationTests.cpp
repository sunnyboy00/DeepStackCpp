// IntegrationTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma once
#include "IntegrationTests.h"
#include "TreeBuilderParams.h"
#include "card_to_string_conversion.h"
#include "Node.h"
#include "tree_values.h"
#include "Constants.h"
#include "tree_builder.h"
#include "TreeVisualizer.h"
#include "TreeCFR.h"
#include <Eigen/Dense>
#include <iostream>


void test_tree_builder()
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

void test_tree_visualiser()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);

	TreeVisualizer visualizer;
	visualizer.graphviz(tree, "big_tree");
}

void test_tree_cfr()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 100000, 499);

	tree_values tv;
	tv.compute_values(tree, &starting_ranges);
	cout << "Exploitability: " << tree.exploitability << endl;
}


int main()
{
	Tf2 a1(4, 3);
	a1.setZero();

	Tf2 a2(4, 3);
	a2.setValues({ { 0, 100, 200 },{ 300, 400, 500 },
	{ 600, 700, 800 },{ 900, 1000, 1100 } });

	Tf2& a3 = a1;

	cout << "a1" << endl << a1.data() << endl;
	cout << "a3" << endl << a3.data() << endl;

	cout << "Done";
}

