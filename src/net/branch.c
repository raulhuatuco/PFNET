/** @file branch.c
 *  @brief This file defines the Branch data structure and its associated methods.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2016, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#include <pfnet/branch.h>
#include <pfnet/bus.h>

// Branch
struct Branch {

  // Properties
  char type;         /**< @brief %Branch type */

  // Buses
  Bus* bus_k;        /**< @brief Bus connected to the "k" side */
  Bus* bus_m;        /**< @brief Bus connected to the "m" side */
  Bus* reg_bus;      /**< @brief Bus regulated by this transformer */

  // Conductance
  REAL g;            /**< @brief Series conductance (p.u.) */
  REAL g_k;          /**< @brief %Shunt conductance on "k" side (p.u.) */
  REAL g_m;          /**< @brief %Shunt conductance on "m" side (p.u.) */

  // Susceptance
  REAL b;            /**< @brief Series susceptance (p.u.) */
  REAL b_k;          /**< @brief %Shunt susceptance on "k" side (p.u.) */
  REAL b_m;          /**< @brief %Shunt shunt susceptance "m" side (p.u.) */

  // Tap ratio
  REAL ratio;        /**< @brief Transformer taps ratio (p.u.) */
  REAL ratio_max;    /**< @brief Maximum transformer taps ratio (p.u.) */
  REAL ratio_min;    /**< @brief Minimum transformer taps ratio (p.u.) */
  char num_ratios;   /**< @brief Number of tap positions */

  // Phase shift
  REAL phase;        /**< @brief Transformer phase shift (radians) */
  REAL phase_max;    /**< @brief Maximum transformer phase shift (radians) */
  REAL phase_min;    /**< @brief Minimum transformer phase shift (radians) */

  // Flow bounds
  REAL P_max;        /**< @brief Maximum active power flow (p.u.) */
  REAL P_min;        /**< @brief Minimum active power flow (p.u.) */
  REAL Q_max;        /**< @brief Maximum reactive power flow (p.u.) */
  REAL Q_min;        /**< @brief Minimum reactive power flow (p.u.) */

  // Power ratings
  REAL ratingA;      /**< @brief Power rating A (p.u. system base MVA) */
  REAL ratingB;      /**< @brief Power rating B (p.u. system base MVA) */
  REAL ratingC;      /**< @brief Power rating C (p.u. system base MVA) */

  // Flags
  BOOL outage;           /**< @brief Flag for indicating that branch in on outage */
  BOOL pos_ratio_v_sens; /**< @brief Flag for positive ratio-voltage sensitivity */
  char vars;             /**< @brief Flags for indicating which quantities should be treated as variables */
  char fixed;            /**< @brief Flags for indicating which quantities should be fixed to their current value */
  char bounded;          /**< @brief Flags for indicating which quantities should be bounded */
  char sparse;           /**< @brief Flags for indicating which control adjustments should be sparse */

  // Indices
  int index;         /**< @brief Branch index */
  int index_ratio;   /**< @brief Taps ratio index */
  int index_ratio_y; /**< @brief Taps ratio positive deviation index */
  int index_ratio_z; /**< @brief Taps ratio negative deviation index */
  int index_phase;   /**< @brief Phase shift index */
  int index_P;       /**< @brief Branch active power flow index */
  int index_Q;       /**< @brief Branch reactive power flow index */

  // Sensitivities
  REAL sens_P_u_bound;  /**< @brief Sensitivity of active power flow upper bound */
  REAL sens_P_l_bound;  /**< @brief Sensitivity of active power flow lower bound */

  // List
  Branch* reg_next;   /**< @brief List of branches regulating a bus voltage magnitude */
  Branch* next_k;     /**< @brief List of branches connected to a bus on the "k" side */
  Branch* next_m;     /**< @brief List of branches connected to a bus in the "m" side */
};

void* BRANCH_array_get(void* branch, int index) {
  if (branch)
    return (void*)&(((Branch*)branch)[index]);
  else
    return NULL;
}

Branch* BRANCH_array_new(int num) {
  int i;
  Branch* branch = (Branch*)malloc(sizeof(Branch)*num);
  for (i = 0; i < num; i++) {
    BRANCH_init(&(branch[i]));
    BRANCH_set_index(&(branch[i]),i);
  }
  return branch;
}

void BRANCH_array_show(Branch* branch, int num) {
  int i;
  for (i = 0; i < num; i++)
    BRANCH_show(&(branch[i]));
}

