#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "LookaheadBuilder.h"

#include <string>
#include <iostream>
using namespace std;

const float myEps = 0.001f;


TEST_CASE("build_lookahed_Ks_1200_1200")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets << 1200, 1200;

	tree_builder treeBuilder;
	Node& tree = treeBuilder.build_tree(params);

	lookahead look;
	LookaheadBuilder lookBuilder(look);
	lookBuilder.build_from_tree(tree);

	REQUIRE(look.first_call_terminal);
	REQUIRE(!look.first_call_transition);
	REQUIRE(!look.first_call_check);

	REQUIRE(look.empty_action_mask.size() == 1);

	REQUIRE(look.empty_action_mask[1].dimension(0) == 2);
	REQUIRE(look.empty_action_mask[1].dimension(1) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(3) == 6);
	auto targetTens = look.empty_action_mask[1];
	Tf4 zero = Tf4(targetTens);
	Tensor<bool, 0> eq = (targetTens == zero).all();
	REQUIRE(eq.coeff());

	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == -1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);

	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == -1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);

	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 0);

	REQUIRE(look.bets_count[-2] == 1);
	REQUIRE(look.bets_count[-1] == 1);
	REQUIRE(look.bets_count[0] == 0);
	REQUIRE(look.bets_count[1] == 0);

	REQUIRE(look.nonallinbets_count[-2] == 1);
	REQUIRE(look.nonallinbets_count[-1] == 1);
	REQUIRE(look.nonallinbets_count[0] == 0);
	REQUIRE(look.nonallinbets_count[2] == 0);

	REQUIRE(look.actions_count[-2] == 1);
	REQUIRE(look.actions_count[-1] == 1);
	REQUIRE(look.actions_count[0] == 2);
	REQUIRE(look.actions_count[1] == 0);

	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 0);
}


TEST_CASE("build_lookahed_300_300_Ks")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets << 300, 300;

	tree_builder treeBuilder;
	Node& tree = treeBuilder.build_tree(params);

	lookahead look;
	LookaheadBuilder lookBuilder(look);
	lookBuilder.build_from_tree(tree);

	REQUIRE(look.first_call_terminal);
	REQUIRE(!look.first_call_transition);
	REQUIRE(!look.first_call_check);

	REQUIRE(look.empty_action_mask.size() == 3);

	REQUIRE(look.empty_action_mask[1].dimension(0) == 4);
	REQUIRE(look.empty_action_mask[1].dimension(1) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(3) == 6);

	REQUIRE(look.empty_action_mask[2].dimension(0) == 3);
	REQUIRE(look.empty_action_mask[2].dimension(1) == 2);
	REQUIRE(look.empty_action_mask[2].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[2].dimension(3) == 6);

	REQUIRE(look.empty_action_mask[3].dimension(0) == 2);
	REQUIRE(look.empty_action_mask[3].dimension(1) == 1);
	REQUIRE(look.empty_action_mask[3].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[3].dimension(3) == 6);


	REQUIRE(look.empty_action_mask[1](0, 0, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[1](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](2, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](3, 0, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 1, 0, 0) == 0);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);

	//REQUIRE(look.all_nodes_count[0] == 1);
	//REQUIRE(look.all_nodes_count[1] == 2);
	//REQUIRE(look.all_nodes_count[2] == 0);

	//REQUIRE(look.allin_nodes_count[0] == 0);
	//REQUIRE(look.allin_nodes_count[1] == 1);
	//REQUIRE(look.allin_nodes_count[2] == 0);

	//REQUIRE(look.nonterminal_nodes_count[0] == 1);
	//REQUIRE(look.nonterminal_nodes_count[1] == 0);
	//REQUIRE(look.nonterminal_nodes_count[2] == 0);

	REQUIRE(look.nonterminal_nonallin_nodes_count[-1] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);

	//REQUIRE(look.terminal_nodes_count[0] == 0);
	//REQUIRE(look.terminal_nodes_count[1] == 2);
	//REQUIRE(look.terminal_nodes_count[2] == 0);

	//REQUIRE(look.inner_nodes_count[0] == 1);
	//REQUIRE(look.inner_nodes_count[1] == 1);
	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 2);
	REQUIRE(look.terminal_actions_count[3] == 0);

	REQUIRE(look.bets_count[-2] == 1);
	REQUIRE(look.bets_count[-1] == 1);
	REQUIRE(look.bets_count[0] == 2);
	REQUIRE(look.bets_count[1] == 1);
	REQUIRE(look.bets_count[2] == 0);

	REQUIRE(look.nonallinbets_count[-2] == 1);
	REQUIRE(look.nonallinbets_count[-1] == 1);
	REQUIRE(look.nonallinbets_count[0] == 1);
	REQUIRE(look.nonallinbets_count[2] == 0);

	REQUIRE(look.actions_count[-2] == 1);
	REQUIRE(look.actions_count[-1] == 1);
	REQUIRE(look.actions_count[0] == 4);
	REQUIRE(look.actions_count[1] == 3);
	REQUIRE(look.actions_count[2] == 2);
	REQUIRE(look.actions_count[3] == 0);

	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 2);
	REQUIRE(look.terminal_actions_count[3] == 0);
}

