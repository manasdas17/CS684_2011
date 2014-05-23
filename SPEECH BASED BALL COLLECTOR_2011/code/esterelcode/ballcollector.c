#include "firebird_winavr.h"
#ifndef STRLEN
#  define STRLEN 81
#endif
#define _true 1
#define _false 0
typedef unsigned char boolean;
typedef int integer;
typedef char* string;
void xbee_I_RECEIVE(integer);
void xbee_I_FRONT_IR_VALUE(integer);
int xbee(void);
int xbee_reset(void);
#ifndef _NO_EXTERN_DEFINITIONS
#  ifndef _NO_CONSTANT_DEFINITIONS
#  endif /* _NO_CONSTANT_DEFINITIONS */
#  ifndef _NO_FUNCTION_DEFINITIONS
#  endif /* _NO_FUNCTION_DEFINITIONS */
#  ifndef _NO_PROCEDURE_DEFINITIONS
#  endif /* _NO_PROCEDURE_DEFINITIONS */
#endif /* _NO_EXTERN_DEFINITIONS */

static struct {
  unsigned int RECEIVE : 1;
  unsigned int FRONT_IR_VALUE : 1;
  unsigned int SEND : 1;
  unsigned int MOVE_FWD : 1;
  unsigned int MOTOR_LEFT_SPEED : 1;
  unsigned int MOTOR_RIGHT_SPEED : 1;
  unsigned int MOVE_INPLACE_RIGHT_SPEED : 1;
  unsigned int PICK : 1;
  unsigned int DROP : 1;
  unsigned int X : 1;
  unsigned int X_1 : 1;
  unsigned int X_2 : 1;
  unsigned int X_3 : 1;
  unsigned int X_4 : 1;
  unsigned int X_5 : 1;
  unsigned int X_6 : 1;
  unsigned int X_7 : 1;
  unsigned int X_8 : 1;
  unsigned int X_9 : 1;
  unsigned int X_10 : 1;
  unsigned int X_11 : 1;
  unsigned int X_12 : 1;
  unsigned int X_13 : 1;
  unsigned int X_14 : 1;
  unsigned int X_15 : 1;
} _s = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
#define tick tick_timer()
static integer RECEIVE_v;
static integer FRONT_IR_VALUE_v;
static integer SEND_v;
static integer MOTOR_LEFT_SPEED_v;
static integer MOTOR_RIGHT_SPEED_v;
static integer MOVE_INPLACE_RIGHT_SPEED_v;
static integer fir;
static integer recvdata;
static integer fir_1;
static integer rdata;
static unsigned char _state_1 = 5;
void xbee_I_RECEIVE(integer _v) {
  _s.RECEIVE = 1;
  RECEIVE_v = _v;
}
void xbee_I_FRONT_IR_VALUE(integer _v) {
  _s.FRONT_IR_VALUE = 1;
  FRONT_IR_VALUE_v = _v;
}

