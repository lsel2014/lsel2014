#ifndef TRACKER_H
#define TRACKER_H

#define MAXSENSORSTRACKER 20

void tracker_init(void);
void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);
train_t* tracker_gen_train(int id);
void tracker_updating_train(train_t* train, int sector, telemetry_t* tel);
void tracker_register_train(train_t* train, int sector);
static void tracker_notify(observer_t* this, observable_t* foo);
void tracker_init(void);

#endif
