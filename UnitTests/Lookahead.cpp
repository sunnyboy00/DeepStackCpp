#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "LookaheadBuilder.h"
#include "Resolving.h"

#include <string>
#include <iostream>

using namespace std;

const float myEps = 0.001f;

template <int N>
void AreEq(Tf1& source, const std::array<float, N> target)
{
	assert(source.size() == N);
	for (size_t n = 0; n < N; n++)
	{
		REQUIRE(source(n) == Approx(target[n]).epsilon(myEps));
	}
}

void AreEq(Tf1& source, float scalar)
{
	for (size_t n = 0; n < source.size(); n++)
	{
		float data = source(n);
		REQUIRE(data == Approx(scalar).epsilon(myEps));
	}
}

void BuildLook(Resolving& resolver)
{
	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P2;
	node.bets << 1200, 1200;

	card_tools tools;
	Tf1 player_range = ToTmx(tools.get_uniform_range(node.board));
	Tf1 op_cfvs(card_count);
	op_cfvs.setZero();

	resolver._create_lookahead_tree(node);
	resolver._lookBuilder->build_from_tree(*resolver._lookahead_tree);

	lookahead& look = resolver._lookahead;

	look._reconstruction_gadget = new cfrd_gadget(look.tree->board, player_range, op_cfvs);
	RemoveF4D(look.ranges_data[0], 0, 0, 0, P1) = player_range;
	look._reconstruction_opponent_cfvs = op_cfvs;
}


//TEST_CASE("lookahed_set_opponent_starting_range")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//
//	std::array<float, card_count> opTarget = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
//	Tf1 opRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P2);
//	AreEq(opRrange, opTarget);
//
//	std::array<float, card_count> plTarget = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
//	Tf1 plRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P1);
//	AreEq(plRrange, plTarget);
//}
//
//TEST_CASE("lookahed_compute_current_strategies")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//	look._compute_current_strategies();
//
//	const std::array<DenseIndex, 1> dims = { card_count };
//	Tf1 ac1_str = Util::Slice(look.current_strategy_data[1], { { { 0, 0 },{ 0, -1 },{ 0, -1 },{ 0, -1 } } }).reshape(dims);
//	AreEq(ac1_str, 0.0);
//	Tf4 res = look.current_strategy_data[1];
//	Tf1 ac2_str = Util::Slice(look.current_strategy_data[1], { { { 1, 1 },{ 0, -1 },{ 0, -1 },{ 0, -1 } } }).reshape(dims);
//	AreEq(ac2_str, 1.0);
//}
//
//TEST_CASE("lookahed_compute_ranges")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//	look._compute_current_strategies();
//	look._compute_ranges();
//
//	std::array<float, card_count> opTarget = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
//	Tf1 opRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P2);
//	AreEq(opRrange, opTarget);
//
//	std::array<float, card_count> plTarget = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
//	Tf1 plRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P1);
//	AreEq(plRrange, plTarget);
//
//	//[1] ----------
//	Tf5 range1 = look.ranges_data[1];
//	std::array<float, card_count> p1_target_1 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
//	Tf1 p1_range_1 = RemoveF4D(look.ranges_data[1], 0, 0, 0, P1);
//	AreEq(p1_range_1, p1_target_1);
//
//	std::array<float, card_count> p2_target_1 = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
//	Tf1 p2_range_1 = RemoveF4D(look.ranges_data[1], 0, 0, 0, P2);
//	AreEq(p2_range_1, p2_target_1);
//
//	std::array<float, card_count> p1_target_2 = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
//	Tf1 p1_range_2 = RemoveF4D(look.ranges_data[1], 1, 0, 0, P1);
//	AreEq(p1_range_2, p1_target_2);
//
//	std::array<float, card_count> p2_target_2 = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
//	Tf1 p2_range_2 = RemoveF4D(look.ranges_data[1], 1, 0, 0, P2);
//	AreEq(p2_range_2, p2_target_2);
//}
//
//
//TEST_CASE("lookahed_compute_terminal_equities")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//	look._compute_current_strategies();
//	look._compute_ranges();
//	look._compute_update_average_strategies(0);
//	look._compute_terminal_equities();
//
//	Tf1 p1_csvs_1 = RemoveF4D(look.cfvs_data[0], 0, 0, 0, P1);
//	AreEq(p1_csvs_1, 0);
//	Tf1 p2_csvs_1 = RemoveF4D(look.cfvs_data[0], 0, 0, 0, P2);
//	AreEq(p2_csvs_1, 0);
//
//	Tf1 p1_csvs_2 = RemoveF4D(look.cfvs_data[1], 0, 0, 0, P1);
//	AreEq(p1_csvs_2, 0);
//	Tf1 p2_csvs_2 = RemoveF4D(look.cfvs_data[1], 0, 0, 0, P2);
//	std::array<float, card_count> p2_target_csvs_2 = { -2400, -2400,     0, -2400, -2400, -2400 };
//	AreEq(p2_csvs_2, p2_target_csvs_2);
//
//
//	Tf1 p1_csvs_3 = RemoveF4D(look.cfvs_data[1], 1, 0, 0, P1);
//	std::array<float, card_count> p1_target_csvs_3 = { 240,   240,     0,   960,  -720,  -720 };
//	AreEq(p1_csvs_3, p1_target_csvs_3);
//
//	Tf1 p2_csvs_3 = RemoveF4D(look.cfvs_data[1], 1, 0, 0, P2);
//	std::array<float, card_count> p2_target_csvs_3 = { 600,   600,     0,  2400, -1800, -1800 };
//	AreEq(p2_csvs_3, p2_target_csvs_3);
//}
//
//TEST_CASE("lookahed_compute_cfvs")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//	look._compute_current_strategies();
//	look._compute_ranges();
//	look._compute_update_average_strategies(0);
//	look._compute_terminal_equities();
//	look._compute_cfvs();
//	//Util::Print(look.cfvs_data, 0);
//
//
//	Tf1 p1_csvs_1 = RemoveF4D(look.cfvs_data[1], 1, 0, 0, P1);
//	std::array<float, card_count> p1_target_csvs_1 = { 240,   240,     0,   960,  -720,  -720 };
//	AreEq(p1_csvs_1, p1_target_csvs_1);
//
//	Tf1 p2_csvs_1 = RemoveF4D(look.cfvs_data[1], 1, 0, 0, P2);
//	std::array<float, card_count> p2_target_csvs_1 = { 600,   600,     0,  2400, -1800, -1800 };
//	AreEq(p2_csvs_1, p2_target_csvs_1);
//}
//
//TEST_CASE("lookahed_compute_regrets")
//{
//	Resolving resolver;
//	BuildLook(resolver);
//	lookahead&  look = resolver._lookahead;
//	look._set_opponent_starting_range();
//	look._compute_current_strategies();
//	look._compute_ranges();
//	look._compute_update_average_strategies(0);
//	look._compute_terminal_equities();
//	look._compute_cfvs();
//	look._compute_regrets();
//
//	Tf1 p1_csvs_1 = RemoveF3D(look.regrets_data[1], 0, 0, 0);
//	std::array<float, card_count> p1_target_regret_1 = { 0,   0,     0,   0,  1800,  1800 };
//	AreEq(p1_csvs_1, p1_target_regret_1);
//
//	Tf1 p2_csvs_1 = RemoveF3D(look.regrets_data[1], 1, 0, 0);
//	AreEq(p2_csvs_1, 0);
//}
//
//TEST_CASE("lookahed_full_cycle")
//{
//	Resolving resolver;
//
//	Node node;
//	card_to_string_conversion converter;
//	node.board = converter.string_to_board("Ks");
//	node.street = 2;
//	node.current_player = P2;
//	node.bets << 1200, 1200;
//
//	card_tools tools;
//	Tf1 player_range = ToTmx(tools.get_uniform_range(node.board));
//	Tf1 op_cfvs(card_count);
//	op_cfvs.setZero();
//	LookaheadResult result = resolver.resolve(node, player_range, op_cfvs, 5, 10);
//
//	std::array<float, card_count> achieved_cfvs = { 240,240,0,960,-720,-720 };
//	AreEq(result.achieved_cfvs, achieved_cfvs);
//
//	std::array<float, card_count> children_cfvs = { 240.000000, 240.000000, 0.000000, 960.000000, -720.000000, -720.000000 };
//	Tf1 children_cfvs_res = RemoveF1D(result.children_cfvs, 1);
//	AreEq(children_cfvs_res, children_cfvs);
//
//	std::array<float, card_count> strategy1 = { 0,  0,  0,  0,  0 , 0 };
//	Tf1 stRes1 = RemoveF1D(result.strategy, 0);
//	AreEq(stRes1, strategy1);
//	std::array<float, card_count> strategy2 = { 1,  1,  1,  1 , 1,  1 };
//	Tf1 stRes2 = RemoveF1D(result.strategy, 1);
//	AreEq(stRes2, strategy2);
//}

