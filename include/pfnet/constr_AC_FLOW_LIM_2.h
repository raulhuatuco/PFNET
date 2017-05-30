/** @file constr_AC_FLOW_LIM_2.h
 *  @brief This file lists the constants and routines associated with the constraint of type AC_FLOW_LIM_2.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2017, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#ifndef __CONSTR_AC_FLOW_LIM_2_HEADER__
#define __CONSTR_AC_FLOW_LIM_2_HEADER__

#include <math.h>
#include "constr.h"

// Function prototypes
Constr* CONSTR_AC_FLOW_LIM_2_new(Net* net);
void CONSTR_AC_FLOW_LIM_2_init(Constr* c);
void CONSTR_AC_FLOW_LIM_2_count_step(Constr* c, Branch* br, int t);
void CONSTR_AC_FLOW_LIM_2_allocate(Constr* c);
void CONSTR_AC_FLOW_LIM_2_clear(Constr* c);
void CONSTR_AC_FLOW_LIM_2_analyze_step(Constr* c, Branch* br, int t);
void CONSTR_AC_FLOW_LIM_2_eval_step(Constr* c, Branch* br, int t, Vec* v, Vec* ve);
void CONSTR_AC_FLOW_LIM_2_store_sens_step(Constr* c, Branch* br, int t, Vec* sA, Vec* sf, Vec* sGu, Vec* sGl);
void CONSTR_AC_FLOW_LIM_2_free(Constr* c);

#endif
