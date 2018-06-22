#ifndef __basefunc_h_
#define __basefunc_h_

#include "fpar.h"

void base_add (node_t *n);
void base_sub (node_t *n);
void base_mult (node_t *n);
void base_div (node_t *n);
void base_neg (node_t *n);
void base_id (node_t *n);
void base_iddiv (node_t *n);
void base_idmul (node_t *n);

/*
  nicht benötigt für die VMB CRTL
  
void base_sqrt (node_t *n);
void base_sin (node_t *n);
void base_cos (node_t *n);
void base_tan (node_t *n);
void base_exp (node_t *n);
void base_ln  (node_t *n);

void base_pow (node_t *n);

void base_x (node_t *n); */

#endif
