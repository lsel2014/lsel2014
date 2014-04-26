#ifndef RAILWAY_H
#define RAILWAY_H

#include "train.h" 
#include "Model/observer.h"
#include "Interpreter/interp.h"

#define NSECTORS 4
#define MAXRAILWAY 4

typedef struct {
  int id;
  traint_t* registeredTrains [MAXTRAINS];
  } sector_t; 

// ID is add so it can be more than one railway 
typedef struct {
  int id;
  observable_t* observable;
  sector_t* railwaySectors [NSECTORS];
  } railway _t; 
  
  
railway_t* train_new( int id );
void railway_init( railway_t* this, int id );
sector_t* sector_new( int id);
void railway_init( sector_t* this, int id );
int railway_cmd(char* arg);

void railway_register_train( railway_t* this, train_t* train , int sector);
void railway_erase_train( railway_t* this, train_t* train , int sector);

#endif
