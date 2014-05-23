/* $*************** KCG Version 6.1.2 (build i5) ****************
** Command: s2c612 -config C:/Users/SKY/Desktop/ACC/KCG\kcg_s2c_config.txt
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */
#ifndef _KCG_TYPES_H_
#define _KCG_TYPES_H_

#define KCG_MAPW_CPY

#include "./user_macros.h"

#ifndef kcg_int
#define kcg_int kcg_int
typedef int kcg_int;
#endif /* kcg_int */

#ifndef kcg_bool
#define kcg_bool kcg_bool
typedef unsigned char kcg_bool;
#endif /* kcg_bool */

#ifndef kcg_real
#define kcg_real kcg_real
typedef double kcg_real;
#endif /* kcg_real */

#ifndef kcg_char
#define kcg_char kcg_char
typedef unsigned char kcg_char;
#endif /* kcg_char */

#ifndef kcg_false
#define kcg_false ((kcg_bool) 0)
#endif /* kcg_false */

#ifndef kcg_true
#define kcg_true ((kcg_bool) 1)
#endif /* kcg_true */

#ifndef kcg_assign
//#include "kcg_assign.h"
#endif /* kcg_assign */

#ifndef kcg_assign_struct
#define kcg_assign_struct kcg_assign
#endif /* kcg_assign_struct */

#ifndef kcg_assign_array
#define kcg_assign_array kcg_assign
#endif /* kcg_assign_array */

/* truthtables::TruthTableValues */
typedef enum {
  T_truthtables,
  F_truthtables,
  X_truthtables
} TruthTableValues_truthtables;
/* ACC::SMACC */
typedef enum {
  SSM_TR_no_trans_SMACC,
  SSM_TR_Stop_1_SMACC,
  SSM_TR_Move_1_SMACC
} SSM_TR_SMACC;
/* ACC::SMACC */
typedef enum { SSM_st_Stop_SMACC, SSM_st_Move_SMACC } SSM_ST_SMACC;
/* ACC::SMACC::Move::SMmove */
typedef enum {
  SSM_TR_no_trans_SMmove_SMACC_Move,
  SSM_TR_Forward_1_SMmove_SMACC_Move,
  SSM_TR_Forward_2_SMmove_SMACC_Move,
  SSM_TR_LeftMove_1_SMmove_SMACC_Move,
  SSM_TR_RightMove_1_SMmove_SMACC_Move
} SSM_TR_SMmove_SMACC_Move;
/* ACC::SMACC::Move::SMmove */
typedef enum {
  SSM_st_Forward_SMACC_Move_SMmove,
  SSM_st_LeftMove_SMACC_Move_SMmove,
  SSM_st_RightMove_SMACC_Move_SMmove,
  SSM_st_BackMove_SMACC_Move_SMmove
} SSM_ST_SMmove_SMACC_Move;

typedef struct { kcg_int k; kcg_real f; } struct__4927;

/* lut::LutIndex */
typedef struct__4927 LutIndex_lut;

#ifndef kcg_copy_struct__4927
#define kcg_copy_struct__4927(kcg_C1, kcg_C2) (kcg_assign_struct((kcg_C1), (kcg_C2), sizeof (struct__4927)))
#endif /* kcg_copy_struct__4927 */

#ifndef kcg_comp_struct__4927
extern kcg_bool kcg_comp_struct__4927(
  struct__4927 *kcg_c1,
  struct__4927 *kcg_c2);
#endif /* kcg_comp_struct__4927 */

#define kcg_comp_LutIndex_lut kcg_comp_struct__4927

#define kcg_copy_LutIndex_lut kcg_copy_struct__4927

#endif /* _KCG_TYPES_H_ */
/* $*************** KCG Version 6.1.2 (build i5) ****************
** kcg_types.h
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */

