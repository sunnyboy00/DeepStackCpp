//#include "catch.hpp"
//#include "bet_sizing.h"
//#include "Node.h"
//#include "card_to_string_conversion.h"
//#include "TreeBuilderParams.h"
//#include "tree_builder.h"
//#include "card_tools.h"
//#include "cfrd_gadget.h"
//
//#include <string>
//#include <iostream>
//using namespace std;
//
//const float myEps = 0.001f;
//
//
//TEST_CASE("cfr_gadget_Ks_1_iter")
//{
//	card_to_string_conversion converter;
//	card_tools tools;
//
//	Tf1 opponent_cfvs = Tf1(card_count);
//	opponent_cfvs.setConstant(1);
//
//	ArrayXf board = converter.string_to_board("Ks");
//	Tf1 player_range = ToTmx(tools.get_uniform_range(board));
//
//	cfrd_gadget gadget(board, player_range, opponent_cfvs);
//	Tf1 cur_opponent_cfvs = Tf1(opponent_cfvs);
//	cur_opponent_cfvs.setConstant(1);
//
//	Tf1 op_range = gadget.compute_opponent_range(cur_opponent_cfvs, 0);
//	cout << op_range;
// 
//	REQUIRE(op_range(0) == Approx(0.5).epsilon(myEps));
//	REQUIRE(op_range(1) == Approx(0.5).epsilon(myEps));
//	REQUIRE(op_range(2) == Approx(0.0).epsilon(myEps));
//	REQUIRE(op_range(3) == Approx(0.5).epsilon(myEps));
//	REQUIRE(op_range(4) == Approx(0.5).epsilon(myEps));
//	REQUIRE(op_range(5) == Approx(0.5).epsilon(myEps));
//}