TEST_CASE("build_lookahed_300_900_Ks")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 300, 900;

	tree_builder treeBuilder;
	Node& tree = treeBuilder.build_tree(params);

	lookahead look;
	LookaheadBuilder lookBuilder(look);
	lookBuilder.build_from_tree(tree);

	REQUIRE(look.first_call_terminal);
	REQUIRE(!look.first_call_transition);
	REQUIRE(!look.first_call_check);

	REQUIRE(look.empty_action_mask.size() == 2);

	REQUIRE(look.empty_action_mask[1].dimension(0) == 3);
	REQUIRE(look.empty_action_mask[1].dimension(1) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[1].dimension(3) == 6);

	REQUIRE(look.empty_action_mask[2].dimension(0) == 2);
	REQUIRE(look.empty_action_mask[2].dimension(1) == 1);
	REQUIRE(look.empty_action_mask[2].dimension(2) == 1);
	REQUIRE(look.empty_action_mask[2].dimension(3) == 6);
	
	REQUIRE(look.empty_action_mask.size() == 2);

	REQUIRE(look.empty_action_mask[1](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](2, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](3, 0, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 1, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);

	REQUIRE(look.nonterminal_nonallin_nodes_count[-1] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == 0);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);

	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 0);

	REQUIRE(look.bets_count[-2] == 1);
	REQUIRE(look.bets_count[-1] == 1);
	REQUIRE(look.bets_count[0] == 1);
	REQUIRE(look.bets_count[1] == 0);
	REQUIRE(look.bets_count[2] == 0);

	REQUIRE(look.nonallinbets_count[-2] == 1);
	REQUIRE(look.nonallinbets_count[-1] == 1);
	REQUIRE(look.nonallinbets_count[0] == 0);
	REQUIRE(look.nonallinbets_count[1] == 0);

	REQUIRE(look.actions_count[-2] == 1);
	REQUIRE(look.actions_count[-1] == 1);
	REQUIRE(look.actions_count[0] == 3);
	REQUIRE(look.actions_count[1] == 2);
	REQUIRE(look.actions_count[2] == 0);

	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 2);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 0);
}

