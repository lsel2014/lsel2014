#include "ir_identification.h"

ir_identification_t* ir_identification_new (int id, int num_trains)
{
    ir_identification_t* this = (ir_identification_t*) malloc ((sizeof) ir_identification_t);
    ir_identification_init (this, id);

    return this;
}

void ir_identification_init (id_identification_t* this, int id, int num_trains)
{
    sensor_init ((sensor_t*) this, id);
    this->num_trains = num_trains;

    /*
     * GPIO assignment
     */
    int i;
    for (i = 0; i < (this->num_trains); i++)
    {
        this->gpio[i] = i;
        pinMode (this->gpio[i], INPUT);
    }

    /*
     * Run periodic polling task
     */
    task_add ("IR polling", IR_DEADLINE, ir_identification_polling_task, 

}


void ir_identification_polling_task (ir_identification_t* this)
{
    /*
     * Start periodic polling task
     */
    rt_task_set_periodic (NULL, TM_NOW, IR_PERIOD);
    
    while (1)
    {
        rt_task_wait_period (NULL);
        int i;
        for (i = 0; i < (this->num_trains); i++)
        {
            rt_mutex_acquire (&(this->mutex), TM_INFINITE);

            

        }

    }

}
