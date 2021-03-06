/** @file constr_BAT_DYN.c
 *  @brief This file defines the data structure and routines associated with the constraint of type BAT_DYN.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2017, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#include <pfnet/constr_BAT_DYN.h>

Constr* CONSTR_BAT_DYN_new(Net* net) {
  Constr* c = CONSTR_new(net);
  CONSTR_set_func_init(c, &CONSTR_BAT_DYN_init);
  CONSTR_set_func_count_step(c, &CONSTR_BAT_DYN_count_step);
  CONSTR_set_func_allocate(c, &CONSTR_BAT_DYN_allocate);
  CONSTR_set_func_clear(c, &CONSTR_BAT_DYN_clear);
  CONSTR_set_func_analyze_step(c, &CONSTR_BAT_DYN_analyze_step);
  CONSTR_set_func_eval_step(c, &CONSTR_BAT_DYN_eval_step);
  CONSTR_set_func_store_sens_step(c, &CONSTR_BAT_DYN_store_sens_step);
  CONSTR_set_func_free(c, &CONSTR_BAT_DYN_free);
  CONSTR_init(c);
  return c;
}

void CONSTR_BAT_DYN_init(Constr* c) {

  // Init
  CONSTR_set_name(c,"battery dynamics");
  CONSTR_set_data(c,NULL);
}

void CONSTR_BAT_DYN_clear(Constr* c) {

  // Counters
  CONSTR_set_A_nnz(c,0);
  CONSTR_set_A_row(c,0);

  // Flags
  CONSTR_clear_bus_counted(c);
}

void CONSTR_BAT_DYN_count_step(Constr* c, Branch* br, int t) {

  // Local variables
  Bus* buses[2];
  Bus* bus;
  Bat* bat;
  int* A_nnz;
  int* A_row;
  char* bus_counted;
  int i;
  int T;

  // Number of periods
  T = BRANCH_get_num_periods(br);

  // Constr data
  A_nnz = CONSTR_get_A_nnz_ptr(c);
  A_row = CONSTR_get_A_row_ptr(c);
  bus_counted = CONSTR_get_bus_counted(c);

  // Check pointer
  if (!A_nnz || !A_row || !bus_counted)
    return;

  // Check outage
  if (BRANCH_is_on_outage(br))
    return;

  // Bus data
  buses[0] = BRANCH_get_bus_k(br);
  buses[1] = BRANCH_get_bus_m(br);

  // Buses
  for (i = 0; i < 2; i++) {
    
    bus = buses[i];
    
    if (!bus_counted[BUS_get_index(bus)*T+t]) {
      
      // Batteries
      for (bat = BUS_get_bat(bus); bat != NULL; bat = BAT_get_next(bat)) {

	// Variables
 	if (BAT_has_flags(bat,FLAG_VARS,BAT_VAR_E) && BAT_has_flags(bat,FLAG_VARS,BAT_VAR_P)) { // E and P
	  
	  // Initial condition (E_0 = E_init)
	  if (t == 0) {
	    (*A_nnz)++; // E_0
	    (*A_row)++;
	  }
	
	  // Update equation (E_{t+1} - E_t - eta_c Pc_t + (1/eta_d) Pd_t = 0)
	  (*A_nnz)++;   // E_t
	  (*A_nnz)++;   // Pc_t
	  (*A_nnz)++;   // Pd_t
	  if (t < T-1)  // t = T-1 is last time period
	    (*A_nnz)++; // E_{t+1}
	  (*A_row)++;
	}
      }
    }
    
    // Update counted flag
    bus_counted[BUS_get_index(bus)*T+t] = TRUE;
  }
}

void CONSTR_BAT_DYN_allocate(Constr* c) {

  // Local variables
  int num_constr;
  int num_vars;
  int A_nnz;

  num_vars = NET_get_num_vars(CONSTR_get_network(c));
  num_constr = CONSTR_get_A_row(c);
  A_nnz = CONSTR_get_A_nnz(c);

  // J f
  CONSTR_set_J(c,MAT_new(0,num_vars,0));
  CONSTR_set_f(c,VEC_new(0));

  // A b
  CONSTR_set_A(c,MAT_new(num_constr,   // size1 (rows)
			 num_vars,     // size2 (cols)
			 A_nnz));      // nnz
  CONSTR_set_b(c,VEC_new(num_constr));
  
  // G l u
  CONSTR_set_l(c,VEC_new(0));
  CONSTR_set_u(c,VEC_new(0));
  CONSTR_set_G(c,MAT_new(0,        // size1 (rows)
			 num_vars, // size2 (cols)
			 0));      // nnz
}

void CONSTR_BAT_DYN_analyze_step(Constr* c, Branch* br, int t) {

  // Local variables
  Bus* buses[2];
  Bus* bus;
  Bat* bat;
  int* A_nnz;
  int* A_row;
  char* bus_counted;
  Vec* b;
  Mat* A;
  int i;
  int T;

  // Number of periods
  T = BRANCH_get_num_periods(br);

  // Cosntr data
  b = CONSTR_get_b(c);
  A = CONSTR_get_A(c);
  A_nnz = CONSTR_get_A_nnz_ptr(c);
  A_row = CONSTR_get_A_row_ptr(c);
  bus_counted = CONSTR_get_bus_counted(c);

  // Check pointers
  if (!A_nnz || !A_row || !bus_counted)
    return;

  // Check outage
  if (BRANCH_is_on_outage(br))
    return;

  // Bus data
  buses[0] = BRANCH_get_bus_k(br);
  buses[1] = BRANCH_get_bus_m(br);

  // Buses
  for (i = 0; i < 2; i++) {

    bus = buses[i];

    if (!bus_counted[BUS_get_index(bus)*T+t]) {
      
      // Batteries
      for (bat = BUS_get_bat(bus); bat != NULL; bat = BAT_get_next(bat)) {
	
	// Variables
 	if (BAT_has_flags(bat,FLAG_VARS,BAT_VAR_E) && BAT_has_flags(bat,FLAG_VARS,BAT_VAR_P)) { // E and P
	  
	  // Initial condition (E_0 = E_init)
	  if (t == 0) {
	    VEC_set(b,*A_row,BAT_get_E_init(bat));  
	    MAT_set_i(A,*A_nnz,*A_row);
	    MAT_set_j(A,*A_nnz,BAT_get_index_E(bat,t));
	    MAT_set_d(A,*A_nnz,1.);
	    (*A_nnz)++; // E_0
	    (*A_row)++;
	  }
	
	  // Update equation (E_{t+1} - E_t - eta_c Pc_t + (1/eta_d) Pd_t = 0)
	  MAT_set_i(A,*A_nnz,*A_row);
	  MAT_set_j(A,*A_nnz,BAT_get_index_E(bat,t));
	  MAT_set_d(A,*A_nnz,-1.);
	  (*A_nnz)++;   // E_t

	  MAT_set_i(A,*A_nnz,*A_row);
	  MAT_set_j(A,*A_nnz,BAT_get_index_Pc(bat,t));
	  MAT_set_d(A,*A_nnz,-BAT_get_eta_c(bat));
	  (*A_nnz)++;   // Pc_t

	  MAT_set_i(A,*A_nnz,*A_row);
	  MAT_set_j(A,*A_nnz,BAT_get_index_Pd(bat,t));
	  MAT_set_d(A,*A_nnz,1./BAT_get_eta_d(bat));
	  (*A_nnz)++;   // Pd_t

	  if (t < T-1) {
	    VEC_set(b,*A_row,0.);
	    MAT_set_i(A,*A_nnz,*A_row);
	    MAT_set_j(A,*A_nnz,BAT_get_index_E(bat,t+1));
	    MAT_set_d(A,*A_nnz,1.);
	    (*A_nnz)++; // E_{t+1}
	  }
	  else
	    VEC_set(b,*A_row,-BAT_get_E_final(bat));
	  (*A_row)++;
	}
      }      
    }

    // Update counted flag
    bus_counted[BUS_get_index(bus)*T+t] = TRUE;
  }
}

void CONSTR_BAT_DYN_eval_step(Constr* c, Branch* br, int t, Vec* values, Vec* values_extra) {
  // Nothing to do
}

void CONSTR_BAT_DYN_store_sens_step(Constr* c, Branch* br, int t, Vec* sA, Vec* sf, Vec* sGu, Vec* sGl) {
  // Nothing for now
}

void CONSTR_BAT_DYN_free(Constr* c) {
  // Nothing to do
}
