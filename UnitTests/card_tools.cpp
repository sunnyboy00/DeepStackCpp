#include "catch.hpp"
#include "Node.h"
#include "card_tools.h"
#include <string>

TEST_CASE("hand_is_possible")
{
	card_tools cardTools;

	ArrayXf hand1(card_count);
	hand1 << 0, 1, 2, 3, 4, 5;

	REQUIRE(cardTools.hand_is_possible(hand1));

	ArrayXf hand2(card_count);
	hand2 << 0, 1, 0, 3, 4, 5;
	REQUIRE(!cardTools.hand_is_possible(hand2));

	ArrayXf hand3(card_count);
	hand3 << 0, 1, 2, 3, 2, 5;
	REQUIRE(!cardTools.hand_is_possible(hand3));
}

TEST_CASE("get_possible_hand_indexes")
{
	card_tools cardTools;

	ArrayXf board1(1);
	board1 << 0;

	CardArray result = cardTools.get_possible_hand_indexes(board1);
	REQUIRE(!result(0));
	REQUIRE(result(1));
	REQUIRE(result(2));
	REQUIRE(result(3));
	REQUIRE(result(4));
	REQUIRE(result(5));

	ArrayXf board2(1);
	board2 << 1;

	result = cardTools.get_possible_hand_indexes(board2);
	REQUIRE(result(0));
	REQUIRE(!result(1));
	REQUIRE(result(2));
	REQUIRE(result(3));
	REQUIRE(result(4));
	REQUIRE(result(5));

	ArrayXf board3(1);
	board3 << 3;

	result = cardTools.get_possible_hand_indexes(board3);
	REQUIRE(result(0));
	REQUIRE(result(1));
	REQUIRE(result(2));
	REQUIRE(!result(3));
	REQUIRE(result(4));
	REQUIRE(result(5));
}

TEST_CASE("get_impossible_hand_indexes")
{
	card_tools cardTools;

	ArrayXf board(1);
	board << 3;

	CardArray result = cardTools.get_impossible_hand_indexes(board);
	REQUIRE(!result(0));
	REQUIRE(!result(1));
	REQUIRE(!result(2));
	REQUIRE(result(3));
	REQUIRE(!result(4));
	REQUIRE(!result(5));
}

TEST_CASE("get_uniform_range")
{
	card_tools cardTools;
	ArrayXf board(1);
	board << 3;

	CardArray result = cardTools.get_uniform_range(board);
	float koef = 1.0f / (card_count - 1);
	REQUIRE(result(0) == koef);
	REQUIRE(result(1) == koef);
	REQUIRE(result(2) == koef);
	REQUIRE(result(3) == 0);
	REQUIRE(result(4) == koef);
	REQUIRE(result(5) == koef);
}

TEST_CASE("get_random_range")
{
	card_tools cardTools;
	ArrayXf board(1);
	board << 4;

	CardArray result = cardTools.get_uniform_range(board);
	REQUIRE(result(0) > 0);
	REQUIRE(result(1) > 0);
	REQUIRE(result(2) > 0);
	REQUIRE(result(3) > 0);
	REQUIRE(result(4) == 0);
	REQUIRE(result(5) > 0);
}

TEST_CASE("is_valid_range")
{
	card_tools cardTools;
	ArrayXf board(1);
	board << 4;

	CardArray result = cardTools.get_uniform_range(board);
	REQUIRE(cardTools.is_valid_range(result, board));

	result(4) = 0.3f;
	REQUIRE(!cardTools.is_valid_range(result, board));
}

TEST_CASE("get_boards_count")
{
	card_tools cardTools;
	REQUIRE(cardTools.get_boards_count() == 6);
}

TEST_CASE("get_second_round_boards")
{
	card_tools cardTools;
	MatrixXf boards = cardTools.get_second_round_boards();
	REQUIRE(boards(0, 0) == 0);
	REQUIRE(boards(1, 0) == 1);
	REQUIRE(boards(2, 0) == 2);
	REQUIRE(boards(3, 0) == 3);
	REQUIRE(boards(4, 0) == 4);
	REQUIRE(boards(5, 0) == 5);
	REQUIRE(boards.rows() == card_count);
}
TEST_CASE("get_board_index")
{
	card_tools cardTools;

	ArrayXf board(1);
	board << 4;

	int index = cardTools.get_board_index(board);
	REQUIRE(index == 4);

	ArrayXf board2(1);
	board2 << 0;

	index = cardTools.get_board_index(board2);
	REQUIRE(index == 0);
}