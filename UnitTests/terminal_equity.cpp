#include "catch.hpp"
#include "terminal_equity.h"
#include <iostream>

int GetCallMatrixVal(string me, string other, ArrayXXf callMatrix)
{
	card_to_string_conversion converter;
	return callMatrix(converter.string_to_card(other), converter.string_to_card(me));
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

	cout << callMatrix;
	REQUIRE(GetCallMatrixVal("As", "Ah", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Ah", "As", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Ah", "Ah", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("As", "As", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Ks", "Kh", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Ks", "Ks", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Kh", "Kh", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Qs", "Qh", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Qs", "Qs", callMatrix) == 0);
	REQUIRE(GetCallMatrixVal("Qh", "Qh", callMatrix) == 0);


	REQUIRE(GetCallMatrixVal("As", "Kh", callMatrix) == 1);
	REQUIRE(GetCallMatrixVal("Ks", "As", callMatrix) == -1);

	REQUIRE(GetCallMatrixVal("Ks", "Qs", callMatrix) == 1);
	REQUIRE(GetCallMatrixVal("Qs", "Ks", callMatrix) == -1);
	REQUIRE(GetCallMatrixVal("Kh", "Qh", callMatrix) == 1);
	REQUIRE(GetCallMatrixVal("Qh", "Kh", callMatrix) == -1);

	REQUIRE(GetCallMatrixVal("Ks", "Qs", callMatrix) == 1);
	REQUIRE(GetCallMatrixVal("As", "Qs", callMatrix) == 1);
	REQUIRE(GetCallMatrixVal("Qs", "As", callMatrix) == -1);
	REQUIRE(GetCallMatrixVal("Kh", "Qh", callMatrix) == 1);
}