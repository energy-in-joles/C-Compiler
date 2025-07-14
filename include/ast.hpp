#pragma once

#include <iostream>
#include <string>
#include <vector>

// ------- Node and Context -------
#include "ast_node.hpp"
#include "ast_context.hpp"

// ------- Expressions -------
#include "ast_identifier.hpp"
#include "ast_constant.hpp"
#include "ast_increment.hpp"

// ------- Functions and Scoping -------
#include "ast_scope.hpp"
#include "ast_function_call.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_function_definition.hpp"

// ------- Math and logic -------
#include "ast_unary_op.hpp"
#include "ast_binary_op.hpp"
#include "ast_relational_op.hpp"
#include "ast_logical_op.hpp"

// ------- Statements -------
#include "ast_jump_statement.hpp" // return
#include "ast_assignment.hpp"
#include "ast_keyword.hpp" // break, continue

// ------- Blocks -------
#include "ast_ifelse.hpp"
#include "ast_while.hpp"
#include "ast_for.hpp"
#include "ast_switch_case.hpp"

// ------- Declarations -------
#include "ast_declaration.hpp"
#include "ast_direct_declarator.hpp"
#include "ast_type_specifier.hpp"
#include "ast_init_declarator.hpp"
#include "ast_translation_unit.hpp"
#include "ast_enum.hpp"

// --- Arrays ---
#include "ast_array_declarator.hpp"
#include "ast_array_index.hpp"

// ------- Pointers -------
#include "ast_pointer_declarator.hpp"
#include "ast_pointer_unary.hpp"

// ------- Sizeof -------
#include "ast_sizeof.hpp"

ast::NodePtr
ParseAST(std::string file_name);
