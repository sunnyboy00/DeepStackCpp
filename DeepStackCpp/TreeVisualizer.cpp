#include "TreeVisualizer.h"



TreeVisualizer::TreeVisualizer(): _card_to_string()
{
}


template<typename ... Args>
string TreeVisualizer::string_format(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

string TreeVisualizer::add_tensor(ArrayXXf tensor, const char* name, const char* format, const std::unordered_map<int, string>* labels)
{
	// std::stringstream fmt;
	string out = "";
	string format_l;

	if (name != nullptr)
	{
		string name_l(name);
		out = "| " + name_l + ": ";
	}

	if (format == nullptr)
	{
		//std::setprecision(3);
		format_l = "%.3f";
	}
	else
	{
		format_l = string(format);
	}

	for (int i = 0; i < tensor.cols(); i++)
	{
		if (labels != nullptr)
		{
			out += labels->at(i) + ":";
		}

		out += string_format(format_l, tensor(i)) + ", ";

	}

	return out;
}

string TreeVisualizer::add_range_info(const Node& node)
{
	string out = "";

	if (node.ranges_absolute.size() > 0)
	{
		out += add_tensor(node.ranges_absolute.row(0), "abs_range1");
		out += add_tensor(node.ranges_absolute.row(1), "abs_range2");
	}

	if (node.cf_values.size() > 0)
	{
		//--cf values computed by real tree dfs
		out += add_tensor(node.cf_values.row(0), "cf_values1");
		out += add_tensor(node.cf_values.row(1), "cf_values2");
	}

	if (node.cf_values_br.size() > 0)
	{
		//--cf values that br has in real tree
		out += add_tensor(node.cf_values_br.row(0), "cf_values_br1");
		out += add_tensor(node.cf_values_br.row(1), "cf_values_br2");
	}

	return out;
}

GraphvisNode* TreeVisualizer::node_to_graphviz(const Node & node)
{
	GraphvisNode* out = new GraphvisNode();

	//--1.0 label
	out->Label = "\"<f0>" + to_string(node.current_player);

	if (node.terminal)
	{
		if (node.node_type == terminal_fold)
		{
			out->Label += "| TERMINAL FOLD";
		}
		else if (node.node_type == terminal_call)
		{
			out->Label += "| TERMINAL CALL";
		}
		else
		{
			assert("unknown terminal node type");
		}
	}
	else
	{
		string betOneStr = string_format("%.0f", node.bets(P1 - 1));
		string betTwoStr = string_format("%.0f", node.bets(P2 - 2));

		out->Label += "| bet1: " + betOneStr + "| bet2: " + betTwoStr;

		if (node.street > 0)
		{
			out->Label += "| street: " + to_string(node.street);
			out->Label += "| board: " + _card_to_string.cards_to_string(node.board);
			out->Label += "| depth: "  + to_string(node.depth);
		}
	}

	// No such field node.margin in sources
	//if (node.margin > 0)
	//{
	//	out->Label += "| margin: " + to_string(node.margin);
	//}

	out->Label += add_range_info(node);

	if (node.cfv_infset.size() > 0)
	{
		out->Label += "| cfv1: " + to_string(node.cfv_infset(0));
		out->Label += "| cfv2: " + to_string(node.cfv_infset(1));
		out->Label += "| cfv_br1: " + to_string(node.cfv_br_infset(0));
		out->Label += "| cfv_br2: " + to_string(node.cfv_br_infset(1));
		out->Label += "| epsilon1: " + to_string(node.epsilon(0));
		out->Label += "| epsilon2: " + to_string(node.epsilon(1));
	}

	if (node.lookahead_coordinates.size() > 0)
	{
		out->Label += "| COORDINATES ";
		out->Label += "| action_id: " + to_string(node.lookahead_coordinates(0));
		out->Label += "| parent_action_id: " + to_string(node.lookahead_coordinates(1));
		out->Label += "| gp_id: " + to_string(node.lookahead_coordinates(2));
	}

	out->Label += "\"";

	// --2.0 name
	out->Name = "\"node" + to_string(node_to_graphviz_counter) + "\"";

	// --3.0 shape
	out->Shape = "\"record\"";

	node_to_graphviz_counter++;

	return out;
}

GraphvizConnection* TreeVisualizer::nodes_to_graphviz_edge(const GraphvisNode& from, const GraphvisNode & to, const Node & node, const Node & child_node)
{
	GraphvizConnection* out = new GraphvizConnection();
	out->Id_from = from.Name;
	out->Id_to = to.Name;
	out->Id = edge_to_graphviz_counter;

	// --get the child id of the child node
	long long child_id = -1;
	for (size_t i = 0; i < node.children.size(); i++)
	{
		if (node.children[i] == &child_node)
		{
			child_id = i;
		}
	}

	assert(child_id != -1);
	out->Strategy = add_tensor(node.strategy.row(child_id), nullptr, "%.2f", &_card_to_string.card_to_string_table);
	edge_to_graphviz_counter++;
	return out;
}

GraphvisNode* TreeVisualizer::graphviz_dfs(const Node& node, vector<GraphvisNode*>& nodes, vector<GraphvizConnection*>& edges)
{
	GraphvisNode* gv_node = node_to_graphviz(node);
	nodes.push_back(gv_node);

	for (size_t i = 0; i < node.children.size(); i++)
	{
		Node* child_node = node.children[i];
		GraphvisNode* gv_node_child = graphviz_dfs(*child_node, nodes, edges);
		GraphvizConnection* gv_edge = nodes_to_graphviz_edge(*gv_node, *gv_node_child, node, *child_node);
		edges.push_back(gv_edge);
	}

	return gv_node;
}

void TreeVisualizer::graphviz(const Node & root, string filename)
{
	if (filename == "")
	{
		filename = "tree_2.dot";
	}

	string out = "digraph g {  graph [ rankdir = \"LR\"];node [fontsize = \"16\" shape = \"ellipse\"]; edge [];";

	vector<GraphvisNode*> nodes;
	vector<GraphvizConnection*> edges;

	graphviz_dfs(root, nodes, edges);

	for (size_t i = 0; i < nodes.size(); i++)
	{
		GraphvisNode* node = nodes[i];
		string node_text = node->Name + "[" + "label=" + node->Label + " shape = " + node->Shape + "];";
		out += node_text;
	}

	for (size_t i = 0; i < edges.size(); i++)
	{
		GraphvizConnection* edge = edges[i];
		string edge_text = edge->Id_from + ":f0 -> " + edge->Id_to + ":f0 [ id = " + to_string(edge->Id) + " label = \"" + edge->Strategy + "\"];";
		out += edge_text;
	}

	out += "}";

	//--write into dot file
	ofstream file;
	string targetpath = data_directory + "Dot/" + filename;
	file.open(targetpath);
	file << out;
	file.close();

	//--run graphviz program to generate image
	string dotTarget = "dot " + targetpath + " -Tsvg -O";
	system(dotTarget.c_str());
}


