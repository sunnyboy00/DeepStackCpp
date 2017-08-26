//#pragma once
//#include "CustomSettings.h"
//#include "Util.h"
//#include "Node.h"
//
//class continual_resolving
//{
//public:
//	continual_resolving();
//	~continual_resolving();
//
//	//-- - Solves a depth - limited lookahead from the first node of the game to get
//	//	-- opponent counterfactual values.
//	//	--
//	//	--The cfvs are stored in the field `starting_cfvs_p1`. Because this is the
//	//	-- first node of the game, exact ranges are known for both players, so
//	//	-- opponent cfvs are not necessary for solving.
//	void resolve_first_node();
//
//	//	-- - Re - initializes the continual re - solving to start a new game from the root
//	//	-- of the game tree.
//	//	-- @param state the first state where the re - solving player acts in the new
//	//	--game(a table of the type returned by @{protocol_to_node.parse_state
//	//})
//	void start_new_hand(state);
//
//	//-- - Re - solves a node to choose the re - solving player's next action.
//	//-- @param node the game node where the re - solving player is to act(a table of
//	//	-- the type returned by @{protocol_to_node.parsed_state_to_node
//	//})
//	//-- @param state the game state where the re - solving player is to act
//	//-- (a table of the type returned by @{protocol_to_node.parse_state})
//	//-- @local
//	void _resolve_node(Node& node, state);
//
//	//	-- - Updates the player's range and the opponent's counterfactual values to be
//	//	-- consistent with game actions since the last re - solved state.
//	//	--Updates it only for actions we did not make, since we update the invariant for our action as soon as we make it.
//	//	--
//	//	-- @param node the game node where the re - solving player is to act(a table of
//	//	-- the type returned by @{protocol_to_node.parsed_state_to_node
//	//})
//	//-- @param state the game state where the re - solving player is to act
//	//-- (a table of the type returned by @{protocol_to_node.parse_state})
//	//-- @local
//	void _update_invariant(node, state);
//
//	//	-- - Re - solves a node and chooses the re - solving player's next action.
//	//	-- @param node the game node where the re - solving player is to act(a table of
//	//	-- the type returned by @{protocol_to_node.parsed_state_to_node
//	//})
//	//-- @param state the game state where the re - solving player is to act
//	//-- (a table of the type returned by @{protocol_to_node.parse_state})
//	//-- @return an action sampled from the re - solved strategy at the given state,
//	//--with the fields :
//	//	--
//	//	-- * `action`: an element of @{constants.acpc_actions}
//	//	--
//	//		-- * `raise_amount`: the number of chips to raise(if `action` is raise)
//	void compute_action(node, state);
//
//	//	-- - Samples an action to take from the strategy at the given game state.
//	//	-- @param node the game node where the re - solving player is to act(a table of
//	//	-- the type returned by @{protocol_to_node.parsed_state_to_node
//	//})
//	//-- @param state the game state where the re - solving player is to act
//	//-- (a table of the type returned by @{protocol_to_node.parse_state})
//	//-- @return an index representing the action chosen
//	//-- @local
//	void _sample_bet(node, state);
//
//	//	-- - Converts an internal action representation into a cleaner format.
//	//	-- @param node the game node where the re - solving player is to act(a table of
//	//	-- the type returned by @{protocol_to_node.parsed_state_to_node
//	//})
//	//-- @param sampled_bet the index of the action to convert
//	//-- @return a table specifying the action, with the fields :
//	//	--
//	//	-- * `action`: an element of @{constants.acpc_actions}
//	//	--
//	//	-- * `raise_amount`: the number of chips to raise(if `action` is raise)
//	//	-- @local
//	void _bet_to_action(node, sampled_bet);
//};
//