void BRANCH_clear_flags(Branch* br, char flag_type) {
  if (br) {
    if (flag_type == FLAG_VARS)
      br->vars = 0x00;
    else if (flag_type == FLAG_BOUNDED)
      br->bounded = 0x00;
    else if (flag_type == FLAG_FIXED)
      br->fixed = 0x00;
    else if (flag_type == FLAG_SPARSE)
      br->sparse = 0x00;
  }
}

void BRANCH_clear_sensitivities(Branch* br) {
  if (br) {
    br->sens_P_u_bound = 0;
    br->sens_P_l_bound = 0;
  }
}

char BRANCH_get_type(Branch* br) {
  if (br)
    return br->type;
  else
    return BRANCH_TYPE_LINE;
}

char BRANCH_get_obj_type(void* br) {
  if (br)
    return OBJ_BRANCH;
  else
    return OBJ_UNKNOWN;
}

REAL BRANCH_get_sens_P_u_bound(Branch* br) {
  if (br)
    return br->sens_P_u_bound;
  else
    return 0;
}

REAL BRANCH_get_sens_P_l_bound(Branch* br) {
  if (br)
    return br->sens_P_l_bound;
  else
    return 0;
}

int BRANCH_get_index(Branch* br) {
  if (br)
    return br->index;
  else
    return 0;
}

int BRANCH_get_index_ratio(Branch* br) {
  if (br)
    return br->index_ratio;
  else
    return 0;
}

int BRANCH_get_index_ratio_y(Branch* br) {
  if (br)
    return br->index_ratio_y;
  else
    return 0;
}

int BRANCH_get_index_ratio_z(Branch* br) {
  if (br)
    return br->index_ratio_z;
  else
    return 0;
}

int BRANCH_get_index_phase(Branch* br) {
  if (br)
    return br->index_phase;
  else
    return 0;
}

REAL BRANCH_get_ratio(Branch* br) {
  if (br)
    return br->ratio;
  else
    return 0;
}

REAL BRANCH_get_ratio_max(Branch* br) {
  if (br)
    return br->ratio_max;
  else
    return 0;
}

REAL BRANCH_get_ratio_min(Branch* br) {
  if (br)
    return br->ratio_min;
  else
    return 0;
}

REAL BRANCH_get_b(Branch* br) {
  if (br)
    return br->b;
  else
    return 0;
}

REAL BRANCH_get_b_from(Branch* br) {
  return BRANCH_get_b_k(br);
}

REAL BRANCH_get_b_k(Branch* br) {
  if (br)
    return br->b_k;
  else
    return 0;
}

REAL BRANCH_get_b_to(Branch* br) {
  return BRANCH_get_b_m(br);
}

REAL BRANCH_get_b_m(Branch* br) {
  if (br)
    return br->b_m;
  else
    return 0;
}

REAL BRANCH_get_g(Branch* br) {
  if (br)
    return br->g;
  else
    return 0;
}

REAL BRANCH_get_g_from(Branch* br) {
  return BRANCH_get_g_k(br);
}

REAL BRANCH_get_g_k(Branch* br) {
  if (br)
    return br->g_k;
  else
    return 0;
}

REAL BRANCH_get_g_to(Branch* br) {
  return BRANCH_get_g_m(br);
}

REAL BRANCH_get_g_m(Branch* br) {
  if (br)
    return br->g_m;
  else
    return 0;
}

Bus* BRANCH_get_bus_from(Branch* br) {
  return BRANCH_get_bus_k(br);
}

Bus* BRANCH_get_bus_k(Branch* br) {
  if (br)
    return br->bus_k;
  else
    return NULL;
}

Bus* BRANCH_get_bus_to(Branch* br) {
  return BRANCH_get_bus_m(br);
}

Bus* BRANCH_get_bus_m(Branch* br) {
  if (br)
    return br->bus_m;
  else
    return NULL;
}

Bus* BRANCH_get_reg_bus(Branch* br) {
  if (br)
    return br->reg_bus;
  else
    return NULL;
}

Branch* BRANCH_get_reg_next(Branch* br) {
  if (br)
    return br->reg_next;
  else
    return NULL;
}

Branch* BRANCH_get_from_next(Branch* br) {
  return BRANCH_get_next_k(br);
}

Branch* BRANCH_get_next_k(Branch* br) {
  if (br)
    return br->next_k;
  else
    return NULL;
}

