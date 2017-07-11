#pragma once
//#include <gvc.h>
#include "Node.h"
#include "GraphvizNode.h"
#include "card_to_string_conversion.h"
#include "GraphvizConnection.h"

#include <string>
#include <vector>
#include <memory>
#include <Eigen/Dense>
#include <iostream>
#include <iomanip>

using namespace std;
using Eigen::ArrayXXf;

class TreeVisualizer
{
public:
	TreeVisualizer();
	//-- - Generates a string representation of a tensor.
	//-- @param tensor a tensor
	//-- @param[opt] name a name for the tensor
	//-- @param[opt] format a format string to use with @{string.format} for each
	//-- element of the tensor
	//-- @param[opt] labels a list of labels for the elements of the tensor
	//-- @return a string representation of the tensor
	string add_tensor(ArrayXXf tensor, const char* name = nullptr, const char* format = nullptr, const vector<string>* labels = nullptr);

	//-- - Generates a string representation of any range or value fields that are set
	//	-- for the given tree node.
	//	-- @param node the node
	//	-- @return a string containing concatenated representations of any tensors
	//	-- stored in the `ranges_absolute`, `cf_values`, or `cf_values_br` fields of
	//	-- the node.
	string add_range_info(const Node& node);

	//-- - Generates data for a graphical representation of a node in a public tree.
	//-- @param node the node to generate data for
	//-- @return a table containing `name`, `label`, and `shape` fields for graphviz
	unique_ptr<GraphvisNode> node_to_graphviz(const Node& node);

	//-- - Generates data for graphical representation of a public tree action as an
	//	-- edge in a tree.
	//	-- @param from the graphical node the edge comes from
	//	-- @param to the graphical node the edge goes to
	//	-- @param node the public tree node before at which the action is taken
	//	-- @param child_node the public tree node that results from taking the action
	//	-- @return a table containing fields `id_from`, `id_to`, `id` for graphviz and
	//	--a `strategy` field to use as a label for the edge
	unique_ptr<GraphvizConnection> nodes_to_graphviz_edge(const GraphvisNode& from, const GraphvisNode& to, const Node& node, const Node& child_node);

private:
	long long node_to_graphviz_counter;
	long long edge_to_graphviz_counter;
	card_to_string_conversion _card_to_string;

	template<typename ... Args>
	string string_format(const std::string& format, Args ... args);
};

