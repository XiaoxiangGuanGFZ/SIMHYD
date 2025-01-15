#ifndef structs
#define structs


typedef struct
{
    /* data */
    char FP_DATA[300];
    char FP_OUT[300];
    char PARA_SNOW[1000];
    char PARA_SIM[1000];
    char FLAG_OBS[10];
    char FLAG_MUTE[10];
    int CALC_N;

} ST_GP;

typedef struct
{
    double Prec;  // precipitation, mm
    double Tair;  // daily average air temperature
    double Tmax;  // daily maximum air temperature
    double Epot;  // potential evapotranspiration, mm 
    double Qobs;  // observated streamflow, mm
} ST_VAR_IN;


typedef struct 
{
    double EXC;
    double SRUN;
    double RUNOFF;
    double INT;
    double INF;
    double REC;
    double SMF;
    double GW;
    double SMS;
    double BAS;
    double ET_soil;
} ST_VAR_SIMHYD;


typedef struct 
{
    double Rainfall;
    double Snowfall;
    double SNO;
    double Tsnow;
    double SNOmlt;
    double Eres;
} ST_VAR_SNOW;


/********
  # INSC = 1.6      #截留储藏能力，mm
  # COEFF = 322.189 #最大入渗损失，mm
  # SMSC = 453.701   #土壤持水能力，mm
  # SUB = 0.184  #壤中流方程比例系数
  # SQ = 7.915   #入渗损失指数
  # CRAK = 0.977 #地下水方程比例系数
  # Kg = 0.007   #基流退水系数
 */
typedef struct 
{
    double INSC;
    double COEFF;
    double SMSC;
    double SUB;
    double SQ;
    double CRAK;
    double Kg;
} ST_PARA_SIM;


typedef struct 
{
    double Trs;
    double Tmlt;
    double SNO50;
    double SNO100;
    double Ls;
    double Bmlt6;
    double Bmlt12;
} ST_PARA_SNOW;


typedef struct
{
    int y;
    int m;
    int d;
    int dn; // number of the day in a year
} ST_DATE;



#endif


