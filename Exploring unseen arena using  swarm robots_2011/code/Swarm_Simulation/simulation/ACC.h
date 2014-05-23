/* $*************** KCG Version 6.1.2 (build i5) ****************
** Command: s2c612 -config C:/Users/SKY/Desktop/ACC/KCG\kcg_s2c_config.txt
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */
#ifndef _ACC_H_
#define _ACC_H_

#include "kcg_types.h"

/* ========================  input structure  ====================== */
typedef struct {
  kcg_real /* ACC::LEFT_WHITELINE_VALUE */ LEFT_WHITELINE_VALUE;
  kcg_real /* ACC::MIDDLE_WHITELINE_VALUE */ MIDDLE_WHITELINE_VALUE;
  kcg_real /* ACC::RIGHT_WHITELINE_VALUE */ RIGHT_WHITELINE_VALUE;
  kcg_real /* ACC::FRONT_LEFT_RANGER */ FRONT_LEFT_RANGER;
  kcg_real /* ACC::FRONT_RIGHT_RANGER */ FRONT_RIGHT_RANGER;
  kcg_real /* ACC::LEFT_CORNER_RANGER */ LEFT_CORNER_RANGER;
  kcg_real /* ACC::RIGHT_CORNER_RANGER */ RIGHT_CORNER_RANGER;
  kcg_real /* ACC::FRONT_RIGHT2_RANGER */ FRONT_RIGHT2_RANGER;
  kcg_real /* ACC::FRONT_LEFT2_RANGER */  FRONT_LEFT2_RANGER;
} inC_ACC;

/* ========================  context type  ========================= */
typedef struct {
  /* ---------------------------  outputs  --------------------------- */
  kcg_bool /* ACC::BUZZER_ON */ BUZZER_ON;
  kcg_real /* ACC::FORWARD_SPEED */ FORWARD_SPEED;
  kcg_real /* ACC::TURN_SPEED */ TURN_SPEED;
  /* -----------------------  no local probes  ----------------------- */
  /* -------------------- initialization variables  ------------------ */
  kcg_bool init1;
  kcg_bool init;
  /* ----------------------- local memories  ------------------------- */
  SSM_ST_SMmove_SMACC_Move /* ACC::SMACC::Move::SMmove */ SMmove_state_nxt_SMACC_Move;
  SSM_ST_SMACC /* ACC::SMACC */ SMACC_state_nxt;
  /* -------------------- no sub nodes' contexts  -------------------- */
  /* ----------------- no clocks of observable data ------------------ */
} outC_ACC;

/* ===========  node initialization and cycle functions  =========== */
/** "Title_1" {Title = "Title : Adaptive Cruise Control"} */
/** "Author_1" {Author = "Created by : Hermesh Gupta"} */
/** "Date_1" {Date = "05/09/2011"} */
/* ACC */
extern void ACC(inC_ACC *inC, outC_ACC *outC);

extern void ACC_reset(outC_ACC *outC);

#endif /* _ACC_H_ */
/* $*************** KCG Version 6.1.2 (build i5) ****************
** ACC.h
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */

