#include <ti-basic-plus-plus/sema/semantic_analyzer.h>

#include <assert.h>
#include <stb_ds.h>
#include <ti-basic-plus-plus/calculator/builtin_functions.h>
#include <ti-basic-plus-plus/calculator/variable.h>

// This code isn't the nicest, it was one last things I wrote!

static inline ast_node_t** duplicate_array(ast_node_t** array) {
  ast_node_t** new_array = NULL;
  for (size_t i = 0; i < arrlenu(array); i++) {
    arrput(new_array, array[i]);
  }

  return new_array;
}

static bool is_variable_free(variable_t var, variable_t* used) {
  for (size_t i = 0; i < arrlenu(used); i++) {
    if (used[i].type == var.type && used[i].id == var.id) {
      return false;
    }
  }

  return true;
}

static variable_t find_free_variable(variable_t* reserved,
                                     variable_t* assigned,
                                     variable_type_t type,
                                     diagnostics_t* d);

static void analyze_expr(ast_node_t* expr,
                         ast_node_t** variable_decls,
                         ast_node_t** function_decls,
                         diagnostics_t* d) {
  switch (expr->kind) {
    case AST_BINARY_EXPRESSION:
      analyze_expr(expr->children[0], variable_decls, function_decls, d);
      analyze_expr(expr->children[1], variable_decls, function_decls, d);
      break;
    case AST_UNARY_EXPRESSION:
      analyze_expr(expr->children[0], variable_decls, function_decls, d);
      break;
    case AST_IDENTIFIER:
      for (size_t i = 0; i < arrlenu(variable_decls); i++) {
        ast_node_t* decl = variable_decls[i];

        if (arrlenu(decl->data.variable_decl.name) ==
                arrlenu(expr->data.identifier) &&
            strncmp(decl->data.variable_decl.name, expr->data.identifier,
                    arrlenu(expr->data.identifier)) == 0) {
          arrput(expr->children, decl);
          expr->dont_free_children = true;
          goto VAR_FOUND;
        }
      }

      diag_report_source(d, ERROR, &expr->location,
                         "reference to undeclared variable");
      return;

    VAR_FOUND:
      break;
    case AST_FUNCTION_CALL:
      for (size_t i = 0; i < arrlenu(function_decls); i++) {
        ast_node_t* decl = function_decls[i];

        if (arrlenu(decl->data.function_decl.name) ==
                arrlenu(expr->data.function_call.name) &&
            strncmp(decl->data.function_decl.name,
                    expr->data.function_call.name,
                    arrlenu(expr->data.function_call.name)) == 0) {
          expr->data.function_call.matched_function.decl = (ast_node_t*)decl;
          expr->data.function_call.is_builtin = false;
          goto FUNC_FOUND;
        }
      }

      builtin_function_t func =
          match_builtin_function(expr->data.function_call.name,
                                 arrlenu(expr->data.function_call.name));
      if (func != FUNC_UNKNOWN) {
        expr->data.function_call.matched_function.builtin = func;
        expr->data.function_call.is_builtin = true;
        goto FUNC_FOUND;
      }

      diag_report_source(d, ERROR, &expr->location,
                         "reference to undeclared function");
      return;

    FUNC_FOUND:
      for (size_t i = 0; i < arrlenu(expr->children); i++) {
        analyze_expr(expr->children[i], variable_decls, function_decls, d);
        if (should_exit(d)) {
          return;
        }
      }
      break;
    default:
      return;
  }
}

