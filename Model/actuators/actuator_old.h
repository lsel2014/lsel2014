#ifndef ACTUATOR_H
#define ACTUATOR_H

typedef struct actuator_t
{	
	int id;

} actuator_t;


/*
 * Rail change
 */
void actuator_rail_change_init (actuator_t* this, int id);
actuator_t* actuator_rail_change_new (int id);


/*
 * Traffic light
 */
void actuator_traffic_light_init (actuator_t* this, int id);
actuator_t* actuator_traffic_light_new (int id);


/*
 * Barrier
 */
void actuator_barrier_init (actuator_t* this, int id);
actuator_t* actuator_barrier_new (int id);


/*
 * Sun
 */
void actuator_sun_init (actuator_t* this, int id);
actuator_t* actuator_sun_new (int id);

/*
 * Frames
 */
void actuator_frames_init (actuator_t* this, int id);
actuator_t* actuator_frames_new (int id);

#endif
