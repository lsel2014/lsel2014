#include "railway.h"
#include "train.h"

railway_t* railways[MAXRAILWAY];
int nrailways;

void railways_setup(void) {
	railway_new(0);
	interp_addcmd("railway", railway_cmd, "Lists railway sectors");
}
railway_t*
railway_new(int id) {
	railway_t* this = (railway_t*) malloc(sizeof(railway_t));
	railway_init(this, id);
	if (nrailways < MAXRAILWAY) {
		railways[nrailways++] = this;
	}
	return this;
}

void railway_init(railway_t* this, int id) {
	int i;
	this->id = id;
	observable_init(&this->observable);
	for (i = 0; i < NSECTORS; i++) {
		this->railwaySectors[i] = sector_new(i);
	}
	rt_mutex_create(&this->mutex, NULL);
}
sector_t*
sector_new(int id) {
	sector_t* this = (sector_t*) malloc(sizeof(sector_t));
	sector_init(this, id);
	return this;
}

void sector_init(sector_t* this, int id) {
	int i;
	this->id = id;
	for ( i = 0; i < MAXTRAINS ; i++)
	this-> registeredTrains [i] = NULL;
	this-> nregisteredtrains = 0;
}

int railway_cmd(char* arg) {
	int i, j ,k;
	for (i = 0; i < nrailways; i++) {
		for (j = 0; j < NSECTORS; j++) {
			printf("via %d : Sector %d \n", railways[i]->id, railways[i]->railwaySectors[j]->id);
			for (k = 0; k < railways[i]->railwaySectors[j]-> nregisteredtrains ; k++)
				printf("train %d \n", train_get_ID(railways[i]->railwaySectors[j]->registeredTrains[k]));
		}
	}
	return 0;
}

void railway_register_train(railway_t* this, train_t* train, int sector) {
	int rs;
	rs = this->railwaySectors[sector]->nregisteredtrains;
	if (rs < MAXTRAINS) {
		rt_mutex_acquire(&this->mutex, TM_INFINITE);
		this->railwaySectors[sector]->registeredTrains[rs] = train;
		this->railwaySectors[sector]->nregisteredtrains++;
		observable_notify_observers(&this->observable);
		rt_mutex_release(&this->mutex);
	}
}
void railway_erase_train ( railway_t* this, train_t* train ) {
	int i, j ,k;
	for (i = 0; i < NSECTORS; i++) {
	for (j = 0; j < this -> railwaySectors[i] -> nregisteredtrains ; j++) {
		if ( train_get_ID(train) == train_get_ID(this -> railwaySectors[i]-> registeredTrains[j])){
		rt_mutex_acquire(&this->mutex, TM_INFINITE);
		this -> railwaySectors[i]-> registeredTrains[j] = NULL;
		for (k = j ; k < MAXTRAINS - 1 ; k++)
			this -> railwaySectors[i]-> registeredTrains[k] =
					this -> railwaySectors[i]-> registeredTrains[k+1];
		this->railwaySectors[i]->nregisteredtrains--;
		rt_mutex_release(&this->mutex);
		}
		}
	}
}
