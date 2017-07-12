#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "card_to_string_conversion.h"
#include <string>

using namespace std;

unsigned int Factorial(unsigned int number) {
	return number <= 1 ? number : Factorial(number - 1)*number;
}

TEST_CASE("card_to_string", "[factorial]") {
	card_to_string_conversion converter;
	string card = converter.card_to_string(0);
	REQUIRE(card == "Ah");
}