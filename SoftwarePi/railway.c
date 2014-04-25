#include "railway.h"

railway_t* 
train_new( int id )
{
  railway_t* this = (railway_t*) malloc(sizeof(railway_t));
  railway_init(this, id );
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
  return NULL;
}
void 
railway_init( sector_t* this, int id )
{
  return NULL;
}
int 
railway_cmd(char* arg)
{
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
