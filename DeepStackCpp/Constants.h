#pragma once

//  - Various constants used in DeepStack.
//  @module constants

// the number of players in the game
static const int players_count = 2;

// the number of betting rounds in the game
static const int streets_count = 2;

// IDs for each player and chance
enum players { chance = 0, P1 = 1, P2 = 2 }; 

//  IDs for terminal nodes (either after a fold or call action) and nodes that follow a check action
// @field terminal_fold(terminal node following fold) `-2`
// @field terminal_call(terminal node following call) -either last street or allin- `-1`
// @field chance_node(node for the chance player) `0`
// @field check(node following check) `-1`
// @field inner_node(any other node) `2`
enum node_types { terminal_fold = -2, terminal_call = -1, check = -1, chance_node = 0, inner_node = 1 };

//  IDs for fold and check/call actions
enum actions { fold = -2, ccall = -1 };

// String representations for actions in the ACPC protocol
// @field fold "`fold`"
// @field ccall(check / call) "`ccall`"
// @field raise "`raise`"
//constants.acpc_actions = {}
//constants.acpc_actions.fold = "fold"
//constants.acpc_actions.ccall = "ccall"
//constants.acpc_actions.raise = "raise"