static void analyze_block(ast_node_t* node,
                          variable_t* reserved,
                          variable_t** assigned,
                          ast_node_t** variable_decls,
                          ast_node_t** function_decls,
                          diagnostics_t* d) {
  ast_node_t** local_variable_decls = duplicate_array(variable_decls);

  for (size_t i = 0; i < arrlenu(node->children); i++) {
    ast_node_t* decl = node->children[i];

    if (decl->kind == AST_VARIABLE_DECL) {
      arrput(local_variable_decls, decl);

      if (decl->data.variable_decl.has_variable) {
        if (is_variable_free(decl->data.variable_decl.variable, reserved)) {
          diag_report_source(d, ERROR, &decl->location,
                             "variable not reserved");
          goto CLEANUP;
        }
        continue;
      }

      variable_t var = find_free_variable(reserved, *assigned,
                                          decl->data.variable_decl.type, d);
      if (should_exit(d)) {
        goto CLEANUP;
      }

      arrput(*assigned, var);
      decl->data.variable_decl.has_variable = true;
      decl->data.variable_decl.variable = var;
    } else if (decl->kind == AST_BINARY_EXPRESSION ||
               decl->kind == AST_UNARY_EXPRESSION ||
               decl->kind == AST_IDENTIFIER ||
               decl->kind == AST_FUNCTION_CALL) {
      analyze_expr(decl, local_variable_decls, function_decls, d);
      if (should_exit(d)) {
        goto CLEANUP;
      }
    } else if (decl->kind == AST_BLOCK) {
      analyze_block(decl, reserved, assigned, local_variable_decls,
                    function_decls, d);
      if (should_exit(d)) {
        goto CLEANUP;
      }
    } else if (decl->kind == AST_IF_STATEMENT ||
               decl->kind == AST_WHILE_STATEMENT ||
               decl->kind == AST_RETURN_STATEMENT) {
      for (size_t j = 0; j < arrlenu(decl->children); j++) {
        ast_node_t* child = decl->children[j];

        if (child->kind == AST_BINARY_EXPRESSION ||
            child->kind == AST_UNARY_EXPRESSION ||
            child->kind == AST_IDENTIFIER ||
            child->kind == AST_FUNCTION_CALL) {
          analyze_expr(child, local_variable_decls, function_decls, d);
          if (should_exit(d)) {
            goto CLEANUP;
          }
        } else if (child->kind == AST_BLOCK) {
          analyze_block(child, reserved, assigned, local_variable_decls,
                        function_decls, d);
          if (should_exit(d)) {
            goto CLEANUP;
          }
        }
      }
    }
  }

CLEANUP:
  arrfree(local_variable_decls);
}

void analyze_semantics(ast_node_t* node, diagnostics_t* d) {
  assert(node != NULL);
  assert(d != NULL);

  variable_t* reserved_variables = NULL;
  variable_t* assigned_variables = NULL;
  ast_node_t** variable_decls = NULL;
  ast_node_t** function_decls = NULL;

  // Get all the variable reservations.
  for (size_t i = 0; i < arrlenu(node->children); i++) {
    ast_node_t* decl = node->children[i];

    if (decl->kind == AST_VARIABLE_RESERVATION) {
      variable_t var = decl->data.reserved_variable;
      if (var.type == VAR_LIST && var.id == '0') {
        diag_report_source(d, ERROR, &decl->location, "cannot reserve list 0");
        return;
      }
      arrput(reserved_variables, decl->data.reserved_variable);
    }
  }

  // Assign variables to global variables.
  for (size_t i = 0; i < arrlenu(node->children); i++) {
    ast_node_t* decl = node->children[i];

    if (decl->kind != AST_VARIABLE_DECL) {
      continue;
    }

    arrput(variable_decls, decl);

    if (decl->data.variable_decl.has_variable) {
      if (is_variable_free(decl->data.variable_decl.variable,
                           reserved_variables)) {
        diag_report_source(d, ERROR, &decl->location, "variable not reserved");
        goto CLEANUP;
      }
      continue;
    }

    variable_t var = find_free_variable(reserved_variables, assigned_variables,
                                        decl->data.variable_decl.type, d);
    if (should_exit(d)) {
      goto CLEANUP;
    }

    arrput(assigned_variables, var);
    decl->data.variable_decl.has_variable = true;
    decl->data.variable_decl.variable = var;
  }

  // Assign variables to function parameters and local variables.
  for (size_t i = 0; i < arrlenu(node->children); i++) {
    ast_node_t* decl = node->children[i];

    if (decl->kind != AST_FUNCTION_DECL) {
      continue;
    }

    arrput(function_decls, decl);

    size_t num_children = arrlenu(decl->children);
    assert(num_children > 0);

    ast_node_t* parameters = NULL;
    ast_node_t* body = NULL;

    if (num_children == 2) {
      parameters = decl->children[0];
      assert(parameters->kind == AST_PARAMETER_LIST);
      body = decl->children[1];
      assert(body->kind == AST_BLOCK);
    } else if (num_children == 1) {
      body = decl->children[0];
      assert(body->kind == AST_BLOCK);
    } else {
      assert(false);
    }

    ast_node_t** local_variable_decls = duplicate_array(variable_decls);

    if (parameters != NULL) {
      for (size_t j = 0; j < arrlenu(parameters->children); j++) {
        ast_node_t* param = parameters->children[j];
        assert(param->kind == AST_PARAMETER);
        arrput(local_variable_decls, param);
        variable_t var =
            find_free_variable(reserved_variables, assigned_variables,
                               param->data.variable_decl.type, d);
        if (should_exit(d)) {
          arrfree(local_variable_decls);
          goto CLEANUP;
        }

        arrput(assigned_variables, var);
        param->data.variable_decl.has_variable = true;
        param->data.variable_decl.variable = var;
      }
    }

    /* print_local_variables(local_variable_decls); */
    analyze_block(body, reserved_variables, &assigned_variables,
                  local_variable_decls, function_decls, d);

    arrfree(local_variable_decls);
    if (should_exit(d)) {
      goto CLEANUP;
    }
  }

CLEANUP:
  arrfree(function_decls);
  arrfree(variable_decls);
  arrfree(reserved_variables);
  arrfree(assigned_variables);
}

