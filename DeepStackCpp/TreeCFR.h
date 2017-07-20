#pragma once
#include "Node.h"
#include "terminal_equity.h"
#include "assert.h"
#include "Util.h"
#include "arguments.h"

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
	void cfrs_iter_dfs(Node& node, size_t iter);

	//-- - Update a node's total regrets with the current iteration regrets.
	//-- @param node the node to update
	//-- @param current_regrets the regrets from the current iteration of CFR
	void update_regrets(Node& node, const ArrayXXf& current_regrets);

	//-- - Update a node's average strategy with the current iteration strategy.
	//-- @param node the node to update
	//-- @param current_strategy the CFR strategy for the current iteration
	//-- @param iter the iteration number of the current CFR iteration
	void update_average_strategy(Node& node, ArrayXXf& current_strategy, size_t iter);

	// Reduces 3d tensor to ArrayXXf map using specific dim
	inline Map<ArrayXXf> Reduce3dTensor(Tensor<float, 3>& tensor, int dim);

	//	-- - Run CFR to solve the given game tree.
	//	-- @param root the root node of the tree to solve.
	//	-- @param[opt] starting_ranges probability vectors over player private hands
	//	-- at the root node(default uniform)
	//	-- @param[opt] iter_count the number of iterations to run CFR for
	//	--(default @{arguments.cfr_iters
	//})
	void run_cfr(Node& root, const ArrayXXf&  starting_ranges, size_t iter_count = 0);

};

