// IntegrationTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma once
#include "IntegrationTests.h"
#include "TreeBuilderParams.h"
#include "card_to_string_conversion.h"
#include "Node.h"
#include "tree_values.h"
#include "Resolving.h"
#include "Constants.h"
#include "tree_builder.h"
#include "TreeVisualizer.h"
#include "data_generation.h"
#include "lookahead.h"
#include "TreeCFR.h"
#include <Eigen/Dense>
#include <iostream>
#include <ctime>
#include "TreeLookahed.h"
#include "TreeLookahead.h"

void test_tree_builder()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets = Array2();
	params.root_node->bets << 1200, 1200;

	tree_builder builder;
	Node* tree = builder.build_tree(params);
}

void test_tree_visualiser()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 800, 800;

	tree_builder builder;
	Node* tree = builder.build_tree(params);

	TreeVisualizer visualizer;
	visualizer.graphviz(*tree, "big_tree");
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
	Node* tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXX starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(*tree, starting_ranges, 100000, 499);

	tree_values tv;
	tv.compute_values(*tree, &starting_ranges);
	cout << "Exploitability: " << tree->exploitability << endl;
}

void test_run_cfr()
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node* tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXX starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(*tree, starting_ranges);
}

void Resolve()
{
	Resolving resolver;

	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P1;
	node.bets << 800, 800;

	card_tools tools;
	ArrayX player_range = tools.get_uniform_range(node.board);
	ArrayX op_cfvs(card_count);
	op_cfvs(0) = -500;
	op_cfvs(1) = 0;
	op_cfvs(2) = 700;
	op_cfvs(3) = -900;
	op_cfvs(4) = 800;
	op_cfvs(5) = 1200;

	LookaheadResult result = resolver.resolve(node, player_range, op_cfvs);
}

void GenerateData()
{
	data_generation generator;
	generator.generate_data(train_data_count, valid_data_count);
}


void TestTreeLookahed()
{
	Resolving resolver;
	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P1;
	node.bets << 10, 10;

	card_tools tools;
	Range player_range = tools.get_uniform_range(node.board);
	Range op_cfvs(card_count);
	op_cfvs.setZero();
	op_cfvs <<
		-500,
		0,
		700,
		-900,
		800,
		1200;

	resolver._create_lookahead_tree(node);
	TreeLookahed* look = new TreeLookahed(*resolver._lookahead_tree, 500, 10000);
	Range p = Range(player_range);
	Range o = Range(op_cfvs);

	look->_reconstruction_gadget = new cfrd_gadget(look->_root->board, p, o);
	look->_root->ranges.row(P1) = player_range;
	look->_reconstruction_opponent_cfvs = op_cfvs;

	look->_reconstruction = true;
	look->_compute();
	LookaheadResult result = look->get_results();

}

int main()
{
	clock_t begin = clock();
	TestTreeLookahed();
	//test_tree_visualiser();
int main()
{
	clock_t begin = clock();
	//test_tree_visualiser();
	//test_run_cfr();
	//test_tree_visualiser();
	//Resolve();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed_secs << endl;
}

