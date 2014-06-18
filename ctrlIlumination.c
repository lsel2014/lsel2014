#include <rtdk.h>
#include "ctrlIlumination.h"
#include "railway.h"
#include "crossingGate.h"
#include "semaphore.h"
#include "model.h"
#include "trafficLight.h"
#include "semaphore.h"

static observer_t ctrlilu_observer;

static struct railway_data_t {
	railway_t* railway;
	int platform;
} ctrlilu_railway[MAXRAILWAY];

static struct crossingGate_data_t {
	crossingGate_t* crossingGate;
} ctrlilu_crossingGate[MAXCROSINGGATES];

static struct semaphore_data_t {
	semaphore_t* semaphore;
	int sector;
} ctrlilu_semaphore[MAXSEMAFORES];

static struct trafficLight_data_t {
	trafficLight_t* trafficLight;
	int id;
} ctrlilu_trafficLight[MAXTRAFFICLIGHTS];

static int n_railway;
static int n_crossingGate;
static int n_semaphore;
static int n_traficLight;

void 
ctrlilumination_notify(observer_t* this)
{
     int i,j,k,l;
     railway_t* rail;
     crossingGate_t* cross;
     for( i = 0 ; i < n_railway ; i++){
     rail= ctrlilu_railway[i].railway;
    // check = 0;
       for( j = 0; j < NSECTORS ; j++){
          if( rail->railwaySectors[j]->nregisteredtrains > 0) {
                 //there is a train with override activated
	         for( l = 0 ; l < rail->railwaySectors[j]->nregisteredtrains ; l++){
               	    if (train_get_security(rail->railwaySectors[j]->registeredTrains[l]) == 1) {
              			semaphore_set_state(semaphores[j], I2C_SEMAPHORE_RED);
               	    } else {
               	    //there is a train, but override is deactivated
               			semaphore_set_state(semaphores[j], I2C_SEMAPHORE_YELLOW);
                    }
                 }
                 for( k = 0; k < n_crossingGate ; k++){
                    cross = ctrlilu_crossingGate[k].crossingGate;
                    if( j ==  cross->sensiblesector){
                       crossingGate_set_position(cross, DOWN);
                      //trafficLight_set_state(trafficLights[0], ON);
		    }
		    if( j == cross->sensiblesector - 1){
		       if(train_get_direction(rail->railwaySectors[j]->registeredTrains[0]) == FORWARD) {
 		         crossingGate_set_position(cross, UP);
		         crossingGate_set_light(cross, 0);
		       }else{
		         crossingGate_set_light(cross, 1);
		       }
		    }
		    if( j == cross->sensiblesector + 1){
		       if(train_get_direction(rail->railwaySectors[j]->registeredTrains[0]) == REVERSE) {
		         crossingGate_set_position(cross, UP);
		         crossingGate_set_light(cross, 0);
                       }else{
			 crossingGate_set_light(cross, 1);
		       }
		    }
               }
          }else{//no trains
               semaphore_set_state(semaphores[j], I2C_SEMAPHORE_GREEN);
	  }
               /*for( k = 0; k < n_crossingGate ; k++){
                   cross = ctrlilu_crossingGate[k].crossingGate;
                   if( j ==  cross->sensiblesectors[0] || j ==  cross->sensiblesectors[1])
                    check++;
                   }
               }
     }            
              
        if(check == 2){
        crossingGate_set_position(cross, UP); 
        trafficLight_set_state(trafficLights[0], OFF);
        }*/
     }  
  }
}

void 
ctrlilumination_init(void)
{
    static struct railway_name_t {
		const char* name;
		int platform;
	} railway_names[] = { { "via0", 0 }, { NULL, 0 } };
	
    static struct crossingGate_name_t {
		const char* name;
	} crossingGate_names[] = { { "barrera0" }, { NULL } };
	
	static struct semaphore_name_t {
		const char* name;
		int sector;
	} semaphore_names[] = { { "semaforo0",0 }, { "semaforo1",1 }, { "semaforo2",2 },
                             { "semaforo3",3 }, { NULL } };
                             
	static struct trafficlight_name_t {
		const char* name;
		int id;
	} trafficlight_names[] = { { "semaforotrafico0" , 0}, { NULL, 0} };
	

	struct railway_name_t* r;
	struct crossingGate_name_t* cr;
	struct semaphore_name_t* sph;
	struct trafficlight_name_t* tl;	
	
	observer_init(&ctrlilu_observer, ctrlilumination_notify);
	n_railway=0;
    n_crossingGate=0;
    n_semaphore=0;
    
	for (r = railway_names; r->name; ++r) {
		observable_t* obs = model_get_observable(r->name);
		observable_register_observer(obs, &ctrlilu_observer);
		ctrlilu_railway[n_railway].railway = (railway_t*) obs;
		ctrlilu_railway[n_railway].platform = r->platform;
		++n_railway;
	}
	for (cr = crossingGate_names; cr->name; ++cr) {
		observable_t* obs = model_get_observable(cr->name);
		observable_register_observer(obs, &ctrlilu_observer);
		ctrlilu_crossingGate[n_crossingGate].crossingGate = (crossingGate_t*) obs;
		++n_crossingGate;
	}

	for (sph = semaphore_names; sph->name; ++sph) {
		observable_t* obs = model_get_observable(sph->name);
		observable_register_observer(obs, &ctrlilu_observer);
		ctrlilu_semaphore[n_semaphore].semaphore = (semaphore_t*) obs;
		ctrlilu_semaphore[n_semaphore].sector = sph->sector;
		++n_semaphore;
	}
	for (tl = trafficlight_names; tl->name; ++tl) {
		observable_t* obs = model_get_observable(tl->name);
		observable_register_observer(obs, &ctrlilu_observer);
		ctrlilu_trafficLight[n_traficLight].trafficLight = (trafficLight_t*) obs;
		ctrlilu_trafficLight[n_traficLight].id = tl->id;
		++n_traficLight;
	}

}

