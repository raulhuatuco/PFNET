/** @file constr_AC_FLOW_LIM.h
 *  @brief This file lists the constants and routines associated with the constraint of type AC_FLOW_LIM.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2017, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#ifndef __CONSTR_AC_FLOW_LIM_HEADER__
#define __CONSTR_AC_FLOW_LIM_HEADER__

#include <math.h>
#include "constr.h"

// Parameters
#define CONSTR_AC_FLOW_LIM_PARAM 1e-8

// Function prototypes
void CONSTR_AC_FLOW_LIM_init(Constr* c);
void CONSTR_AC_FLOW_LIM_count_step(Constr* c, Branch* br, int t);
void CONSTR_AC_FLOW_LIM_allocate(Constr* c);
void CONSTR_AC_FLOW_LIM_clear(Constr* c);
void CONSTR_AC_FLOW_LIM_analyze_step(Constr* c, Branch* br, int t);
void CONSTR_AC_FLOW_LIM_eval_step(Constr* c, Branch* br, int t, Vec* v);
void CONSTR_AC_FLOW_LIM_store_sens_step(Constr* c, Branch* br, int t, Vec* sA, Vec* sf, Vec* sGu, Vec* sGl);
void CONSTR_AC_FLOW_LIM_free(Constr* c);

#endif
