//PLEAESE CHANGE THIS FILE'S NAME AND INTEGRATE WITH EVERYTHING ELSE//

/**
 * Arrival time estimator based on how DCC trains work.
 *
 *
 * DCC controllers let you set three points for the speed mapping curve.
 * Vini is the voltage applied to the motor when the first speed code is sent
 * Vmid is the voltage applied to the motor when the central speed code is sent
 * Vmax is the voltage applied to the motor at max speed
 *
 * Vini is set to compensate for the minimum voltage required for the motor to start.
 * That way all speed codes cause a speed variation and there are no "dead codes" that keep
 * the train static.
 *
 * Vmid is used to create a more reallistic speed profile, splitting the curve in two linear
 * sections
 *
 * Acceleration and deceleration profile is set by CV3 and 4. When you set a new train speed,
 * the controller increments or decrements the "real" speed at a rate defined by these values.
 * The speed is incremented in speed code equivalents. For example if your current train speed
 * is 10 and you set the new speed to 20 the controller will increment the speed code from 10
 * to 20 at a certain rate. The time for incrementing one speed code is calculated as:
 *
 * T= 0.896*CV3or4value/MaxSpeedStep
 *
 * In our case MaxSpeedStep=28 as we are using the 28 step speed mode.
 *
 * THE FUNCTION IS IMPLEMENTED AS IF IT WERE NOT INTEGRATED WITH THE TRAIN MODEL
 * REQUIRED PARAMETERS SHOULD BE TAKEN FROM A TRAIN STRUCTURE AND RESULTS STORED
 * THERE TOO. THIS IS JUST AN ALGORITHM TESTING FUNCTION.
 *
 */

#include "train.h"
#include "dcc.h"
#include "tasks.h"
/*To be filled with empirical values for EACH TRAIN*/
/* THIS SHOULD BE DONE AUTOMATICALLY*/

/**
 * Select a train
 * Start it at the min speed code
 * Wait for it to complete a full lap
 * Start monitoring sector times
 * After N laps take the average and compute std dev
 * repeat for each speed code
 * Fill the table.
 */

int main() {
	/*TAKE INTO ACCOUNT INTERNAL AND EXTERNAL TRACKS*/
	float sector_lengths[4] = { 1.1, 2.2, 3.3, 4.4 };
	int sector_pins[4] = { 1, 2, 3, 4 };
	dcc_sender_t* dccobject = dcc_new(12, 50);
	train_t* trains[2];
	trains[0] = train_new("Diesel", 0b0000100, '0', 20, dccobject);
	trains[1] = train_new("Renfe", 0b0000011, '0', 25, dccobject);
	float tables[28][2];
	task_start_all();
	int i, j, k;
	for (i = 0; i < 4; i++) {
		pinMode(sector_pins[i], INPUT);
	}
	for (i = 0; i < 2; i++) {
		for (j = 1; j < 29; j++) {
			dcc_add_speed_packet(trains[i]->dcc, trains[i]->ID, j);
			while (!digitalWrite(sector_pins[4]))
				;
			while (!digitalWrite(sector_pins[4]))
				;
			for (k = 0; k < 4; k++) {
			while(!digitalWrite())
		}
	}

}

}
float speed_table[28];

float get_initial_coarse_estimation(int current_speed, int target_speed,
	float sector_length, int cv3, int vmin, int vmid, int vmax) {

/*ASSUME MOTOR SPEED IS LINEAR WITH APPLIED VOLTAGE*/
/*IMPLEMENT FOR DECELERATIONS TOO*/
/*NO UNITS FOR NOW, USE MICRO/NANOSECONDS AND AVOID FLOATS IF POSSIBLE*/
/*Integration using trapezoidal rule*/
float acceleration_time_step = 0.896 * (float) cv3 / 28.0;
float acceleration_time = acceleration_time_step
		* (target_speed - current_speed);
float distance_accelerating = 0;
int i;
/*We calculate first the distance that the train will be accelerating*/
for (i = current_speed; i < target_speed; i++) {
	distance_accelerating += (speed_table[i + 1] + speed_table[i])
			* acceleration_time_step / 2;
}

/*ASSUME (for now) THAT IT TAKES LESS THAN ONE SECTOR TO ACCELERATE*/
float remaining_distance = sector_length - distance_accelerating;
/*Assume that remaining distance till next sector will be covered at constant speed*/
if (speed_table[target_speed] <= 0)
	return -1;
return acceleration_time + remaining_distance / speed_table[target_speed];
}

