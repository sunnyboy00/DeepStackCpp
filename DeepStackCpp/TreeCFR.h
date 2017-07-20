#pragma once
#include "Node.h"
#include "terminal_equity.h"
#include "assert.h"

#include <map>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class TreeCFR
{
public:
	TreeCFR();


private:

	// --for ease of implementation, we use small epsilon rather than zero when working with regrets
	const float regret_epsilon = 1.0f / 1000000000;

	map<ArrayXf*, terminal_equity*> _cached_terminal_equities;

	//-- - Gets an evaluator for player equities at a terminal node.
	//--
	//--Caches the result to minimize creation of @{terminal_equity | TerminalEquity
	//}
	//--objects.
	//-- @param node the terminal node to evaluate
	//-- @return a @{terminal_equity | TerminalEquity} evaluator for the node
	terminal_equity* _get_terminal_equity(Node& node);

	//-- - Recursively walks the tree, applying the CFR algorithm.
	//	-- @param node the current node in the tree
	//	-- @param iter the current iteration number
	//	-- @local
	void cfrs_iter_dfs(Node& node, long long iter);
};

