#include "Model/observer.h"
#include "Model/Sensors/sensorIR.h"
#include "model.h"
#include "tracker.h"
#include "train.h"


static observer_t tracker_observer;
// Arrays donde se guardan los elementos de la maqueta
// que interesan al tracker. 
static struct ir_sensor_data_t {
		sensorIR_t* sensor;
		char sector;
} ir_sensors [MAXIRSENSORS];
 
static struct train_data_t {
		train_t* train;
		int IRsimbolicId;
} tracker_trains [MAXTRAINS];

static int n_ir_sensors;
static int n_trains;

static event_t* event;
// Esta funcion solo traduce la id simbolica al 
// puntero de tren, la he sacado para limpiar un poco
train_t*
tracker_gen_train(int id){
     struct train_data_t* t;             
     for (t = tracker_trains; t->train; ++t) {
         if ( id == t-> IRsimbolicId )
         return t->train;
     }
     return NULL;
}  
                    
// El notify revisa los sensores que estan registrados 
// y si ve que alguno tiene algun evento, lo lee y 
// actualiza la maqueta 
static
void tracker_notify (observer_t* this, observable_t* foo)
{

	struct ir_sensor_data_t* p;
	for (p = ir_sensors; p->sensor; ++p) {
	    event = sensorIR_get_event(p->sensor);
	    printf( " he cogido a %d en %c", event->passingTrain,p->sector);
	    if (event->flag == 1 ) {
	    	train_set_current_sector (tracker_gen_train(event->passingTrain), p->sector);
	    	//railway_register_train(tracker_gen_train(event->passingTrain), p->sector);
      	}
    }
}

void
tracker_init (void)
{
    // Estructura donde metemos los nombres que los sensores van a tener
    // en el modelo y sus sectores asociados. 
	static struct ir_name_t {
			const char* name;
			char sector;
	} ir_names [] = {
			{ "IRsensor[0]", 0 },
			{ "IRsensor[1]", 1 },
			{ "IRsensor[2]", 2 },
			{ "IRsensor[3]", 3 },
			{ NULL, 0 }
	};
	
	struct ir_name_t* s;
	// Estructura donde metemos los nombres que los trenes van a tener
    // en el modelo y las id que tienen en los sensores IR. 
	static struct train_name_t {
			const char* name;
			int IRsimbolicId;
	} train_names [] = {
			{ "Diesel", 4 },
			{ "Renfe", 3 },
			{ NULL, 0 }
	};
	
	struct train_name_t* t;
	// Inicializamos el observer del tracker
	observer_init (&tracker_observer, tracker_notify);
	
	n_ir_sensors = 0;
	n_trains=0;
	// Estos bucles se dedican a coger los sensores y los trenes
	// del modelo y registrarlos en el tracker 
	for (s = ir_names; s->name; ++s) {
		observable_t* obs = model_get_IRsensor (s->name);
		observable_register_observer (obs, &tracker_observer);
		ir_sensors[n_ir_sensors].sensor = (sensorIR_t*) obs;
		ir_sensors[n_ir_sensors].sector = s->sector;
		++n_ir_sensors;
	}
	
	for (t = train_names; t->name; ++t) {
		observable_t* obs = model_get_train (t->name);
		observable_register_observer (obs, &tracker_observer);
		tracker_trains[n_ir_sensors].train = (train_t*) obs;
		tracker_trains[n_ir_sensors].IRsimbolicId = t->IRsimbolicId;
		++n_trains;
	}
	// Comentario: cuando este la via, hay que cogerla tambien.
}


  

