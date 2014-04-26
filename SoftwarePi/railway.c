#include "railway.h"

void 
railways_setup(void)
{
  railway_new (0);
  interp_addcmd("railway", railway_cmd, "Lists railway sectors");
}
railway_t* 
railway_new( int id )
{
  railway_t* this = (railway_t*) malloc(sizeof(railway_t));
  railway_init(this, id );
  if (nrailways < MAXRAILWAY ) {
		railways[nrailways++] = this;
	}
  return this;
}

void 
railway_init( railway_t* this, int id )
{
  int i;
  this->id = id;
  observable_init(&this->observable);
  for( i= 0; i< NSECTOR; i++){
    this->railwaySectors[i] = sector_new(i);
    }
}
sector_t* 
sector_new( int id)
{
  sector_t* this = (sector_t*) malloc(sizeof(sector_t));
  sector_init(this, id );	
}

void 
sector_init( sector_t* this, int id )
{
  this->id = id;
}
int 
railway_cmd(char* arg)
{
  struct sector_t* s;
	for(s = ;i<nsensors;i++){
		printf("Sensor %d",sensors[i]->id );
	}
	return 0;
  return NULL;
}

void railway_register_train( railway_t* this, train_t* train , int sector)
{
  return NULL;
}
void railway_erase_train( railway_t* this, train_t* train , int sector)
{
  return NULL;
}
