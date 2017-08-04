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
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 300, 900;

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
	test_tree_visualiser();

	Tensor<float, 5> test(2, 1, 1, 2, 6);
	RemoveF3D(test, 0, 0, 0).setConstant(1);
	//test_tree_visualiser();
	//// Shuffle all dimensions to the left by 1.
	//Tensor<float, 5> i2(2, 2, 2, 2, 2);
	//std::array<DenseIndex, 5> ssss = { 0, 1, 3, 2, 4 };
	//Tensor<float, 5> output33 = i2.shuffle({ { 0, 1, 3, 2, 4 } });
	//Tensor<float, 5> output2 = Util::Transpose(i2, ssss);

	//Tensor<float, 2> input(2, 2);
	//Tensor<float, res> currentRes = input.chip(0, 0);

	//Util::MultiSliceFill(input, { 0 }, { 0, 1 }, 8);
//	int offset = Util::ConvertOffset(input, -1, 0);
	//Remove2D(input, 0).setConstant(1);
	cout << test;

	//Tensor<float, 5> output = Util::Transpose(input, { 0, 2, 1, 3, 4});

	//// ... set some values in input.
	//std::array<DenseIndex, 5> test = { 0, 1, 3, 2, 4 };
	////Eigen::array<DenseIndex, 5> ff = test;
	//Tensor<float, 5> output2 = Util::Transpose(input, { 0, 1, 3, 2, 4 });
	//Tensor<float, 5> output = input.shuffle(test);

	//eigen_assert(output.dimension(0) == 20);
	//eigen_assert(output.dimension(1) == 50);
	//eigen_assert(output.dimension(2) == 30);

	//Eigen::Tensor<float, 2> a(2, 1);
	//a.setConstant(1);
	//Eigen::Tensor<float, 2> b(2, 1);

	////a.setZero();
	////Eigen::array<int, 2> action_dims_2{ { 2, 2 } };
	//Tensor<bool, 0> eq = (a < Eigen::Tensor<float, 2>(a).setZero()).all();
	//bool rr = eq.coeff();

	//std::array<DenseIndex, 2> ar = { 2, 2 };
	//cout << a;
	cout << "Done";

	//Eigen::array<Index, 2> dd(ar);



	//Util::ResizeAndFill(a, ar);
	//auto s = a.size();
	//cout << m;
	//cout << m;


	//Eigen::Tensor<int, 2> a(4, 3);
	//a.setValues({ { 0, 100, 200 },{ 300, 400, 500 },
	//{ 600, 700, 800 },{ 900, 1000, 1100 } });
	//Eigen::Tensor<int, 1> row_3 = a.chip(2, 0);
	//Eigen::Tensor<int, 1> col_2 = a.chip(1, 1);
	//cout << "a" << endl << a << endl;
	//cout << "row_3" << endl << row_3 << endl;
	//cout << "col_2" << endl << col_2 << endl;

	//int d1 = 4;
	//int d2 = 3;
	//int d3 = 3;

	//Eigen::Tensor<float, 3> tensor(d1, d2, d3);

	//float koef = 0;
	//for (int i = 0; i < d1; ++i) {
	//	for (int j = 0; j < d2; ++j) {
	//		for (int k = 0; k < d3; ++k) {
	//			tensor(i, j, k) = koef;
	//			koef++;
	//		}
	//	}
	//}
	//
	//Tensor<float, 2> playerCf = tensor.chip(0, 1);
	//Map<ArrayXXf> plCfAr(playerCf.data(), playerCf.dimension(0), playerCf.dimension(1));
	//Tensor<float, 2> temp;

	//Map<ArrayXXf> map = Util::TensorToArray2d(tensor, 1, temp);
	////Tensor<float, 2> playerCf = tensor.chip(0, 1);

	////Map<ArrayXXf> plCfAr(playerCf.data(), playerCf.dimension(0), playerCf.dimension(1));

	////auto res = Reduce3dTensor2(tensor, 0);
	//cout << playerCf << endl;
	////cout << res;

	//RowVectorXf data(6);
	//data << 0, 1, 2, 3, 4, 5;
	//ArrayXXf as(4, 6);

	//ArrayXXf res = data.replicate(2, 4);
//	cout << map;
	//test_tree_cfr();
	cout << "Done";
	//float target[6] = { -1, 1, 5, 5, 6, 6 };

	//float array[40];
	//for (int i = 0; i < 40; ++i) array[i] = i;
	//typedef Matrix<float, Dynamic, Dynamic, RowMajor> MatrixType;

	//Vector2f test;
	//test << 1, 2;

	//MatrixType res = test.replicate(1, 2);

	//cout << res;

	//MatrixType M1(3, 3);
	//M1 << 1, 2, 3,
	//	4, 5, 6,
	//	7, 8, 9;



	//Map<MatrixType, 0, Stride<0, 0>> zeroMap(M1.data(), 2, 2);  
	//Map<MatrixType, 0, Stride<2, 0>> oneMap(M1.data(), 2, 2);  
	//cout << "Row stride = 0:" << "\n";
	//cout << zeroMap << "\n" << "Row stride = 2:" << "\n";
	//cout << oneMap;
	//cout << "\n" << "Done";
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

