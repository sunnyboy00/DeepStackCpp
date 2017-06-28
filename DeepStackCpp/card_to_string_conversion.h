#pragma once
#include <unordered_map>
#include <vector>
#include <math.h>
#include "game_settings.h"

class card_to_string_conversion
{
private:
	//-- - Holds the string representation for every possible card, indexed by its
	// -- numeric representation.
	std::unordered_map<int, std::string> card_to_string_table;

	//-- - Holds the numeric representation for every possible card, indexed by its
	//- string representation.
	std::unordered_map<std::string, int> string_to_card_table;


	// ---All possible card suits - only the first 2 are used in Leduc Hold'em.
	std::vector<int> suit_table{ 'h', 's', 'c', 'd' };

	//-- - All possible card ranks - only the first 3 - 4 are used in Leduc Hold'em and 
	//--variants.
	std::vector<int> rank_table = { 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2' };

public:
	card_to_string_conversion();

	//-- - Gets the suit of a card.
	//-- @param card the numeric representation of the card
	//-- @return the index of the suit
	static inline int card_to_suit(int card);

	/*-- - Gets the rank of a card.
	-- @param card the numeric representation of the card
	-- @return the index of the rank*/
	static inline int card_to_rank(int card);

	//-- - Converts a card's numeric representation to its string representation.
	//-- @param card the numeric representation of a card
	//-- @return the string representation of the card
	inline std::string card_to_string(int card);

	//-- - Converts several cards' numeric representations to their string 
	//--representations.
	//-- @param cards a vector of numeric representations of cards
	//-- @return a string containing each card's string representation, concatenated
	std::string cards_to_string(std::vector<int> cards);

	//-- - Converts a card's string representation to its numeric representation.
	//-- @param card_string the string representation of a card
	//-- @return the numeric representation of the card
	inline int string_to_card(std::string card_string);

	//-- - Converts a string representing zero or one board cards to a
	//	-- vector of numeric representations.
	//	-- @param card_string either the empty string or a string representation of a
	//	-- card
	//	-- @return either an empty tensor or a tensor containing the numeric
	//	-- representation of the card
	std::vector<int> string_to_board(std::string card_string);
};

//inline std::string card_to_string_conversion::card_to_suit(int card)
//	return card % game_settings.suit_count + 1
