#ifndef RAILWAY_H
#define RAILWAY_H

#include <native/mutex.h>
#include "train.h" 
#include "Model/observer.h"
#include "Interpreter/interp.h"

#define NSECTORS 4
#define MAXRAILWAY 4

typedef struct {
  int id;
  train_t* registeredTrains [MAXTRAINS];
  int nregisteredtrains;
  } sector_t; 

// ID is add so it can be more than one railway 
typedef struct {
  observable_t observable;
  int id;
  RT_MUTEX mutex;
  sector_t* railwaySectors [NSECTORS];
  } railway_t;
  
extern railway_t* railways[MAXRAILWAY];
extern int nrailways;

void railways_setup(void);
railway_t* railway_new( int id );
void railway_init( railway_t* this, int id );
sector_t* sector_new( int id);
void sector_init( sector_t* this, int id );
int railway_cmd(char* arg);

void railway_register_train( railway_t* this, train_t* train , int sector);
void railway_erase_train( railway_t* this, train_t* train );

#endif
