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

lookahead BuildLook()
{
	Resolving resolver;
	Node node;
	TreeBuilderParams params;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets << 1200, 1200;

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
	return look;
}


TEST_CASE("lookahed_set_opponent_starting_range")
{
	lookahead&  look = BuildLook();
	look._set_opponent_starting_range();

	std::array<float, card_count> opTarget = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
	Tf1 opRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P2);
	AreEq(opRrange, opTarget);

	std::array<float, card_count> plTarget = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
	Tf1 plRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P1);
	AreEq(plRrange, plTarget);
}

//TEST_CASE("lookahed_compute_current_strategies")
//{
//	lookahead look = BuildLook();
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

TEST_CASE("lookahed_compute_ranges")
{
	lookahead look = BuildLook();
	look._set_opponent_starting_range();
	look._compute_current_strategies();
	look._compute_ranges();

	std::array<float, card_count> opTarget = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
	Tf1 opRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P2);
	AreEq(opRrange, opTarget);

	std::array<float, card_count> plTarget = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
	Tf1 plRrange = RemoveF4D(look.ranges_data[0], 0, 0, 0, P1);
	AreEq(plRrange, plTarget);

	//[1] ----------
	Tf5 range1 = look.ranges_data[1];
	std::array<float, card_count> p1_target_1 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	Tf1 p1_range_1 = RemoveF4D(look.ranges_data[1], 0, 0, 0, P1);
	AreEq(p1_range_1, p1_target_1);

	std::array<float, card_count> p2_target_1 = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
	Tf1 p2_range_1 = RemoveF4D(look.ranges_data[1], 0, 0, 0, P2);
	AreEq(p2_range_1, p2_target_1);

	std::array<float, card_count> p1_target_2 = { 0.2, 0.2, 0.0, 0.2, 0.2, 0.2 };
	Tf1 p1_range_2 = RemoveF4D(look.ranges_data[1], 1, 0, 0, P1);
	AreEq(p1_range_2, p1_target_2);

	std::array<float, card_count> p2_target_2 = { 0.5, 0.5, 0.0, 0.5, 0.5, 0.5 };
	Tf1 p2_range_2 = RemoveF4D(look.ranges_data[1], 1, 0, 0, P2);
	AreEq(p2_range_2, p2_target_2);
}

//
//TEST_CASE("lookahed_compute_terminal_equities")
//{
//	lookahead look = BuildLook();
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