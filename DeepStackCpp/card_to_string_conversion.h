#pragma once
#include <unordered_map>
#include <vector>
#include <math.h>
#include "game_settings.h"
#include "CustomSettings.h"
#include <Eigen/Dense>
#include <string>

using namespace std;

class card_to_string_conversion
{
private:
	// All possible card suits - only the first 2 are used in Leduc Hold'em.
	static string const suit_table[];

	// All possible card ranks - only the first 3 - 4 are used in Leduc Hold'em and 
	//variants.
	static string const  rank_table[];

public:

	// Holds the string representation for every possible card, indexed by its
	// numeric representation.
	unordered_map<int, string> card_to_string_table;

	// Holds the numeric representation for every possible card, indexed by its
	// string representation.
	unordered_map<string, int> string_to_card_table;

	card_to_string_conversion();

	//-- Gets the rank of a card.
	//-- @param card the numeric representation of the card
	// -- @return the index of the rank
	inline int card_to_rank(int card);

	// Gets the suit of a card.
	// @param card the numeric representation of the card
	// @return the index of the suit
	inline int card_to_suit(int card);

	// Converts a card's numeric representation to its string representation.
	// @param card the numeric representation of a card
	// @return the string representation of the card
	inline string card_to_string(int card);

	// Converts several cards' numeric representations to their string 
	//representations.
	// @param cards a vector of numeric representations of cards
	// @return a string containing each card's string representation, concatenated
	string cards_to_string(ArrayX cards);

	// Converts a card's string representation to its numeric representation.
	// @param card_string the string representation of a card
	// @return the numeric representation of the card
	inline int string_to_card(string card_string);

	// Converts a string representing zero or one board cards to a
	//	-- vector of numeric representations.
	//	-- @param card_string either the empty string or a string representation of a
	//	-- card
	//	-- @return either an empty tensor or a tensor containing the numeric
	//	-- representation of the card
	ArrayX string_to_board(string card_string);
};

//inline std::string card_to_string_conversion::card_to_suit(int card)
//	return card % game_settings.suit_count + 1
