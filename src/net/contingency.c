/** @file contingency.c
 *  @brief This file defines the Cont data structure and its associated methods.
 *
 * This file is part of PFNET.
 *
 * Copyright (c) 2015-2016, Tomas Tinoco De Rubira.
 *
 * PFNET is released under the BSD 2-clause license.
 */

#include <pfnet/contingency.h>
#include <pfnet/branch.h>
#include <pfnet/gen.h>
#include <pfnet/bus.h>

// Gen outage
struct Gen_outage {
  Gen* gen;
  Bus* bus;
  Bus* reg_bus;
  struct Gen_outage* next;
};

// Branch outage
struct Branch_outage {
  Branch* br;
  Bus* bus_k;
  Bus* bus_m;
  Bus* reg_bus;
  char br_type;
  struct Branch_outage* next;
};

// Types
typedef struct Gen_outage Gen_outage;
typedef struct Branch_outage Branch_outage;

// Contingency
struct Cont {

  // Output
  char output_string[CONT_BUFFER_SIZE]; /**< @brief Output string */

  // Generator outages
  Gen_outage* gen_outage;           /**< @brief List of generator outages */

  // Branch outages
  Branch_outage* br_outage;           /**< @brief List of branch outages */
};


void CONT_apply(Cont* cont) {

  // Local variables
  Gen_outage* go;
  Branch_outage* bo;

  if (cont) {

    // Generators
    for (go = cont->gen_outage; go != NULL; go = go->next) {

      // Outage flag
      GEN_set_outage(go->gen,TRUE);

      // Connection
      GEN_set_bus(go->gen,NULL);    // disconnect bus from gen
      BUS_del_gen(go->bus,go->gen); // disconnect gen from bus

      // Regulation
      GEN_set_reg_bus(go->gen,NULL);        // gen does not regulate reg_bus
      BUS_del_reg_gen(go->reg_bus,go->gen); // reg_bus is not regulated by gen
    }

    // Branches
    for (bo = cont->br_outage; bo != NULL; bo = bo->next) {

      // Outage flag
      BRANCH_set_outage(bo->br,TRUE);

      // Connection
      BRANCH_set_bus_k(bo->br,NULL);            // disconnect bus_k from branch
      BRANCH_set_bus_m(bo->br,NULL);           // disconnect bus_m from branch
      BUS_del_branch_k(bo->bus_k,bo->br);       // disconnect branch from bus_k
      BUS_del_branch_m(bo->bus_m,bo->br);     // disconnect branch from bus_m

      // Regulation
      BRANCH_set_reg_bus(bo->br,NULL);      // branch does not regulate reg_bus
      BUS_del_reg_tran(bo->reg_bus,bo->br); // reg_bus is not regulated by branch

      // Type
      if (BRANCH_get_type(bo->br) != BRANCH_TYPE_LINE)
	BRANCH_set_type(bo->br,BRANCH_TYPE_TRAN_FIXED);
    }
  }
}

void CONT_clear(Cont* cont) {

  // Local variables
  Gen_outage* go;
  Branch_outage* bo;

  if (cont) {

    // Generators
    for (go = cont->gen_outage; go != NULL; go = go->next) {

      // Outage flag
      GEN_set_outage(go->gen,FALSE);

      // Connections
      GEN_set_bus(go->gen,go->bus); // connect bus to gen
      BUS_add_gen(go->bus,go->gen); // connect gen to bus

      // Regulation
      GEN_set_reg_bus(go->gen,go->reg_bus); // gen does regulates reg_bus
      BUS_add_reg_gen(go->reg_bus,go->gen); // reg_bus is regulated by gen
    }

    // Branches
    for (bo = cont->br_outage; bo != NULL; bo = bo->next) {

      // Outage flag
      BRANCH_set_outage(bo->br,FALSE);

      // Connection
      BRANCH_set_bus_k(bo->br,bo->bus_k);     // connect bus_k from branch
      BRANCH_set_bus_m(bo->br,bo->bus_m);     // connect bus_m from branch
      BUS_add_branch_k(bo->bus_k,bo->br);     // connect branch from bus_k
      BUS_add_branch_m(bo->bus_m,bo->br);     // connect branch from bus_m

      // Regulation
      BRANCH_set_reg_bus(bo->br,bo->reg_bus); // branch regulates reg_bus
      BUS_add_reg_tran(bo->reg_bus,bo->br);   // reg_bus is regulated by branch

      // Type
      BRANCH_set_type(bo->br,bo->br_type);
    }
  }
}

