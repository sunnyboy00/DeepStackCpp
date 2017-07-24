#pragma once
#include "Node.h"
#include "terminal_equity.h"
#include "assert.h"
#include "Util.h"
#include "arguments.h"

#include <map>
#include <Eigen/Dense>

//--- Computes the expected value of a strategy profile on a game's public tree,
// --as well as the value of a best response against the profile.
class tree_values
{
public:
	tree_values();
	~tree_values();

//private:
	terminal_equity _terminal_equity;

	card_tools _cardTools;

	//-- - Recursively walk the tree and calculate the probability of reaching each
	//-- node using the saved strategy profile.
	//--
	//--The reach probabilities are saved in the `ranges_absolute` field of each
	//-- node.
	//-- @param node the current node of the tree
	//-- @param ranges_absolute a 2xK tensor containing the probabilities of each
	//-- player reaching the current node with each private hand
	void _fill_ranges_dfs(Node& node, ArrayXXf& ranges_absolute);

};

