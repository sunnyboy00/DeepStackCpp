#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include <string>


Node CreateNode()
{
	card_to_string_conversion converter;
	Node root_node;
	root_node.board = converter.string_to_board("");
	root_node.street = 1;
	root_node.bets = T2f();
	return root_node;
}


TEST_CASE("bet_sizing_300_300")
{
	bet_sizing_manager manager;
	Node root_node = CreateNode();
	root_node.current_player = P1;
	root_node.bets << 100, 200;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 600);
	REQUIRE(bets(1, 0) == 1200);

	REQUIRE(bets(0, 1) == 200);
	REQUIRE(bets(1, 1) == 200);
}

TEST_CASE("bet_sizing_100_100")
{

	bet_sizing_manager manager;
	Node root_node = CreateNode();
	root_node.current_player = P1;
	root_node.bets << 100, 100;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 300);
	REQUIRE(bets(1, 0) == 1200);

	REQUIRE(bets(0, 1) == 100);
	REQUIRE(bets(1, 1) == 100);

	REQUIRE(bets.dimension(0) == 2);
}

TEST_CASE("bet_sizing_100_100_P2")
{

	bet_sizing_manager manager;
	Node root_node = CreateNode();
	root_node.current_player = P2;
	root_node.bets << 100, 100;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 100);
	REQUIRE(bets(1, 0) == 100);

	REQUIRE(bets(0, 1) == 300);
	REQUIRE(bets(1, 1) == 1200);

	REQUIRE(bets.dimension(0) == 2);
}

TEST_CASE("bet_sizing_400_100_P2")
{

	bet_sizing_manager manager;
	Node root_node = CreateNode();
	root_node.current_player = P2;
	root_node.bets << 400, 100;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 400);
	REQUIRE(bets(0, 1) == 1200);

	REQUIRE(bets.dimension(0) == 1);
}

TEST_CASE("bet_sizing_300_600_P1")
{

	bet_sizing_manager manager;
	Node root_node = CreateNode();
	root_node.current_player = P1;
	root_node.bets << 300, 600;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 1200);
	REQUIRE(bets(0, 1) == 600);

	REQUIRE(bets.dimension(0) == 1);
}

TEST_CASE("bet_sizing_100_200_pot_size_0.5_1")
{

	Tf1 potSize(2);
	potSize << 0.5, 1.0;
	bet_sizing_manager manager(potSize);
	Node root_node = CreateNode();
	root_node.current_player = P1;
	root_node.bets << 100, 200;

	TX2f bets = manager.get_possible_bets(root_node);

	REQUIRE(bets(0, 0) == 400);
	REQUIRE(bets(1, 0) == 600);
	REQUIRE(bets(2, 0) == 1200);

	REQUIRE(bets(0, 1) == 200);
	REQUIRE(bets(1, 1) == 200);
	REQUIRE(bets(2, 1) == 200);

	REQUIRE(bets.dimension(0) == 3);
}