#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "Resolving.h"
#include "TreeLookahead.h"
#include "cfrd_gadget_f.h"
#include <string>
#include <iostream>

using namespace std;

const float myEps = 0.001f;

template <int N>
void AreEq(Range& source, const std::array<float, N> target)
{
	assert(source.size() == N);
	for (size_t n = 0; n < N; n++)
	{
		REQUIRE(source(n) == Approx(target[n]).epsilon(myEps));
	}
}

void AreEq(Range& source, float scalar)
{
	for (long long n = 0; n < source.size(); n++)
	{
		float data = source(n);
		REQUIRE(data == Approx(scalar).epsilon(myEps));
	}
}

TreeLookahead* BuildLook(Resolving& resolver)
{
	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P2;
	node.bets << 1200, 1200;

	card_tools tools;
	Range player_range = tools.get_uniform_range(node.board);
	Range op_cfvs(card_count);
	op_cfvs.setZero();

	resolver._create_lookahead_tree(node);
	TreeLookahead* look = new TreeLookahead(*resolver._lookahead_tree);
	Range_f p = Range_f(player_range);
	Range_f o = Range_f(op_cfvs);

	look->_reconstruction_gadget = new cfrd_gadget_f(look->_root.board, p, o);
	look->_root.ranges.row(P1) = player_range;
	look->_reconstruction_opponent_cfvs = op_cfvs;
	return look;
}


TEST_CASE("tree_lookahed_set_opponent_starting_range")
{
	Resolving resolver;
	TreeLookahead* look = BuildLook(resolver);
	look->_set_opponent_starting_range();

	std::array<float, card_count> opTarget = { 0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f };
	Range opRrange = look->_root.ranges.row(P2);
	AreEq(opRrange, opTarget);

	std::array<float, card_count> plTarget = { 0.2f, 0.2f, 0.0f, 0.2f, 0.2f, 0.2f };
	Range plRrange = look->_root.ranges.row(P1);
	AreEq(plRrange, plTarget);
}

TEST_CASE("tree_lookahed_full_cycle_P2")
{
	Resolving resolver;
	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P2;
	node.bets << 100, 100;

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
	TreeLookahead* look = new TreeLookahead(*resolver._lookahead_tree);
	Range_f p = Range_f(player_range);
	Range_f o = Range_f(op_cfvs);

	look->_reconstruction_gadget = new cfrd_gadget_f(look->_root.board, p, o);
	look->_root.ranges.row(P1) = player_range;
	look->_reconstruction_opponent_cfvs = op_cfvs;

	look->_cfr_skip_iters = 5;
	look->_cfr_iters = 10;
	look->_reconstruction = true;
	look->_compute();
	LookaheadResult_f result = look->get_results();

	std::array<float, card_count> achieved_cfvs = { 10.2131f, 10.2131f, 0.0000f, 80.0000f,- 60.0000,- 60.0000 };
	AreEq(result.achieved_cfvs, achieved_cfvs);

	std::array<float, card_count> strategy1 = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000 };
	ArrayXf stRes1 = result.strategy.row(0);
	AreEq(stRes1, strategy1);
	std::array<float, card_count> strategy2 = { 1.0000,1.0000,0.3333,0.0000,1.0000,1.0000 };
	ArrayXf stRes2 = result.strategy.row(1);
	AreEq(stRes2, strategy2);
	std::array<float, card_count> strategy3 = { 0.0000,0.0000,0.3333,1.0000,0.0000,0.0000 };
	ArrayXf stRes3 = result.strategy.row(2);
	AreEq(stRes3, strategy3);
	std::array<float, card_count> strategy4 = { 0.0000,0.0000,0.3333,0.0000,0.0000,0.0000 };
	ArrayXf stRes4 = result.strategy.row(3);
	AreEq(stRes4, strategy4);

	
	std::array<float, card_count> children_cfvs1 = { 50.000000,50.000000,0.000000,100.000000,-50.000000,-50.000000 };
	ArrayXf  chStr1 = result.children_cfvs.row(1);
	AreEq(chStr1, children_cfvs1);

	std::array<float, card_count> children_cfvs2 = { -148.934738,-148.934738,0.000000,0.000000,-100.000000,-100.000000 };
	ArrayXf  chStr2 = result.children_cfvs.row(2);
	AreEq(chStr2, children_cfvs2);

	std::array<float, card_count> children_cfvs3 = { -85.490082,-85.591858,0.000000,1059.708374,-70.304504,-70.304504 };
	ArrayXf  chStr3 = result.children_cfvs.row(3);
	AreEq(chStr3, children_cfvs3);
}

TEST_CASE("tree_lookahed_full_cycle_P1")
{
	Resolving resolver;
	Node node;
	card_to_string_conversion converter;
	node.board = converter.string_to_board("Ks");
	node.street = 2;
	node.current_player = P1;
	node.bets << 100, 100;

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
	TreeLookahead* look = new TreeLookahead(*resolver._lookahead_tree);
	Range_f p = Range_f(player_range);
	Range_f o = Range_f(op_cfvs);

	look->_reconstruction_gadget = new cfrd_gadget_f(look->_root.board, p, o);
	look->_root.ranges.row(P1) = player_range;
	look->_reconstruction_opponent_cfvs = op_cfvs;

	look->_cfr_skip_iters = 5;
	look->_cfr_iters = 10;
	look->_reconstruction = true;
	look->_compute();
	LookaheadResult_f result = look->get_results();

	std::array<float, card_count> achieved_cfvs = { 11.4541, 8.4088, 0.0000,87.3549,-11.0164,-11.0164 };
	AreEq(result.achieved_cfvs, achieved_cfvs);

	std::array<float, card_count> strategy1 = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000 };
	ArrayXf stRes1 = result.strategy.row(0);
	AreEq(stRes1, strategy1);
	std::array<float, card_count> strategy2 = { 0.9375,1.0000,0.3333,0.4428,1.0000,1.0000 };
	ArrayXf stRes2 = result.strategy.row(1);
	AreEq(stRes2, strategy2);
	std::array<float, card_count> strategy3 = { 0.0625,0.0000,0.3333,0.5572,0.0000,0.0000 };
	ArrayXf stRes3 = result.strategy.row(2);
	AreEq(stRes3, strategy3);
	std::array<float, card_count> strategy4 = { 0.0000,0.0000,0.3333,0.0000,0.0000,0.0000 };
	ArrayXf stRes4 = result.strategy.row(3);
	AreEq(stRes4, strategy4);


	std::array<float, card_count> children_cfvs1 = { 35.229362,35.236393,0.000000,95.431213,1.573553,1.573553 };
	ArrayXf  chStr1 = result.children_cfvs.row(1);
	AreEq(chStr1, children_cfvs1);

	std::array<float, card_count> children_cfvs2 = { -156.584564,-181.203430,0.000000,30.273312,-99.999985,-99.999985 };
	ArrayXf  chStr2 = result.children_cfvs.row(2);
	AreEq(chStr2, children_cfvs2);

	std::array<float, card_count> children_cfvs3 = { -87.976555,-84.757027,0.000000,1123.724609,-66.811340,-66.811340 };
	ArrayXf  chStr3 = result.children_cfvs.row(3);
	AreEq(chStr3, children_cfvs3);
}


