/* $*************** KCG Version 6.1.2 (build i5) ****************
** Command: s2c612 -config C:/Users/SKY/Desktop/ACC/KCG\kcg_s2c_config.txt
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */
#include "stage.hh"

using namespace Stg;
#include "stdlib.h"
#include "kcg_consts.h"
#include "kcg_sensors.h"
#include "ACC.h"
FILE *out;
	
// The structure that represents the robot
typedef struct
{
	ModelPosition* pos;
	ModelRanger* ranger;
	ModelLinefollower* linefollower;
	inC_ACC input;
	outC_ACC output;

} robot_t;

// The callback functions needed for handling the changes in the sensor values
int LinefollowerUpdate( ModelLinefollower* mod, robot_t* robot );
int PositionUpdate( Model* mod, robot_t* robot );
int RangerUpdate( ModelRanger* mod, robot_t* robot);

void ACC_reset(outC_ACC *outC)
{
  outC->init = kcg_true;
  outC->init1 = kcg_true;
}


/** "Title_1" {Title = "Title : Adaptive Cruise Control"} */
/** "Author_1" {Author = "Created by : Hermesh Gupta"} */
/** "Date_1" {Date = "05/09/2011"} */
/* ACC */
void ACC(inC_ACC *inC, outC_ACC *outC)
{
  /* ACC::SMACC::Move::SMmove */ SSM_ST_SMmove_SMACC_Move SMmove_state_act_SMACC_Move;
  /* ACC::SMACC::Move::SMmove */ SSM_ST_SMmove_SMACC_Move SMmove_state_sel_SMACC_Move;
  /* ACC::SMACC */ SSM_ST_SMACC SMACC_state_sel;
  /* ACC::SMACC */ SSM_ST_SMACC SMACC_state_act;
  /* ACC::SMACC */ kcg_bool SMACC_reset_act;
	int temp=0; 
	if ((inC->FRONT_RIGHT_RANGER < MIN_SAFE_DISTANCE) & 
	    (inC->FRONT_RIGHT2_RANGER < MIN_SAFE_DISTANCE) &
		(inC->FRONT_LEFT2_RANGER < MIN_SAFE_DISTANCE) &
		(inC->LEFT_CORNER_RANGER < MIN_SAFE_DISTANCE) & 
		(inC->RIGHT_CORNER_RANGER < MIN_SAFE_DISTANCE)) {
	    SMmove_state_act_SMACC_Move = SSM_st_BackMove_SMACC_Move_SMmove;
           }
	  
	else if (inC->LEFT_CORNER_RANGER < MIN_SAFE_DISTANCE) {
		 temp = rand()%2;		
		if (temp == 0)
            	   SMmove_state_act_SMACC_Move = SSM_st_RightMove_SMACC_Move_SMmove;			
 		else 
		   SMmove_state_act_SMACC_Move = SSM_st_LeftMove_SMACC_Move_SMmove;
            //SMmove_state_act_SMACC_Move = SSM_st_RightMove_SMACC_Move_SMmove;
          }
          else if (inC->RIGHT_CORNER_RANGER < MIN_SAFE_DISTANCE) {
		 temp = rand()%2;		
		/*(if (temp == 0)
            	   SMmove_state_act_SMACC_Move = SSM_st_RightMove_SMACC_Move_SMmove;			
 		else 
		   SMmove_state_act_SMACC_Move = SSM_st_LeftMove_SMACC_Move_SMmove;*/
            SMmove_state_act_SMACC_Move = SSM_st_LeftMove_SMACC_Move_SMmove;
          }
	else  if ((inC->FRONT_RIGHT_RANGER < MIN_SAFE_DISTANCE) | 
		  (inC->FRONT_LEFT_RANGER < MIN_SAFE_DISTANCE)) {
		    temp = rand()%2;		
		if (temp == 0)
            	   SMmove_state_act_SMACC_Move = SSM_st_RightMove_SMACC_Move_SMmove;			
 		else 
		   SMmove_state_act_SMACC_Move = SSM_st_LeftMove_SMACC_Move_SMmove;			
          }
	   else 
		SMmove_state_act_SMACC_Move = SSM_st_Forward_SMACC_Move_SMmove;


 
      switch (SMmove_state_act_SMACC_Move) {
        case SSM_st_Forward_SMACC_Move_SMmove :
          outC->BUZZER_ON = kcg_false;
          outC->FORWARD_SPEED = 5.0;
          outC->TURN_SPEED = 0.0;
          break;
        case SSM_st_LeftMove_SMACC_Move_SMmove :
          outC->BUZZER_ON = kcg_false;
          outC->FORWARD_SPEED = 0.0;
          outC->TURN_SPEED = 90.0;
          break;
        case SSM_st_RightMove_SMACC_Move_SMmove :
          outC->BUZZER_ON = kcg_false;
          outC->FORWARD_SPEED = 0.0;
          outC->TURN_SPEED = - 90.0;
          break;
        case SSM_st_BackMove_SMACC_Move_SMmove :
          outC->BUZZER_ON = kcg_false;
          outC->FORWARD_SPEED = -1.0;
          outC->TURN_SPEED = -30.0;
          break;

        
      }

      switch (SMmove_state_act_SMACC_Move) {
        case SSM_st_Forward_SMACC_Move_SMmove :
          outC->SMmove_state_nxt_SMACC_Move = SSM_st_Forward_SMACC_Move_SMmove;
          break;
        case SSM_st_LeftMove_SMACC_Move_SMmove :
          outC->SMmove_state_nxt_SMACC_Move = SSM_st_LeftMove_SMACC_Move_SMmove;
          break;
        case SSM_st_RightMove_SMACC_Move_SMmove :
          outC->SMmove_state_nxt_SMACC_Move =
            SSM_st_RightMove_SMACC_Move_SMmove;
          break;
        case SSM_st_BackMove_SMACC_Move_SMmove :
          outC->SMmove_state_nxt_SMACC_Move =
            SSM_st_BackMove_SMACC_Move_SMmove;
          break;

        
      }
   
}