int xbee(void)
{
  /* Vacuous terminate */;
  switch (_state_1) {
  case 0:
    if (_s.FRONT_IR_VALUE) {
      fir = FRONT_IR_VALUE_v;
      if ((fir < 120)) {
        _s.DROP = 1;
        (SEND_v = 54), (_s.SEND = 1);
        _state_1 = 4;
      } else {
        _state_1 = 0;
      }
    } else {
      _state_1 = 0;
    }
    break;
  case 1:
    if (_s.RECEIVE) {
      recvdata = RECEIVE_v;
      if ((recvdata == 52)) {
        (MOVE_INPLACE_RIGHT_SPEED_v = 200), (_s.MOVE_INPLACE_RIGHT_SPEED = 1);
        goto N29;
      } else {
        if ((recvdata == 53)) {
          (SEND_v = 53), (_s.SEND = 1);
          _s.MOVE_FWD = 1;
          (MOTOR_LEFT_SPEED_v = 150), (_s.MOTOR_LEFT_SPEED = 1);
          (MOTOR_RIGHT_SPEED_v = 150), (_s.MOTOR_RIGHT_SPEED = 1);
          _state_1 = 0;
        } else {
        N29:
          _state_1 = 1;
        }
      }
    } else {
      _state_1 = 1;
    }
    break;
  case 2:
    if (_s.FRONT_IR_VALUE) {
      fir_1 = FRONT_IR_VALUE_v;
      if ((fir_1 < 120)) {
        _s.PICK = 1;
        (SEND_v = 51), (_s.SEND = 1);
        _state_1 = 1;
      } else {
        _state_1 = 2;
      }
    } else {
      _state_1 = 2;
    }
    break;
  case 3:
    if (_s.RECEIVE) {
      rdata = RECEIVE_v;
      if ((rdata == 49)) {
        (MOVE_INPLACE_RIGHT_SPEED_v = 150), (_s.MOVE_INPLACE_RIGHT_SPEED = 1);
        goto N41;
      } else {
        if ((rdata == 50)) {
          (SEND_v = 50), (_s.SEND = 1);
          _s.MOVE_FWD = 1;
          (MOTOR_LEFT_SPEED_v = 150), (_s.MOTOR_LEFT_SPEED = 1);
          (MOTOR_RIGHT_SPEED_v = 150), (_s.MOTOR_RIGHT_SPEED = 1);
          _state_1 = 2;
        } else {
        N41:
          _state_1 = 3;
        }
      }
    } else {
      _state_1 = 3;
    }
    break;
  case 4:
    if (_s.RECEIVE) {
      _state_1 = 3;
    } else {
      _state_1 = 4;
    }
    break;
  case 5:
    _s.DROP = 0;;
    _s.PICK = 0;;
    _s.MOVE_INPLACE_RIGHT_SPEED = 0;;
    _s.MOTOR_RIGHT_SPEED = 0;;
    _s.MOTOR_LEFT_SPEED = 0;;
    _s.MOVE_FWD = 0;;
    _s.SEND = 0;;
    _state_1 = 4;
    break;
  default: break;
  }
  if (_s.SEND) { xbee_O_SEND(SEND_v); _s.SEND = 0; }
  if (_s.MOVE_FWD) { xbee_O_MOVE_FWD(); _s.MOVE_FWD = 0; }
  if (_s.MOTOR_LEFT_SPEED) { xbee_O_MOTOR_LEFT_SPEED(MOTOR_LEFT_SPEED_v); _s.MOTOR_LEFT_SPEED = 0; }
  if (_s.MOTOR_RIGHT_SPEED) { xbee_O_MOTOR_RIGHT_SPEED(MOTOR_RIGHT_SPEED_v); _s.MOTOR_RIGHT_SPEED = 0; }
  if (_s.MOVE_INPLACE_RIGHT_SPEED) { xbee_O_MOVE_INPLACE_RIGHT_SPEED(MOVE_INPLACE_RIGHT_SPEED_v); _s.MOVE_INPLACE_RIGHT_SPEED = 0; }
  if (_s.PICK) { xbee_O_PICK(); _s.PICK = 0; }
  if (_s.DROP) { xbee_O_DROP(); _s.DROP = 0; }
  _s.RECEIVE = 0;
  _s.FRONT_IR_VALUE = 0;
  return 1;
}

int xbee_reset(void)
{
  _s.RECEIVE = 0;
  _s.FRONT_IR_VALUE = 0;
  return 0;
}
/****************************** Fire Bird Specific part ***************************/
static int  IR_THRESHHOLD[3] = {50, 50, 50};
xbee_O_MOTOR_RIGHT_SPEED(int val)
{
	MOTOR_RIGHT_SPEED(val);
}
xbee_O_SEND(int val)
{
	SEND(val);
}
xbee_O_MOVE_INPLACE_RIGHT_SPEED(int val)
{
	rotate_inplace_right_speed(val);
}
xbee_O_DROP(void)
{
	DROP();
}
xbee_O_MOTOR_LEFT_SPEED(int val)
{
	MOTOR_LEFT_SPEED(val);
}
xbee_O_PICK(void)
{
	PICK();
}
xbee_O_MOVE_FWD(void)
{
	MOVE_FWD();
}

/****************************** Main function ***************************/
void main()
{
 init_devices();
 xbee_reset();
 xbee();
 while(1)
 {
    if(DATA_RECEIVED == 1)
    {
	   int val;
	   val=RECEIVE();
      xbee_I_RECEIVE(val);
    }
    xbee_I_FRONT_IR_VALUE(FRONT_IR);
    xbee();
 }
}
