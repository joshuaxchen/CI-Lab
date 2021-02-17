/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * eval.c - This file contains the skeleton of functions to be implemented by
 * you. When completed, it will contain the code used to evaluate an expression
 * based on its AST.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/

#include "ci.h"

extern bool is_binop(token_t);
extern bool is_unop(token_t);
char *strrev(char *str);

/* infer_type() - set the type of a non-root node based on the types of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated.
 * (STUDENT TODO)
 */

static void infer_type(node_t *nptr)
{
    if (nptr)
    {
        for (int i = 0; i < 3; i++)
        {
            infer_type(nptr->children[i]);
        }
        entry_t *entry;
        switch (nptr->tok)
        {
        case TOK_PLUS:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE)
            {
                nptr->type = INT_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE)
            {
                nptr->type = STRING_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_BMINUS:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE)
            {
                nptr->type = INT_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_TIMES:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = INT_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == INT_TYPE)
            {
                nptr->type = STRING_TYPE;
            } 
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_DIV:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = INT_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_MOD:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = INT_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_AND:
            if (nptr->children[0]->type == BOOL_TYPE && nptr->children[1]->type == BOOL_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_OR:
            if (nptr->children[0]->type == BOOL_TYPE && nptr->children[1]->type == BOOL_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_LT:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = BOOL_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_GT:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = BOOL_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_EQ:
            if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) 
            {
                nptr->type = BOOL_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_UMINUS:
            if (nptr->children[0]->type == INT_TYPE)
            {
                nptr->type = INT_TYPE;
            }
            else if (nptr->children[0]->type == STRING_TYPE)
            {
                nptr->type = STRING_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_NOT:
            if (nptr->children[0]->type == BOOL_TYPE)
            {
                nptr->type = BOOL_TYPE;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_QUESTION:
            if (nptr->children[0]->type == BOOL_TYPE && nptr->children[1]->type == nptr->children[2]->type) 
            {
                nptr->type = nptr->children[1]->type;
            }
            else
            {
                handle_error(ERR_TYPE);
            }
            break;
        case TOK_ID:
            entry = get(nptr->val.sval);
            if (entry != NULL) {
                nptr->type = entry->type;
            } else {
                handle_error(ERR_UNDEFINED);
            }
            break;
        default:
            break;
        }
        return;
    }
}

/* infer_root() - set the type of the root node based on the types of children
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated. 
 */

static void infer_root(node_t *nptr)
{
    if (nptr == NULL)
        return;
    // check running status
    if (terminate || ignore_input)
        return;

    // check for assignment
    if (nptr->type == ID_TYPE)
    {
        infer_type(nptr->children[1]);
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            infer_type(nptr->children[i]);
        }
        if (nptr->children[0] == NULL)
        {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        nptr->type = nptr->children[0]->type;
    }
    return;
}

/* eval_node() - set the value of a non-root node based on the values of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The val field of the node is updated.
 * (STUDENT TODO) 
 */

static void eval_node(node_t *nptr)
{
    if (nptr)
    {
        if (nptr->tok == TOK_QUESTION || nptr->tok == TOK_AND || nptr->tok == TOK_OR) {
            eval_node(nptr->children[0]);
        } else {
            for (int i = 0; i < 3; i++)
            {
                eval_node(nptr->children[i]);
            }
        }
        entry_t *entry;
        switch (nptr->tok)
        {
            case TOK_PLUS:
                if (nptr->type == INT_TYPE) 
                {
                    nptr->val.ival = nptr->children[0]->val.ival + nptr->children[1]->val.ival;
                } 
                else 
                {
                    nptr->val.sval = calloc(1, strlen(nptr->children[0]->val.sval) + strlen(nptr->children[1]->val.sval) + 1);
                    nptr->val.sval[0] = '\0';
                    strcpy(nptr->val.sval, nptr->children[0]->val.sval);
                    strcat(nptr->val.sval, nptr->children[1]->val.sval);
                }
                break;
            case TOK_BMINUS:
                nptr->val.ival = nptr->children[0]->val.ival - nptr->children[1]->val.ival;
                break;
            case TOK_TIMES:
                if (nptr->type == INT_TYPE) {
                    nptr->val.ival = nptr->children[0]->val.ival * nptr->children[1]->val.ival;
                } else {
                    if (nptr->children[1]->val.ival > 0) {
                        nptr->val.sval = calloc(1, strlen(nptr->children[0]->val.sval) * nptr->children[1]->val.ival + 1);
                        nptr->val.sval[0] = '\0';
                        for (int i = 0; i < nptr->children[1]->val.ival;i++) {
                            strcat(nptr->val.sval, nptr->children[0]->val.sval);
                        }
                    } else {
                        handle_error(ERR_EVAL);
                    }
                }
                break;
            case TOK_DIV:
                if (nptr->children[1]->val.ival != 0) {
                    nptr->val.ival = nptr->children[0]->val.ival / nptr->children[1]->val.ival;
                } else {
                    handle_error(ERR_EVAL);
                }
                break;
            case TOK_MOD:
                if (nptr->children[1]->val.ival != 0) {
                    nptr->val.ival = nptr->children[0]->val.ival % nptr->children[1]->val.ival;
                } else {
                    handle_error(ERR_EVAL);
                }
                break;
            case TOK_AND:
                if (nptr->children[0]->val.bval && nptr->children[1]->val.bval) {
                    nptr->val.bval = true;
                } else {
                    nptr->val.bval = false;
                }
                break;
            case TOK_OR:
                if (nptr->children[0]->val.bval || nptr->children[1]->val.bval) {
                    nptr->val.bval = true;
                } else {
                    nptr->val.bval = false;
                }
                break;
            case TOK_LT:
                if (nptr->children[0]->type == INT_TYPE) {
                    nptr->val.bval = nptr->children[0]->val.bval < nptr->children[1]->val.bval;
                } else {
                    int i = strcmp(nptr->children[0]->val.sval, nptr->children[1]->val.sval);
                    if (i < 0) {
                        nptr->val.bval = true;
                    } else {
                        nptr->val.bval = false;
                    }
                }
                
                break;
            case TOK_GT:
                if (nptr->children[0]->type == INT_TYPE) {
                    nptr->val.bval = nptr->children[0]->val.bval > nptr->children[1]->val.bval;
                } else {
                    int i = strcmp(nptr->children[0]->val.sval, nptr->children[1]->val.sval);
                    if (i > 0) {
                        nptr->val.bval = true;
                    } else {
                        nptr->val.bval = false;
                    }
                }
                
                break;
            case TOK_EQ:
                if (nptr->children[0]->type == INT_TYPE) {
                    nptr->val.bval = nptr->children[0]->val.bval == nptr->children[1]->val.bval;
                } else {
                    int i = strcmp(nptr->children[0]->val.sval, nptr->children[1]->val.sval);
                    if (i == 0) {
                        nptr->val.bval = true;
                    } else {
                        nptr->val.bval = false;
                    }
                }
                break;
            case TOK_UMINUS:
                if (nptr->type == INT_TYPE) {
                    nptr->val.ival = -1 * nptr->children[0]->val.ival;
                } else {
                    nptr->val.sval = calloc(1, sizeof(nptr->children[0]->val.sval));
                    strcpy(nptr->val.sval, strrev(nptr->children[0]->val.sval));
                }
                
                break;
            case TOK_NOT:
                nptr->val.bval = !nptr->children[0]->val.bval;
                break;
            case TOK_QUESTION:
                if (nptr->children[0]->val.bval) {
                    eval_node(nptr->children[1]);
                    if (nptr->children[1]->type == STRING_TYPE) {
                        nptr->val.sval = calloc(1, sizeof(this_token->repr));
                        strcpy(nptr->val.sval, nptr->children[1]->val.sval);
                    } else {
                        nptr->val = nptr->children[1]->val;
                    }
                } else {
                    eval_node(nptr->children[2]);
                    if (nptr->children[2]->type == STRING_TYPE) {
                        nptr->val.sval = calloc(1, sizeof(this_token->repr));
                        strcpy(nptr->val.sval, nptr->children[2]->val.sval);
                    } else {
                        nptr->val = nptr->children[2]->val;
                    }
                }
                break;
            case TOK_ID:
                entry = get(nptr->val.sval);
                if (entry != NULL) {
                    nptr->val = entry->val;
                } else {
                    handle_error(ERR_EVAL);
                }
                break;
            default:
                break;
            }

        return;
    }
}

/* eval_root() - set the value of the root node based on the values of children 
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The val dield of the node is updated. 
 */

void eval_root(node_t *nptr)
{
    if (nptr == NULL)
        return;
    // check running status
    if (terminate || ignore_input)
        return;

    // check for assignment
    if (nptr->type == ID_TYPE)
    {
        eval_node(nptr->children[1]);
        if (terminate || ignore_input)
            return;

        if (nptr->children[0] == NULL)
        {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        put(nptr->children[0]->val.sval, nptr->children[1]);
        return;
    }

    for (int i = 0; i < 2; ++i)
    {
        eval_node(nptr->children[i]);
    }
    if (terminate || ignore_input)
        return;

    if (nptr->type == STRING_TYPE)
    {
        (nptr->val).sval = (char *)malloc(strlen(nptr->children[0]->val.sval) + 1);
        if (!nptr->val.sval)
        {
            logging(LOG_FATAL, "failed to allocate string");
            return;
        }
        strcpy(nptr->val.sval, nptr->children[0]->val.sval);
    }
    else
    {
        nptr->val.ival = nptr->children[0]->val.ival;
    }
    return;
}

/* infer_and_eval() - wrapper for calling infer() and eval() 
 * Parameter: A pointer to a root node.
 * Return value: none.
 * Side effect: The type and val fields of the node are updated. 
 */

void infer_and_eval(node_t *nptr)
{
    infer_root(nptr);
    eval_root(nptr);
    return;
}

/* strrev() - helper function to reverse a given string 
 * Parameter: The string to reverse.
 * Return value: The reversed string. The input string is not modified.
 * (STUDENT TODO)
 */

char *strrev(char *str)
{
    char *newstr = calloc(1, sizeof(str));
    int n = strlen(str); 
  
    for (int i = 0; i < n; i++) 
        newstr[i] = str[n - i - 1]; 
    return newstr;
}