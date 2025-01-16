#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "def_constants.h"
#include "def_structs.h"
#include "Func_SIMHYD.h"

/*************
  SMS0 = SMSC * 0.05  #初始时刻土壤湿度，mm
  GW0 =  SMSC * 0.15  #初始时刻地下水储量，mm
  EXC = NULL  #截留后的净雨，mm
  SRUN = NULL #超渗地表径流，mm
  RUNOFF = NULL #总径流，mm
  INT = NULL #壤中流，mm
  INF = NULL #入渗量，mm
  REC = NULL #补充地下水量，mm
  SMF = NULL #补充土壤含水量，mm
  GW = NULL  #地下储水量，mm
  SMS = NULL #土壤湿度，mm
  BAS = NULL #基流，mm
  ET = NULL  #土壤水分蒸发量，mm
 *************/

void Model_SIMHYD(
    double Rainfall,
    double SNOmlt,
    double Eres,
    double SMS0,
    double GW0,
    double *EXC,
    double *SRUN,
    double *RUNOFF,
    double *INT,
    double *INF,
    double *REC,
    double *SMF,
    double *GW,
    double *SMS,
    double *BAS,
    double *ET_soil,
    ST_PARA_SIM Para_sim
)
{
    
    double PP;
    PP = Rainfall + SNOmlt;
    *EXC = PP - Para_sim.INSC;
    if (*EXC >= 0)
    {
        *INF = fmin(Para_sim.COEFF * exp( - Para_sim.SQ * SMS0 / Para_sim.SMSC), *EXC);
        *SRUN = *EXC - *INF;
        *INT = Para_sim.SUB * SMS0 / Para_sim.SMSC * *INF;
        *REC = Para_sim.CRAK * SMS0 / Para_sim.SMSC * (*INF - *INT);
        *SMF = *INF - *INT - *REC;
        *GW = GW0 + *SMF;
        *BAS = Para_sim.Kg * *GW;

        *ET_soil = fmin(Eres, 10 * SMS0 / Para_sim.SMSC);
        *SMS = SMS0 + *SMF - *ET_soil;
        *GW = *GW - *BAS;
        *RUNOFF = *BAS + *INT + *SRUN;
    } else {
        *EXC = 0;
        *INF = 0;
        *SRUN = 0;
        *INT = 0;
        *REC = 0;
        *SMF = 0;
        *GW = GW0 + *SMF;
        *BAS = Para_sim.Kg * *GW;

        *ET_soil = fmin(Eres, 10 * SMS0 / Para_sim.SMSC);
        *SMS = SMS0 + *SMF - *ET_soil;
        *GW = *GW - *BAS;
        *RUNOFF = *BAS + *INT + *SRUN;
    }
    double t;
    if (*SMS > Para_sim.SMSC) {
      t = *SMS - Para_sim.SMSC;
      *GW = *GW + t;
      *SMS = Para_sim.SMSC;
    }

    if (*SMS < 0.1)
    {
        *SMS = 0.1;
        *GW = *GW - (0.1 - *SMS);
        if (*GW < 0)
        {
            *GW = 0;
        }
    }
}

double fmin(
    double x1,
    double x2
)
{
    if (x1 < x2)
    {
        return(x1);
    } else {
        return(x2);
    }    
}

