#pragma once
#include "Node.h"
#include "CustomSettings.h"

struct TreeBuilderParams
{
	// the betting round of the root node
	int street;

	// the number of chips committed at the root node by each player
	long long bets;

	// the acting player at the root node
	int current_player;

	// a possibly empty vector of board cards at the root node
	Tf1 board;

	// if `true`, only build the current betting round
	bool limit_to_street;

	// object which gives the allowed bets for each player
	Tf1 bet_sizing;

	Node* root_node;

	TreeBuilderParams();
}; 


