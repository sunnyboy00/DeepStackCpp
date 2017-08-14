#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "cfrd_gadget.h"

#include <string>
#include <iostream>
using namespace std;

const float myEps = 0.001f;


TEST_CASE("cfr_gadget_Ks_2_iter")
{
	card_to_string_conversion converter;
	card_tools tools;

	Tf1 opponent_cfvs = Tf1(card_count);
	Util::Print(opponent_cfvs);
	opponent_cfvs.setConstant(0.2f);
	opponent_cfvs(2) = 0;

	ArrayXf board = converter.string_to_board("Ks");
	Tf1 player_range = ToTmx(tools.get_uniform_range(board));

	cfrd_gadget gadget(board, player_range, opponent_cfvs);
	Tf1 cur_opponent_cfvs = Tf1(opponent_cfvs);
	cur_opponent_cfvs.setConstant(0);

	Tf1 op_range = gadget.compute_opponent_range(cur_opponent_cfvs);

	REQUIRE(op_range(0) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(1) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(2) == Approx(0.0).epsilon(myEps));
	REQUIRE(op_range(3) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(4) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(5) == Approx(0.5).epsilon(myEps));

	cur_opponent_cfvs.setValues({ 240, 240, 0, 960, -720, -720 });
	op_range = gadget.compute_opponent_range(cur_opponent_cfvs);

	REQUIRE(op_range(0) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(1) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(2) == Approx(0.0).epsilon(myEps));
	REQUIRE(op_range(3) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(4) == Approx(0.0).epsilon(myEps));
	REQUIRE(op_range(5) == Approx(0.0).epsilon(myEps));
}

TEST_CASE("cfr_gadget_root_3_iter")
{
	card_to_string_conversion converter;
	card_tools tools;

	Tf1 opponent_cfvs = Tf1(card_count);
	Util::Print(opponent_cfvs);
	opponent_cfvs.setConstant(1.0f/6);

	ArrayXf board = converter.string_to_board("");
	Tf1 player_range = ToTmx(tools.get_uniform_range(board));

	cfrd_gadget gadget(board, player_range, opponent_cfvs);
	Tf1 cur_opponent_cfvs = Tf1(opponent_cfvs);
	cur_opponent_cfvs.setConstant(0);

	Tf1 op_range = gadget.compute_opponent_range(cur_opponent_cfvs);

	REQUIRE(op_range(0) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(1) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(2) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(3) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(4) == Approx(0.5).epsilon(myEps));
	REQUIRE(op_range(5) == Approx(0.5).epsilon(myEps));

	cur_opponent_cfvs.setValues({ 119.7473f, 118.7914f, -15.3321f, -15.4158f, -149.6880f, -149.7141f });
	op_range = gadget.compute_opponent_range(cur_opponent_cfvs);
	REQUIRE(op_range(0) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(1) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(2) == Approx(1.2904e-09f).epsilon(myEps));
	REQUIRE(op_range(3) == Approx(1.2835e-09f).epsilon(myEps));
	REQUIRE(op_range(4) == Approx(1.3346e-10f).epsilon(myEps));
	REQUIRE(op_range(5) == Approx(1.3344e-10f).epsilon(myEps));

	cur_opponent_cfvs.setValues({ 249.8833f,  249.3737f, -157.0048f, -157.2358f,  -55.4940f,  -55.2005f });
	op_range = gadget.compute_opponent_range(cur_opponent_cfvs);
	REQUIRE(op_range(0) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(1) == Approx(1.0).epsilon(myEps));
	REQUIRE(op_range(2) == Approx(1.2904e-09f).epsilon(myEps));
	REQUIRE(op_range(3) == Approx(1.2835e-09f).epsilon(myEps));
	REQUIRE(op_range(4) == Approx(1.3346e-10f).epsilon(myEps));
	REQUIRE(op_range(5) == Approx(1.3344e-10f).epsilon(myEps));
}