TEST_CASE("build_lookahed_100_100")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder treeBuilder;
	Node& tree = treeBuilder.build_tree(params);

	lookahead look;
	LookaheadBuilder lookBuilder(look);
	lookBuilder.build_from_tree(tree);

	REQUIRE(!look.first_call_terminal);
	REQUIRE(!look.first_call_transition);
	REQUIRE(look.first_call_check);

	REQUIRE(look.empty_action_mask.size() == 8);

	const auto& d1 = look.empty_action_mask[1].dimensions();
	REQUIRE((d1[0] == 4 && d1[1] == 1 && d1[2] == 1 && d1[3] == 6));

	const auto& d2 = look.empty_action_mask[2].dimensions();
	REQUIRE((d2[0] == 4 && d2[1] == 3 && d2[2] == 1 && d2[3] == 6));

	const auto& d3 = look.empty_action_mask[3].dimensions();
	REQUIRE((d3[0] == 6 && d3[1] == 2 && d3[2] == 2 && d3[3] == 6));

	const auto& d4 = look.empty_action_mask[4].dimensions();
	REQUIRE((d4[0] == 6 && d4[1] == 4 && d4[2] == 2 && d4[3] == 6));

	const auto& d5 = look.empty_action_mask[5].dimensions();
	REQUIRE((d5[0] == 6 && d5[1] == 4 && d5[2] == 6 && d5[3] == 6));

	const auto& d6 = look.empty_action_mask[6].dimensions();
	REQUIRE((d6[0] == 4 && d6[1] == 4 && d6[2] == 18 && d6[3] == 6));

	const auto& d7 = look.empty_action_mask[7].dimensions();
	REQUIRE((d7[0] == 3 && d7[1] == 2 && d7[2] == 54 && d7[3] == 6));

	const auto& d8 = look.empty_action_mask[8].dimensions();
	REQUIRE((d8[0] == 2 && d8[1] == 1 && d8[2] == 54 && d8[3] == 6));

	REQUIRE(look.empty_action_mask[1](0, 0, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[1](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](2, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[1](3, 0, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 0, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[2](2, 1, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](2, 2, 2, 0) == 0);

	REQUIRE(look.empty_action_mask[2](3, 2, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[2](3, 3, 0, 0) == 1);

	REQUIRE(look.empty_action_mask[3](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[3](1, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[3](2, 0, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[3](3, 0, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[3](3, 1, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[3](4, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[3](4, 0, 1, 0) == 0);
	REQUIRE(look.empty_action_mask[3](4, 1, 0, 0) == 0);
	REQUIRE(look.empty_action_mask[3](4, 1, 1, 0) == 0);

	REQUIRE(look.nonterminal_nonallin_nodes_count.size() == 11);
	REQUIRE(look.nonterminal_nonallin_nodes_count[-1] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == 2);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 2);
	REQUIRE(look.nonterminal_nonallin_nodes_count[3] == 6);
	REQUIRE(look.nonterminal_nonallin_nodes_count[4] == 18);
	REQUIRE(look.nonterminal_nonallin_nodes_count[5] == 54);
	REQUIRE(look.nonterminal_nonallin_nodes_count[6] == 54);
	REQUIRE(look.nonterminal_nonallin_nodes_count[7] == 0);
	REQUIRE(look.nonterminal_nonallin_nodes_count[8] == 0);
	REQUIRE(look.nonterminal_nonallin_nodes_count[9] == 0);


	REQUIRE(look.terminal_actions_count.size() == 11);
	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 1);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 2);
	REQUIRE(look.terminal_actions_count[3] == 2);
	REQUIRE(look.terminal_actions_count[4] == 2);
	REQUIRE(look.terminal_actions_count[5] == 2);
	REQUIRE(look.terminal_actions_count[6] == 2);
	REQUIRE(look.terminal_actions_count[7] == 2);
	REQUIRE(look.terminal_actions_count[8] == 0);

	REQUIRE(look.bets_count.size() == 11);
	REQUIRE(look.bets_count[-2] == 1);
	REQUIRE(look.bets_count[-1] == 1);
	REQUIRE(look.bets_count[0] == 3);
	REQUIRE(look.bets_count[1] == 2);
	REQUIRE(look.bets_count[2] == 4);
	REQUIRE(look.bets_count[3] == 4);
	REQUIRE(look.bets_count[4] == 4);
	REQUIRE(look.bets_count[5] == 2);
	REQUIRE(look.bets_count[6] == 1);
	REQUIRE(look.bets_count[7] == 0);
	REQUIRE(look.bets_count[8] == 0);

	REQUIRE(look.nonallinbets_count.size() == 11);
	REQUIRE(look.nonallinbets_count[-2] == 1);
	REQUIRE(look.nonallinbets_count[-1] == 1);
	REQUIRE(look.nonallinbets_count[0] == 2);
	REQUIRE(look.nonallinbets_count[1] == 1);
	REQUIRE(look.nonallinbets_count[2] == 3);
	REQUIRE(look.nonallinbets_count[3] == 3);
	REQUIRE(look.nonallinbets_count[4] == 3);
	REQUIRE(look.nonallinbets_count[5] == 1);
	REQUIRE(look.nonallinbets_count[6] == 0);
	REQUIRE(look.nonallinbets_count[7] == 0);
	REQUIRE(look.nonallinbets_count[8] == 0);

	REQUIRE(look.actions_count.size() == 11);
	REQUIRE(look.actions_count[-2] == 1);
	REQUIRE(look.actions_count[-1] == 1);
	REQUIRE(look.actions_count[0] == 4);
	REQUIRE(look.actions_count[1] == 4);
	REQUIRE(look.actions_count[2] == 6);
	REQUIRE(look.actions_count[3] == 6);
	REQUIRE(look.actions_count[4] == 6);
	REQUIRE(look.actions_count[5] == 4);
	REQUIRE(look.actions_count[6] == 3);
	REQUIRE(look.actions_count[7] == 2);
	REQUIRE(look.actions_count[8] == 0);

	REQUIRE(look.terminal_actions_count.size() == 11);
	REQUIRE(look.terminal_actions_count[-2] == 0);
	REQUIRE(look.terminal_actions_count[-1] == 0);
	REQUIRE(look.terminal_actions_count[0] == 1);
	REQUIRE(look.terminal_actions_count[1] == 2);
	REQUIRE(look.terminal_actions_count[2] == 2);
	REQUIRE(look.terminal_actions_count[3] == 2);
	REQUIRE(look.terminal_actions_count[4] == 2);
	REQUIRE(look.terminal_actions_count[5] == 2);
	REQUIRE(look.terminal_actions_count[7] == 2);
	REQUIRE(look.terminal_actions_count[8] == 0);
}