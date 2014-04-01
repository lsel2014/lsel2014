
struct train {
char train_type;
char current_sector;
SRTIME current_time;
char prev_sector;
SRTIME prev_time;
int velocidad; //velocidad en codigo 0,4,8,12,16...
double velocidad_fis;
char sentido;
int dist_restante;
double tiempo_restante;
};
extern struct train diesel_t;
extern void cambiarVia(int i);