TEST_CASE("lookahed_full_cycle_from_the_first_node")
{
	Resolving resolver;

	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P1;
	node.bets << 100, 100;

	card_tools tools;
	Tf1 player_range = ToTmx(tools.get_uniform_range(node.board));
	Tf1 op_cfvs(card_count);
	op_cfvs(0) =-5;
	op_cfvs(1) = 0;
	op_cfvs(2) = 7;
	op_cfvs(3) = -9;
	op_cfvs(4) = 8;
	op_cfvs(5) = 80;

	LookaheadResult result = resolver.resolve(node, player_range, op_cfvs, 5, 10);

	std::array<float, card_count> achieved_cfvs = { 22485.3223,	17512.9941,	-3649.5071,	37405.9766,	37293.7578,	1263.9318 };
	AreEq(result.achieved_cfvs, achieved_cfvs);

	std::array<float, card_count> children_cfvs = { 240.000000, 240.000000, 0.000000, 960.000000, -720.000000, -720.000000 };
	Tf1 children_cfvs_res = RemoveF1D(result.children_cfvs, 1);
	AreEq(children_cfvs_res, children_cfvs);

	std::array<float, card_count> strategy1 = { 0,  0,  0,  0,  0 , 0 };
	Tf1 stRes1 = RemoveF1D(result.strategy, 0);
	AreEq(stRes1, strategy1);
	std::array<float, card_count> strategy2 = { 1,  1,  1,  1 , 1,  1 };
	Tf1 stRes2 = RemoveF1D(result.strategy, 1);
	AreEq(stRes2, strategy2);
}

//TEST_CASE("test_lookahed_Ks_1200_1200")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("Ks");
//	params.root_node->street = 2;
//	params.root_node->current_player = P2;
//	params.root_node->bets << 1200, 1200;
//
//	card_tools tools;
//	Tf1 player_range = ToTmx(tools.get_uniform_range(node.board));
//	Tf1 op_range = ToTmx(tools.get_uniform_range(node.board));
//	Resolving resolver;
//	resolver.resolve(node, player_range, op_range);
//}