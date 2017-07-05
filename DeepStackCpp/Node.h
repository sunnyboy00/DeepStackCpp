#pragma once
#include "Constants.h"
#include <Eigen/Dense>
#include <vector>
#include <memory>

using namespace std;

struct Node
{

	// Type of the node. An element of @{constants.node_types} (if applicable)
	node_types node_type;

	// The current betting round
	int street;

	// A possibly empty vector of board cards
	Eigen::ArrayXf board;

	// A string representation of the board cards
	std::string board_string;

	// The player acting at the node
	int current_player;

	// The number of chips that each player has committed to the pot
	Eigen::Array2f bets;

	// Half the pot size, equalde to the smaller number in `bets`
	long long pot;

	Node* parent;

	// Is node a terminal node
	bool terminal;

	//--A strategy is represented at each public node by a NxK tensor where:
	//--
	//-- * N is the number of possible child nodes.
	//--
	//-- * K is the number of information sets for the active player in the public
	//--node.For the Leduc Hold'em variants we implement, there is one for each
	//-- private card that the player could hold.
	//--
	//--For a player node, `strategy[i][j]` gives the probability of taking the
	//-- action that leads to the `i`th child when the player holds the `j`th card.
	//--
	//--For a chance node, `strategy[i][j]` gives the probability of reaching the
	//-- `i`th child for either player when that player holds the `j`th card.
	Eigen::ArrayXXf strategy;

	// A list of children nodes
	vector<Node*> children;

	// Actions
	Eigen::ArrayXf actions;

	// Node depth
	long long depth;

	//Node(const Node &node);
};

