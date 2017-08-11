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


TEST_CASE("test_lookahed_Ks_1200_1200")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets << 1200, 1200;

	card_tools tools;
	Tf1 player_range = ToTmx(tools.get_uniform_range(node.board));
	Tf1 op_range = ToTmx(tools.get_uniform_range(node.board));
	Resolving resolver;
	resolver.resolve(node, player_range, op_range);
}