// Stage calls this when the model starts up
extern "C" int Init( Model* mod, CtrlArgs* args )
{
	// Declare a robot instance
	robot_t* robot = new robot_t;
	
	ACC_reset(&robot->output);
	
	// Bind the robot's sensors local instance to the internal representation
	robot->pos = (ModelPosition*)mod;
	robot->linefollower = (ModelLinefollower*)mod->GetUnusedModelOfType( "linefollower" );
	robot->ranger=(ModelRanger*)mod->GetChild("ranger:0");
	// Bind the local function such that it is called in every loop of stage
	robot->linefollower->AddUpdateCallback( (stg_model_callback_t)LinefollowerUpdate, robot );
	robot->pos->AddUpdateCallback( (stg_model_callback_t)PositionUpdate, robot );
	robot->ranger->AddUpdateCallback( (stg_model_callback_t)RangerUpdate,robot);
	robot->linefollower->Subscribe(); // starts the linefollower updates
	robot->pos->Subscribe(); // starts the position updates
	robot->ranger->Subscribe();
	return 0; //ok
}


// inspect the linefollower data and decide what to do
int LinefollowerUpdate( ModelLinefollower* rgr, robot_t* robot )
{
	// access the sensors
	//printf("LEFT	%f\n",rgr->sensors[0].value);
	//printf("MIDDLE	%f\n",rgr->sensors[1].value);
	//printf("RIGHT	%f\n",rgr->sensors[2].value);
	robot->input.LEFT_WHITELINE_VALUE = rgr->sensors[0].value;
	robot->input.MIDDLE_WHITELINE_VALUE = rgr->sensors[1].value;
	robot->input.RIGHT_WHITELINE_VALUE = rgr->sensors[2].value;
	
	ACC(&robot->input,&robot->output);
	
	// Set the speed of the motor here
	robot->pos->SetSpeed( robot->output.FORWARD_SPEED, 0, dtor(robot->output.TURN_SPEED) );
	
return 0;
}

int RangerUpdate(ModelRanger* mod, robot_t* robot)
{ 
	//printf("front left ranger	%f\n",mod->sensors[0].range);
	//printf("front right ranger	%f\n",mod->sensors[1].range);
	//printf("left corner ranger	%f\n",mod->sensors[2].range);
	//printf("right corner ranger	%f\n",mod->sensors[3].range);

	robot->input.FRONT_LEFT_RANGER = mod->sensors[0].range;
	robot->input.FRONT_RIGHT_RANGER = mod->sensors[1].range;
	robot->input.LEFT_CORNER_RANGER = mod->sensors[2].range;
	robot->input.RIGHT_CORNER_RANGER = mod->sensors[3].range;

	ACC(&robot->input,&robot->output);
	
	// Set the speed of the motor here
	robot->pos->SetSpeed( robot->output.FORWARD_SPEED, 0, dtor(robot->output.TURN_SPEED) );
	
return 0;
}

int PositionUpdate( Model* mod, robot_t* robot )
{

	out = fopen("sample.txt","a");	
	robot->pos->SetSpeed(robot->output.FORWARD_SPEED, 0, dtor(robot->output.TURN_SPEED) );
	Pose pose = robot->pos->GetPose();
	//printf( "Pose: [%.2f %.2f %.2f %.2f]\n",pose.x, pose.y, pose.z, pose.a );
	 if (out !=NULL)
		fprintf(out,"%.2f %.2f\n",pose.x, pose.y);
 	fclose(out);
	return 0; // run again
}


/* $*************** KCG Version 6.1.2 (build i5) ****************
** ACC.c
** Generation date: 2011-09-05T16:54:45
*************************************************************$ */

