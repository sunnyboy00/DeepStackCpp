#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "LookaheadBuilder.h"

#include <string>

const float myEps = 0.001f;

////TEST_CASE("build_lookahed_Ks_1200_1200")
////{
////	TreeBuilderParams params;
////	Node node;
////	params.root_node = &node;
////	card_to_string_conversion converter;
////	params.root_node->board = converter.string_to_board("Ks");
////	params.root_node->street = 2;
////	params.root_node->current_player = P2;
////	params.root_node->bets << 1200, 1200;
////
////	tree_builder treeBuilder;
////	Node& tree = treeBuilder.build_tree(params);
////
////	lookahead look;
////	LookaheadBuilder lookBuilder(look);
////	lookBuilder.build_from_tree(tree);
////
////	REQUIRE(look.first_call_terminal);
////	REQUIRE(!look.first_call_transition);
////	REQUIRE(!look.first_call_check);
////
////	REQUIRE(look.empty_action_mask.size() == 1);
////
////	REQUIRE(look.empty_action_mask[1].dimension(0) == 2);
////	REQUIRE(look.empty_action_mask[1].dimension(1) == 1);
////	REQUIRE(look.empty_action_mask[1].dimension(2) == 1);
////	REQUIRE(look.empty_action_mask[1].dimension(3) == 6);
////	auto targetTens = look.empty_action_mask[1];
////	Tf4 zero = Tf4(targetTens);
////	Tensor<bool, 0> eq = (targetTens == zero).all();
////	REQUIRE(eq.coeff());
////
////	REQUIRE(look.all_nodes_count[0] == 1);
////	REQUIRE(look.all_nodes_count[1] == 2);
////	REQUIRE(look.all_nodes_count[2] == 0);
////
////	REQUIRE(look.allin_nodes_count[0] == 0);
////	REQUIRE(look.allin_nodes_count[1] == 1);
////	REQUIRE(look.allin_nodes_count[2] == 0);
////
////	REQUIRE(look.nonterminal_nodes_count[0] == 1);
////	REQUIRE(look.nonterminal_nodes_count[1] == 0);
////	REQUIRE(look.nonterminal_nodes_count[2] == 0);
////
////	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
////	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == -1);
////	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);
////
////	REQUIRE(look.terminal_nodes_count[0] == 0);
////	REQUIRE(look.terminal_nodes_count[1] == 2);
////	REQUIRE(look.terminal_nodes_count[2] == 0);
////
////	REQUIRE(look.inner_nodes_count[0] == 1);
////	REQUIRE(look.inner_nodes_count[1] == 1); 
////
////	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1); 
////	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == -1);
////	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);
////
////	REQUIRE(look.terminal_actions_count[0] == 2);
////	REQUIRE(look.terminal_actions_count[1] == 0);
////
////	REQUIRE(look.bets_count[-2] == 1);
////	REQUIRE(look.bets_count[-1] == 1);
////	REQUIRE(look.bets_count[0] == 0);
////	REQUIRE(look.bets_count[1] == 0);
////
////	REQUIRE(look.nonallinbets_count[-2] == 1);
////	REQUIRE(look.nonallinbets_count[-1] == 1);
////	REQUIRE(look.nonallinbets_count[0] == 0);
////	REQUIRE(look.nonallinbets_count[2] == 0);
////
////	REQUIRE(look.actions_count[-2] == 1);
////	REQUIRE(look.actions_count[-1] == 1);
////	REQUIRE(look.actions_count[0] == 2);
////	REQUIRE(look.actions_count[1] == 0);
////
////	REQUIRE(look.terminal_actions_count[-2] == 0);
////	REQUIRE(look.terminal_actions_count[-1] == 0);
////	REQUIRE(look.terminal_actions_count[0] == 2);
////	REQUIRE(look.terminal_actions_count[1] == 0);
////}


TEST_CASE("build_lookahed_300_300")
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
	int val = look.empty_action_mask[2](2, 2, 1, 0);
	REQUIRE(look.empty_action_mask[2](2, 2, 1, 0) == 0);

	REQUIRE(look.empty_action_mask[2](0, 0, 0, 0) == 1);
	REQUIRE(look.empty_action_mask[2](1, 0, 0, 0) == 1);

	REQUIRE(look.all_nodes_count[0] == 1);
	REQUIRE(look.all_nodes_count[1] == 2);
	REQUIRE(look.all_nodes_count[2] == 0);

	REQUIRE(look.allin_nodes_count[0] == 0);
	REQUIRE(look.allin_nodes_count[1] == 1);
	REQUIRE(look.allin_nodes_count[2] == 0);

	REQUIRE(look.nonterminal_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nodes_count[1] == 0);
	REQUIRE(look.nonterminal_nodes_count[2] == 0);

	REQUIRE(look.nonterminal_nonallin_nodes_count[0] == 1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[1] == -1);
	REQUIRE(look.nonterminal_nonallin_nodes_count[2] == 0);

	REQUIRE(look.terminal_nodes_count[0] == 0);
	REQUIRE(look.terminal_nodes_count[1] == 2);
	REQUIRE(look.terminal_nodes_count[2] == 0);

	REQUIRE(look.inner_nodes_count[0] == 1);
	REQUIRE(look.inner_nodes_count[1] == 1);

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