void CONT_init(Cont* cont) {
  if (cont) {
    strcpy(cont->output_string,"");
    cont->gen_outage = NULL;
    cont->br_outage = NULL;
  }
}

void CONT_del(Cont* cont) {
  if (cont){
    LIST_map(Gen_outage,cont->gen_outage,go,next,{free(go);});
    LIST_map(Branch_outage,cont->br_outage,bo,next,{free(bo);});
    free(cont);
  }
}

int CONT_get_num_gen_outages(Cont* cont) {
  int len;
  if (cont) {
    LIST_len(Gen_outage,cont->gen_outage,next,len);
    return len;
  }
  else
    return 0;
}

int CONT_get_num_branch_outages(Cont* cont) {
  int len;
  if (cont) {
    LIST_len(Branch_outage,cont->br_outage,next,len);
    return len;
  }
  else
    return 0;
}

void CONT_add_gen_outage(Cont* cont, Gen* gen) {
  Gen_outage* go;
  if (cont) {
    for (go = cont->gen_outage; go != NULL; go = go->next) {
      if (go->gen == gen)
    	return;
    }
    go = (Gen_outage*)malloc(sizeof(Gen_outage));
    go->gen = gen;
    go->bus = GEN_get_bus(gen);
    go->reg_bus = GEN_get_reg_bus(gen);
    go->next = NULL;
    LIST_add(Gen_outage,cont->gen_outage,go,next);
  }
}

void CONT_add_branch_outage(Cont* cont, Branch* br) {
  Branch_outage* bo;
  if (cont) {
    for (bo = cont->br_outage; bo != NULL; bo = bo->next) {
      if (bo->br == br)
	return;
    }
    bo = (Branch_outage*)malloc(sizeof(Branch_outage));
    bo->br = br;
    bo->bus_k = BRANCH_get_bus_k(br);
    bo->bus_m = BRANCH_get_bus_m(br);
    bo->reg_bus = BRANCH_get_reg_bus(br);
    bo->br_type = BRANCH_get_type(br);
    bo->next = NULL;
    LIST_add(Branch_outage,cont->br_outage,bo,next);
  }
}

BOOL CONT_has_gen_outage(Cont* cont, Gen* gen) {
  Gen_outage* go;
  if (!cont)
    return FALSE;
  for (go = cont->gen_outage; go != NULL; go = go->next) {
    if (gen == go->gen)
      return TRUE;
  }
  return FALSE;
}

BOOL CONT_has_branch_outage(Cont* cont, Branch* br) {
  Branch_outage* bo;
  if (!cont)
    return FALSE;
  for (bo = cont->br_outage; bo != NULL; bo = bo->next) {
    if (br == bo->br)
      return TRUE;
  }
  return FALSE;
}

Cont* CONT_new(void) {
  Cont* cont = (Cont*)malloc(sizeof(Cont));
  CONT_init(cont);
  return cont;
}

char* CONT_get_show_str(Cont* cont) {

  Gen_outage* go;
  Branch_outage* bo;
  char* out;

  if (!cont)
    return NULL;

  out = cont->output_string;
  strcpy(out,"");

  sprintf(out+strlen(out),"\nGenerator outages\n");
  for (go = cont->gen_outage; go != NULL; go = go->next)
    sprintf(out+strlen(out),"index %d\n",GEN_get_index(go->gen));
  sprintf(out+strlen(out),"\nBranch outages\n");
  for (bo = cont->br_outage; bo != NULL; bo = bo->next)
    sprintf(out+strlen(out),"index %d\n",BRANCH_get_index(bo->br));

  return out;
}

void CONT_show(Cont* cont) {

  printf("%s",CONT_get_show_str(cont));
}
