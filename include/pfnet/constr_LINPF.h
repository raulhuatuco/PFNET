/** @file constr_LINPF.h
 *  @brief This file lists the constants and routines associated with the constraint of type LINPF.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2016, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#ifndef __CONSTR_LINPF_HEADER__
#define __CONSTR_LINPF_HEADER__

#include <math.h>
#include "constr.h"

// Function prototypes
void CONSTR_LINPF_init(Constr* c);
void CONSTR_LINPF_count_branch(Constr* c, Branch* b);
void CONSTR_LINPF_allocate(Constr* c);
void CONSTR_LINPF_clear(Constr* c);
void CONSTR_LINPF_analyze_branch(Constr* c, Branch* b);
void CONSTR_LINPF_eval_branch(Constr* c, Branch* b, Vec* var_values);
void CONSTR_LINPF_store_sens_branch(Constr* c, Branch* b, Vec* sA, Vec* sf, Vec* sGu, Vec* sGl);
void CONSTR_LINPF_free(Constr* c);

#endif