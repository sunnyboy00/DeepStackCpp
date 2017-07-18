#include "catch.hpp"
#include "terminal_equity.h"
#include <iostream>

float GetMatrixVal(string me, string other, ArrayXXf matrix)
{
	card_to_string_conversion converter;
	int c1 = converter.string_to_card(other);
	int c2 = converter.string_to_card(me);
	float res = matrix(c1, c2);
	return res;
}

TEST_CASE("get_last_round_call_matrix")
{
	card_to_string_conversion converter;
	int card = converter.string_to_card("Ah");

	ArrayXf board(1);
	board << (float)card;

	terminal_equity term;

	ArrayXXf callMatrix;
	term.get_last_round_call_matrix(board, callMatrix);

	REQUIRE(GetMatrixVal("As", "Ah", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ah", "As", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ah", "Ah", callMatrix) == 0);
	REQUIRE(GetMatrixVal("As", "As", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ks", "Kh", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ks", "Ks", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Kh", "Kh", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qh", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qs", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qh", "Qh", callMatrix) == 0);


	REQUIRE(GetMatrixVal("As", "Kh", callMatrix) == 1);
	REQUIRE(GetMatrixVal("Ks", "As", callMatrix) == -1);

	REQUIRE(GetMatrixVal("Ks", "Qs", callMatrix) == 1);
	REQUIRE(GetMatrixVal("Qs", "Ks", callMatrix) == -1);
	REQUIRE(GetMatrixVal("Kh", "Qh", callMatrix) == 1);
	REQUIRE(GetMatrixVal("Qh", "Kh", callMatrix) == -1);

	REQUIRE(GetMatrixVal("Ks", "Qs", callMatrix) == 1);
	REQUIRE(GetMatrixVal("As", "Qs", callMatrix) == 1);
	REQUIRE(GetMatrixVal("Qs", "As", callMatrix) == -1);
	REQUIRE(GetMatrixVal("Kh", "Qh", callMatrix) == 1);
}

TEST_CASE("_fold_matrix")
{
	card_to_string_conversion converter;
	ArrayXf board = converter.string_to_board("Qs");
	terminal_equity term;

	term._set_fold_matrix(board);
	auto foldMatrix = term._fold_matrix;

	REQUIRE(GetMatrixVal("As", "Ah", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Ah", "As", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Ks", "Kh", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("As", "Kh", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Ks", "As", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Kh", "Qh", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Qh", "Kh", foldMatrix) == 1);
	REQUIRE(GetMatrixVal("Kh", "Qh", foldMatrix) == 1);

	REQUIRE(GetMatrixVal("Ah", "Ah", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("As", "As", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Ks", "Ks", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Kh", "Kh", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qh", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qs", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Qh", "Qh", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Ks", "Qs", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Ks", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Ks", "Qs", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("As", "Qs", foldMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "As", foldMatrix) == 0);
}

TEST_CASE("_call_matrix")
{
	card_to_string_conversion converter;
	ArrayXf board = converter.string_to_board("");
	terminal_equity term;

	term._set_call_matrix(board);
	auto callMatrix = term._equity_matrix;
	cout << callMatrix;
	REQUIRE(GetMatrixVal("As", "Ah", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ah", "As", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Ah", "Ah", callMatrix) == 0);
	REQUIRE(GetMatrixVal("As", "As", callMatrix) == 0);

	REQUIRE(GetMatrixVal("As", "Kh", callMatrix) == 0.5);
	REQUIRE(GetMatrixVal("Ah", "Ks", callMatrix) == 0.5);

	REQUIRE(GetMatrixVal("Ah", "Qh", callMatrix) == 0.5);
	REQUIRE(GetMatrixVal("As", "Qs", callMatrix) == 0.5);

	REQUIRE(GetMatrixVal("Ks", "As", callMatrix) == -0.5);
	REQUIRE(GetMatrixVal("Qh", "Kh", callMatrix) == -0.5);
	REQUIRE(GetMatrixVal("Kh", "Qh", callMatrix) == 0.5);


	REQUIRE(GetMatrixVal("Ks", "Ks", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Kh", "Kh", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qh", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qs", "Qs", callMatrix) == 0);
	REQUIRE(GetMatrixVal("Qh", "Qh", callMatrix) == 0);

	REQUIRE(GetMatrixVal("Ks", "Qs", callMatrix) == 0.5);
	REQUIRE(GetMatrixVal("Qs", "Ks", callMatrix) == -0.5);
	REQUIRE(GetMatrixVal("Qs", "As", callMatrix) == -0.5);
}