Branch* BRANCH_get_to_next(Branch* br) {
  return BRANCH_get_next_m(br);
}

Branch* BRANCH_get_next_m(Branch* br) {
  if (br)
    return br->next_m;
  else
    return NULL;
}

REAL BRANCH_get_phase(Branch* br) {
  if (br)
    return br->phase;
  else
    return 0;
}

REAL BRANCH_get_phase_max(Branch* br) {
  if (br)
    return br->phase_max;
  else
    return 0;
}

REAL BRANCH_get_phase_min(Branch* br) {
  if (br)
    return br->phase_min;
  else
    return 0;
}

// TODO: Branch flow calculations
REAL BRANCH_get_P_km(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_Q_km(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_P_mk(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_Q_mk(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;}

REAL BRANCH_get_P_km_series(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_Q_km_series(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_P_mk_series(Branch* br) {
  if (br)
    return 0;
}

REAL BRANCH_get_Q_mk_series(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_P_k_shunt(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_Q_k_shunt(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_P_m_shunt(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}

REAL BRANCH_get_Q_m_shunt(Branch* br) {
  if (br)
    // TODO fill in the calcluation
  return 0;
}


REAL BRANCH_get_ratingA(Branch* br) {
  if (br)
    return br->ratingA;
  else
    return 0;
}

REAL BRANCH_get_ratingB(Branch* br) {
  if (br)
    return br->ratingB;
  else
    return 0;
}

REAL BRANCH_get_ratingC(Branch* br) {
  if (br)
    return br->ratingC;
  else
    return 0;
}

REAL BRANCH_get_P_flow_DC(Branch* br) {
  /* Active power flow (DC approx) from bus
     "from" to bus "to". */

  if (br) {
    return -(br->b)*(BUS_get_v_ang(br->bus_k)-
		     BUS_get_v_ang(br->bus_m)-
		     br->phase);
  }
  else
    return 0;
}

void BRANCH_get_var_values(Branch* br, Vec* values, int code) {

  // No branch
  if (!br)
    return;

  if (br->vars & BRANCH_VAR_RATIO) { // taps ratio
    switch(code) {
    case UPPER_LIMITS:
      VEC_set(values,br->index_ratio,br->ratio_max);
      break;
    case LOWER_LIMITS:
      VEC_set(values,br->index_ratio,br->ratio_min);
      break;
    default:
      VEC_set(values,br->index_ratio,br->ratio);
    }
  }
  if (br->vars & BRANCH_VAR_PHASE) { // phase shift
    switch(code) {
    case UPPER_LIMITS:
      VEC_set(values,br->index_phase,br->phase_max);
      break;
    case LOWER_LIMITS:
      VEC_set(values,br->index_phase,br->phase_min);
      break;
    default:
      VEC_set(values,br->index_phase,br->phase);
    }
  }
  if (br->vars & BRANCH_VAR_RATIO_DEV) { // tap ratio deviations
    switch(code) {
    case UPPER_LIMITS:
      VEC_set(values,br->index_ratio_y,BRANCH_INF_RATIO);
      VEC_set(values,br->index_ratio_z,BRANCH_INF_RATIO);
      break;
    case LOWER_LIMITS:
      VEC_set(values,br->index_ratio_y,0.);
      VEC_set(values,br->index_ratio_z,0.);
      break;
    default:
      VEC_set(values,br->index_ratio_y,0.);
      VEC_set(values,br->index_ratio_z,0.);
    }
  }
}

Vec* BRANCH_get_var_indices(void* vbr, char var) {
  Branch* br = (Branch*)vbr;
  Vec* indices;
  if (!br)
    return NULL;
  if (var == BRANCH_VAR_RATIO) {
    indices = VEC_new(1);
    VEC_set(indices,0,br->index_ratio);
    return indices;
  }
  if (var == BRANCH_VAR_PHASE) {
    indices = VEC_new(1);
    VEC_set(indices,0,br->index_phase);
    return indices;
  }
  if (var == BRANCH_VAR_RATIO_DEV) {
    indices = VEC_new(2);
    VEC_set(indices,0,br->index_ratio_y);
    VEC_set(indices,1,br->index_ratio_z);
    return indices;
  }
  return NULL;
}

BOOL BRANCH_has_pos_ratio_v_sens(Branch* branch) {
  if (branch)
    return branch->pos_ratio_v_sens;
  else
    return FALSE;
}

BOOL BRANCH_has_flags(void* vbr, char flag_type, char mask) {
  Branch* br = (Branch*)vbr;
  if (br) {
    if (flag_type == FLAG_VARS)
      return (br->vars & mask) == mask;
    else if (flag_type == FLAG_BOUNDED)
      return (br->bounded & mask) == mask;
    else if (flag_type == FLAG_FIXED)
      return (br->fixed & mask) == mask;
    else if (flag_type == FLAG_SPARSE)
      return (br->sparse & mask) == mask;
    return FALSE;
  }
  else
    return FALSE;
}

BOOL BRANCH_has_properties(void* vbr, char prop) {
  Branch* br = (Branch*)vbr;
  if (!br)
    return FALSE;
  if ((prop & BRANCH_PROP_TAP_CHANGER) && !BRANCH_is_tap_changer(br))
    return FALSE;
  if ((prop & BRANCH_PROP_TAP_CHANGER_V) && !BRANCH_is_tap_changer_v(br))
    return FALSE;
  if ((prop & BRANCH_PROP_TAP_CHANGER_Q) && !BRANCH_is_tap_changer_Q(br))
    return FALSE;
  if ((prop & BRANCH_PROP_PHASE_SHIFTER) && !BRANCH_is_phase_shifter(br))
    return FALSE;
  if ((prop & BRANCH_PROP_NOT_OUT) && BRANCH_is_on_outage(br))
    return FALSE;
  return TRUE;
}

void BRANCH_init(Branch* br) {

  br->type = BRANCH_TYPE_LINE;

  br->bus_k = NULL;
  br->bus_m = NULL;
  br->reg_bus = NULL;

  br->g = 0;
  br->g_k = 0;
  br->g_m = 0;
  br->b = 0;
  br->b_k = 0;
  br->b_m = 0;

  br->ratio = 1;
  br->ratio_max = 1;
  br->ratio_min = 1;
  br->num_ratios = 1;

  br->phase = 0;
  br->phase_max = 0;
  br->phase_min = 0;

  br->P_max = 0;
  br->P_min = 0;
  br->Q_max = 0;
  br->Q_min = 0;

  br->ratingA = 0;
  br->ratingB = 0;
  br->ratingC = 0;

  br->outage = FALSE;
  br->pos_ratio_v_sens = TRUE;
  br->vars = 0x00;
  br->fixed = 0x00;
  br->bounded = 0x00;
  br->sparse = 0x00;

  br->index = 0;
  br->index_ratio = 0;
  br->index_ratio_y = 0;
  br->index_ratio_z = 0;
  br->index_phase = 0;
  br->index_P = 0;
  br->index_Q = 0;

  br->sens_P_u_bound = 0;
  br->sens_P_l_bound = 0;

  br->reg_next = NULL;
  br->next_k = NULL;
  br->next_m = NULL;
};

BOOL BRANCH_is_equal(Branch* br, Branch* other) {
  return br == other;
}

BOOL BRANCH_is_on_outage(Branch* br) {
  if (br)
    return br->outage;
  else
    return FALSE;
}

BOOL BRANCH_is_fixed_tran(Branch* br) {
  if (br)
    return br->type == BRANCH_TYPE_TRAN_FIXED;
  else
    return FALSE;
}

BOOL BRANCH_is_line(Branch* br) {
  if (br)
    return br->type == BRANCH_TYPE_LINE;
  else
    return FALSE;
}

BOOL BRANCH_is_phase_shifter(Branch* br) {
  if (br)
    return br->type == BRANCH_TYPE_TRAN_PHASE;
  else
    return FALSE;
}

BOOL BRANCH_is_tap_changer(Branch* br) {
  if (br)
    return BRANCH_is_tap_changer_v(br) || BRANCH_is_tap_changer_Q(br);
  else
    return FALSE;
}

BOOL BRANCH_is_tap_changer_v(Branch* br) {
  if (br)
    return (br->type == BRANCH_TYPE_TRAN_TAP_V);
  else
    return FALSE;
}

BOOL BRANCH_is_tap_changer_Q(Branch* br) {
  if (br)
    return (br->type == BRANCH_TYPE_TRAN_TAP_Q);
  else
    return FALSE;
}

Branch* BRANCH_list_reg_add(Branch* reg_br_list, Branch* reg_br) {
  LIST_add(Branch,reg_br_list,reg_br,reg_next);
  return reg_br_list;
}

Branch* BRANCH_list_reg_del(Branch* reg_br_list, Branch* reg_br) {
  LIST_del(Branch,reg_br_list,reg_br,reg_next);
  return reg_br_list;
}

int BRANCH_list_reg_len(Branch* reg_br_list) {
  int len;
  LIST_len(Branch,reg_br_list,reg_next,len);
  return len;
}

Branch* BRANCH_list_from_add(Branch* from_br_list, Branch* br) {
  return BRANCH_list_k_add(from_br_list, br);
}

Branch* BRANCH_list_k_add(Branch* k_br_list, Branch* br) {
  LIST_add(Branch,k_br_list,br,next_k);
  return k_br_list;
}

Branch* BRANCH_list_from_del(Branch* from_br_list, Branch* br) {
  return BRANCH_list_k_del(from_br_list, br);
}

Branch* BRANCH_list_k_del(Branch* k_br_list, Branch* br) {
  LIST_del(Branch,k_br_list,br,next_k);
  return k_br_list;
}

int BRANCH_list_from_len(Branch* from_br_list) {
  return BRANCH_list_k_len(from_br_list);
}

int BRANCH_list_k_len(Branch* k_br_list) {
  int len;
  LIST_len(Branch,k_br_list,next_k,len);
  return len;
}

Branch* BRANCH_list_to_add(Branch* to_br_list, Branch* br) {
  return BRANCH_list_m_add(to_br_list,br);
}

Branch* BRANCH_list_m_add(Branch* m_br_list, Branch* br) {
  LIST_add(Branch,m_br_list,br,next_m);
  return m_br_list;
}

Branch* BRANCH_list_to_del(Branch* to_br_list, Branch* br) {
  return BRANCH_list_m_del(to_br_list,br);
}

Branch* BRANCH_list_m_del(Branch* m_br_list, Branch* br) {
  LIST_del(Branch,m_br_list,br,next_m);
  return m_br_list;
}

int BRANCH_list_to_len(Branch* to_br_list) {
  return BRANCH_list_m_len(to_br_list);
}

int BRANCH_list_m_len(Branch* m_br_list) {
  int len;
  LIST_len(Branch,m_br_list,next_m,len);
  return len;
}

Branch* BRANCH_new(void) {
  Branch* branch = (Branch*)malloc(sizeof(Branch));
  BRANCH_init(branch);
  return branch;
}

void BRANCH_set_sens_P_u_bound(Branch* br, REAL value) {
  if (br)
    br->sens_P_u_bound = value;
}

void BRANCH_set_sens_P_l_bound(Branch* br, REAL value) {
  if (br)
    br->sens_P_l_bound = value;
}

void BRANCH_set_index(Branch* br, int index) {
  if (br)
    br->index = index;
}

void BRANCH_set_type(Branch* br, int type) {
  if (br)
    br->type = type;
}

void BRANCH_set_bus_from(Branch* br, Bus* bus_from) {
  BRANCH_set_bus_k(br, bus_from);
}

void BRANCH_set_bus_k(Branch* br, Bus* bus_k) {
  if (br)
    br->bus_k = bus_k;
}

void BRANCH_set_bus_to(Branch* br, Bus* bus_to) {
  BRANCH_set_bus_m(br, bus_to);
}

void BRANCH_set_bus_m(Branch* br, Bus* bus_m) {
  if (br)
    br->bus_m = bus_m;
}

void BRANCH_set_reg_bus(Branch* br, Bus* reg_bus) {
  if (br)
    br->reg_bus = reg_bus;
}

void BRANCH_set_g(Branch* br, REAL g) {
  if (br)
    br->g = g;
}

void BRANCH_set_g_from(Branch* br, REAL g_from) {
  BRANCH_set_g_k(br, g_from);
}

void BRANCH_set_g_k(Branch* br, REAL g_k) {
  if (br)
    br->g_k = g_k;
}

void BRANCH_set_g_to(Branch* br, REAL g_to) {
  BRANCH_set_g_m(br, g_to);
}

void BRANCH_set_g_m(Branch* br, REAL g_m) {
  if (br)
    br->g_m = g_m;
}

void BRANCH_set_b(Branch* br, REAL b) {
  if (br)
    br->b = b;
}

void BRANCH_set_b_from(Branch* br, REAL b_from) {
  BRANCH_set_b_k(br, b_from);
}

void BRANCH_set_b_k(Branch* br, REAL b_k) {
  if (br)
    br->b_k = b_k;
}

void BRANCH_set_b_to(Branch* br, REAL b_to) {
  BRANCH_set_b_m(br, b_to);
}

void BRANCH_set_b_m(Branch* br, REAL b_m) {
  if (br)
    br->b_m = b_m;
}

void BRANCH_set_ratio(Branch* br, REAL ratio) {
  if (br)
    br->ratio = ratio;
}

void BRANCH_set_ratio_max(Branch* br, REAL ratio) {
  if (br)
    br->ratio_max = ratio;
}

void BRANCH_set_ratio_min(Branch* br, REAL ratio) {
  if (br)
    br->ratio_min = ratio;
}

void BRANCH_set_pos_ratio_v_sens(Branch* br, BOOL flag) {
  if (br)
    br->pos_ratio_v_sens = flag;
}

void BRANCH_set_outage(Branch* br, BOOL outage) {
  if (br)
    br->outage = outage;
}

void BRANCH_set_phase(Branch* br, REAL phase) {
  if (br)
    br->phase = phase;
}

void BRANCH_set_phase_max(Branch* br, REAL phase) {
  if (br)
    br->phase_max = phase;
}

void BRANCH_set_phase_min(Branch* br, REAL phase) {
  if (br)
    br->phase_min = phase;
}

void BRANCH_set_P_max(Branch* br, REAL P_max) {
  if (br)
    br->P_max = P_max;
}

void BRANCH_set_P_min(Branch* br, REAL P_min) {
  if (br)
    br->P_min = P_min;
}

void BRANCH_set_Q_max(Branch* br, REAL Q_max) {
  if (br)
    br->Q_max = Q_max;
}

void BRANCH_set_Q_min(Branch* br, REAL Q_min) {
  if (br)
    br->Q_min = Q_min;
}

void BRANCH_set_ratingA(Branch* br, REAL r) {
  if (br)
    br->ratingA = r;
}

void BRANCH_set_ratingB(Branch* br, REAL r) {
  if (br)
    br->ratingB = r;
}

void BRANCH_set_ratingC(Branch* br, REAL r) {
  if (br)
    br->ratingC = r;
}

void BRANCH_set_var_values(Branch* br, Vec* values) {

  // No branch
  if (!br)
    return;

  // Set variable values
  if (br->vars & BRANCH_VAR_RATIO)    // taps ratio
    br->ratio = VEC_get(values,br->index_ratio);
  if (br->vars & BRANCH_VAR_PHASE)    // phase shift
    br->phase = VEC_get(values,br->index_phase);
}

int BRANCH_set_flags(void* vbr, char flag_type, char mask, int index) {

  // Local variables
  char* flags_ptr = NULL;
  Branch* br = (Branch*)vbr;

  // Check branch
  if (!br)
    return index;

  // Set flag pointer
  if (flag_type == FLAG_VARS)
    flags_ptr = &(br->vars);
  else if (flag_type == FLAG_FIXED)
    flags_ptr = &(br->fixed);
  else if (flag_type == FLAG_BOUNDED)
    flags_ptr = &(br->bounded);
  else if (flag_type == FLAG_SPARSE)
    flags_ptr = &(br->sparse);
  else
    return index;

  // Set flags
  if (!((*flags_ptr) & BRANCH_VAR_RATIO) && (mask & BRANCH_VAR_RATIO)) { // taps ratio
    if (flag_type == FLAG_VARS)
      br->index_ratio = index;
    (*flags_ptr) |= BRANCH_VAR_RATIO;
    index++;
  }
  if (!((*flags_ptr) & BRANCH_VAR_PHASE) && (mask & BRANCH_VAR_PHASE)) { // phase shift
    if (flag_type == FLAG_VARS)
      br->index_phase = index;
    (*flags_ptr) |= BRANCH_VAR_PHASE;
    index++;
  }
  if (!((*flags_ptr) & BRANCH_VAR_RATIO_DEV) && (mask & BRANCH_VAR_RATIO_DEV)) { // taps ratio deviations
    if (flag_type == FLAG_VARS) {
      br->index_ratio_y = index;
      br->index_ratio_z = index+1;
    }
    (*flags_ptr) |= BRANCH_VAR_RATIO_DEV;
    index += 2;
  }
  return index;
}

void BRANCH_show(Branch* br) {
  printf("branch %d\t%d\t%d\n",
	 BUS_get_number(br->bus_k),
	 BUS_get_number(br->bus_m),
	 br->type);
}
