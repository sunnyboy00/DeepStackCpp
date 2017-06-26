#pragma once

//-- - Game constants which define the game played by DeepStack.
//-- @module game_settings
//--leduc defintion
//-- - the number of card suits in the deck
static const int suit_count = 2;
//-- - the number of card ranks in the deck
static const int rank_count = 3;
//-- - the total number of cards in the deck
static const int card_count = suit_count * rank_count;
//-- - the number of public cards dealt in the game(revealed after the first betting round)
static const int board_card_count = 1;
//-- - the number of players in the game
static const int player_count = 2;