static variable_t find_free_number(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d);
static variable_t find_free_string(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d);
static variable_t find_free_matrix(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d);
static variable_t find_free_list(variable_t* reserved,
                                 variable_t* assigned,
                                 diagnostics_t* d);

static variable_t find_free_variable(variable_t* reserved,
                                     variable_t* assigned,
                                     variable_type_t type,
                                     diagnostics_t* d) {
  switch (type) {
    case VAR_NUMBER:
      return find_free_number(reserved, assigned, d);
    case VAR_STRING:
      return find_free_string(reserved, assigned, d);
    case VAR_MATRIX:
      return find_free_matrix(reserved, assigned, d);
    case VAR_LIST:
      return find_free_list(reserved, assigned, d);
    default:
      assert(false);
  }

  return (variable_t){.type = VAR_UNKNOWN};
}

static variable_t find_free_number(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d) {
  for (char v = 'A'; v <= 'Z'; v++) {
    variable_t var = {.type = VAR_NUMBER, .id = v};
    if (is_variable_free(var, reserved) && is_variable_free(var, assigned)) {
      return var;
    }
  }

  // Use list 0

  for (char n = 0; n < 100; n++) {
    variable_t var = {.type = VAR_LIST_ELEMENT, .id = n};
    if (is_variable_free(var, assigned)) {
      return var;
    }
  }

  diag_report(d, ERROR, "no free number variables");
  return (variable_t){.type = VAR_UNKNOWN};
}

static variable_t find_free_string(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d) {
  for (char v = '0'; v <= '9'; v++) {
    variable_t var = {.type = VAR_STRING, .id = v};
    if (is_variable_free(var, reserved) && is_variable_free(var, assigned)) {
      return var;
    }
  }

  diag_report(d, ERROR, "no free string variables");
  return (variable_t){.type = VAR_UNKNOWN};
}

static variable_t find_free_matrix(variable_t* reserved,
                                   variable_t* assigned,
                                   diagnostics_t* d) {
  for (char v = 'A'; v <= 'J'; v++) {
    variable_t var = {.type = VAR_MATRIX, .id = v};
    if (is_variable_free(var, reserved) && is_variable_free(var, assigned)) {
      return var;
    }
  }

  diag_report(d, ERROR, "no free matrix variables");
  return (variable_t){.type = VAR_UNKNOWN};
}

static variable_t find_free_list(variable_t* reserved,
                                 variable_t* assigned,
                                 diagnostics_t* d) {
  for (char v = '1'; v <= '9'; v++) {
    variable_t var = {.type = VAR_LIST, .id = v};
    if (is_variable_free(var, reserved) && is_variable_free(var, assigned)) {
      return var;
    }
  }

  diag_report(d, ERROR, "no free list variables");
  return (variable_t){.type = VAR_UNKNOWN};
}

