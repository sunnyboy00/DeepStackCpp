#pragma once

// Representation of the current game state
struct GameState
{
	// the number of chips currently committed by each player
	long long bets[2];

	// the currently acting player
	int current_player;
};