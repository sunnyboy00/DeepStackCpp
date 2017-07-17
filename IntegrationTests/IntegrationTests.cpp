// IntegrationTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma once
#include "IntegrationTests.h"
#include "TreeBuilderParams.h"
#include "card_to_string_conversion.h"
#include "Node.h"
#include "Constants.h"
#include <Eigen/Dense>
#include "tree_builder.h"
#include "TreeVisualizer.h"
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

int main()
{
	//float target[6] = { -1, 1, 5, 5, 6, 6 };

	//float array[40];
	//for (int i = 0; i < 40; ++i) array[i] = i;
	typedef Matrix<float, Dynamic, Dynamic, RowMajor> MatrixType;

	Vector2f test;
	test << 1, 2;

	MatrixType res = test.replicate(1, 2);

	cout << res;

	MatrixType M1(3, 3);
	M1 << 1, 2, 3,
		4, 5, 6,
		7, 8, 9;



	Map<MatrixType, 0, Stride<0, 0>> zeroMap(M1.data(), 2, 2);  
	Map<MatrixType, 0, Stride<2, 0>> oneMap(M1.data(), 2, 2);  
	cout << "Row stride = 0:" << "\n";
	cout << zeroMap << "\n" << "Row stride = 2:" << "\n";
	cout << oneMap;
	cout << "\n" << "Done";
	//cout << m2map;

	//MapTypeConst m2mapconst(p, m2.size());  // a read-only accessor for m2


	//int array[9];
	//for (int i = 0; i < 9; ++i) array[i] = i;
	//auto m = Map<MatrixType>(array, 1);

	//MatrixXf test(3, 3);
	//test << 1, 2, 3;
	//test.resize(1, 3);
	//test.resize(3, 1);
	//test_tree_visualiser();
}

