#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "card_to_string_conversion.h"
#include <string>

using namespace std;

TEST_CASE("card_to_suit")
{
	card_to_string_conversion converter;
	REQUIRE(converter.card_to_suit(0) == 0);
	REQUIRE(converter.card_to_suit(1) == 1);

	REQUIRE(converter.card_to_suit(2) == 0);
	REQUIRE(converter.card_to_suit(3) == 1);

	REQUIRE(converter.card_to_suit(4) == 0);
	REQUIRE(converter.card_to_suit(5) == 1);
}

TEST_CASE("card_to_rank")
{
	card_to_string_conversion converter;
	REQUIRE(converter.card_to_rank(0) == 0);
	REQUIRE(converter.card_to_rank(1) == 0);

	REQUIRE(converter.card_to_rank(2) == 1);
	REQUIRE(converter.card_to_rank(3) == 1);

	REQUIRE(converter.card_to_rank(4) == 2);
	REQUIRE(converter.card_to_rank(5) == 2);
}

TEST_CASE("card_to_string")
{
	card_to_string_conversion converter;
	REQUIRE(converter.card_to_string(0) == "Ah");
	REQUIRE(converter.card_to_string(1) == "As");
	REQUIRE(converter.card_to_string(2) == "Kh");
	REQUIRE(converter.card_to_string(3) == "Ks");
	REQUIRE(converter.card_to_string(4) == "Qh");
	REQUIRE(converter.card_to_string(5) == "Qs");
}

TEST_CASE("cards_to_string") {
	card_to_string_conversion converter;
	ArrayXf cards(2);
	cards << 0, 3;
	REQUIRE(converter.cards_to_string(cards) == "AhKs");
}

TEST_CASE("string_to_card")
{
	card_to_string_conversion converter;
	REQUIRE(converter.string_to_card("Ah") == 0);
	REQUIRE(converter.string_to_card("As") == 1);
	REQUIRE(converter.string_to_card("Kh") == 2);
	REQUIRE(converter.string_to_card("Ks") == 3);
	REQUIRE(converter.string_to_card("Qh") == 4);
	REQUIRE(converter.string_to_card("Qs") == 5);
}

TEST_CASE("string_to_board")
{
	card_to_string_conversion converter;
	ArrayXf result = converter.string_to_board("Qh");
	REQUIRE(result(0) == 4);
}