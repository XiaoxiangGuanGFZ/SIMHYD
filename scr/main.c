
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "def_constants.h"
#include "def_structs.h"
#include "Func_IO.h"
#include "Func_Snow.h"
#include "Func_View.h"
#include "Func_Para.h"
#include "Func_SIMHYD.h"
#include "Func_Metrics.h"

int flag_obs;
int flag_mute;
int flag_snow;

int main(int argc, char * argv[])
{
    ST_GP GP;
    import_global(*(++argv), &GP);
    flag_obs = 0;
    flag_mute = 0;
    flag_snow = 1;
    if (strncmp(GP.FLAG_OBS, "TRUE", 4) == 0)
    {
        flag_obs = 1;
    }
    if (strncmp(GP.FLAG_MUTE, "TRUE", 4) == 0)
    {
        flag_mute = 1;
    }
    if (flag_mute == 0) {Preview_GP(&GP);}

    ST_DATE *ts_date;
    ST_VAR_IN *p_vars_in;
    ts_date = (ST_DATE *)malloc(sizeof(ST_DATE) * GP.CALC_N);
    p_vars_in = (ST_VAR_IN *)malloc(sizeof(ST_VAR_IN) * GP.CALC_N);
    import_data(GP.FP_DATA, GP.CALC_N, ts_date, p_vars_in);
    if (flag_mute == 0){Preview_data(6, GP.CALC_N, ts_date, p_vars_in);}
    for (size_t i = 0; i < GP.CALC_N; i++)
    {
        (ts_date+i)->dn = calculate_day_number((ts_date+i)->y, (ts_date+i)->m, (ts_date+i)->d);
    }
    
    ST_PARA_SIM Para_sim;
    ST_PARA_SNOW Para_snow;
    Para_sim_parse(GP.PARA_SIM,&Para_sim);
    if (strcmp(GP.PARA_SNOW, "FALSE") == 0)
    {
        flag_snow = 0;
    }
    if (flag_snow == 1)
    {
        Para_snow_parse(GP.PARA_SNOW, &Para_snow);
    }
    
    if (flag_mute == 0) {
        Preview_Para_sim(Para_sim);
        if (flag_snow == 1){Preview_Para_snow(Para_snow);}
    }

    if (flag_mute == 0){printf("------------ modeling: \n");}
    
    ST_VAR_SNOW *p_var_snow;
    ST_VAR_SIMHYD *p_var_sim;
    p_var_snow = (ST_VAR_SNOW *)malloc(sizeof(ST_VAR_SNOW) * GP.CALC_N);
    p_var_sim = (ST_VAR_SIMHYD *)malloc(sizeof(ST_VAR_SIMHYD) * GP.CALC_N);

    if (flag_snow == 1)
    {
        for (size_t i = 0; i < GP.CALC_N; i++)
        {
            P_Rain_Snow(
                (p_vars_in + i)->Prec,
                Para_snow.Trs,
                (p_vars_in + i)->Tair,
                &((p_var_snow + i)->Rainfall),
                &((p_var_snow + i)->Snowfall));
        }
    } else {
        for (size_t i = 0; i < GP.CALC_N; i++)
        {
            (p_var_snow + i)->Rainfall = (p_vars_in + i)->Prec;
            (p_var_snow + i)->Snowfall = 0.0;
        }        
    }
    
    double SNO0, Tsnow0, SMS0, GW0;
    for (size_t i = 0; i < GP.CALC_N; i++)
    {
        if (i == 0)
        {
            SNO0 = 5;
            Tsnow0 = -5;
            SMS0 = Para_sim.SMSC * 0.05;
            GW0 = Para_sim.SMSC * 0.15;
        } else {
            SNO0 = (p_var_snow + i - 1)->SNO;
            Tsnow0 = (p_var_snow + i - 1)->Tsnow;
            SMS0 = (p_var_sim + i - 1)->SMS;
            GW0 = (p_var_sim + i - 1)->GW;
        }

        Model_SNOW(
            (ts_date + i)->dn,
            (p_var_snow + i)->Snowfall,
            (p_vars_in + i)->Tair,
            (p_vars_in + i)->Tmax,
            (p_vars_in + i)->Epot,
            Tsnow0,
            SNO0,
            &((p_var_snow + i)->Tsnow),
            &((p_var_snow + i)->SNO),
            &((p_var_snow + i)->SNOmlt),
            &((p_var_snow + i)->Eres),
            Para_snow.SNO50,
            Para_snow.SNO100,
            Para_snow.Ls,
            Para_snow.Bmlt6,
            Para_snow.Bmlt12,
            Para_snow.Tmlt);

        Model_SIMHYD(
            (p_var_snow + i)->Rainfall,
            (p_var_snow + i)->SNOmlt,
            (p_var_snow + i)->Eres,
            SMS0,
            GW0,
            &((p_var_sim + i)->EXC),
            &((p_var_sim + i)->SRUN),
            &((p_var_sim + i)->RUNOFF),
            &((p_var_sim + i)->INT),
            &((p_var_sim + i)->INF),
            &((p_var_sim + i)->REC),
            &((p_var_sim + i)->SMF),
            &((p_var_sim + i)->GW),
            &((p_var_sim + i)->SMS),
            &((p_var_sim + i)->BAS),
            &((p_var_sim + i)->ET_soil),
            Para_sim);
    }
    if (flag_mute == 0){printf("------------ write results: \n");}
    Write_sim2csv(
        GP.FP_OUT,
        ts_date,
        p_vars_in,
        p_var_snow,
        p_var_sim,
        GP.CALC_N);
    if (flag_obs == 1 && flag_mute == 0)
    {
        /*********************
         * metric calculation: NSE, R2, RMSE, MSE, Re (%)
         * *******************/
        double nse = -99.9;
        double MSE = 99.9;
        double RMSE = 99.9;
        double R2 = 0.0;
        double Re = -99.9;
        double *Qobs, *Qsim;
        Qobs = (double *)malloc(sizeof(double) * GP.CALC_N);
        Qsim = (double *)malloc(sizeof(double) * GP.CALC_N);
        for (size_t i = 0; i < GP.CALC_N; i++)
        {
            *(Qobs + i) = (p_vars_in + i)->Qobs;
            *(Qsim + i) = (p_var_sim + i)->RUNOFF;
        }
        Metrics(Qsim, Qobs, GP.CALC_N, &nse, &R2, &RMSE, &MSE, &Re);
        printf("------------ Metrics (obs vs. sim) \n");
        printf("%-4s:%6.3f\n%-4s:%6.3f\n%-4s:%6.3f\n%-4s:%6.3f\n%-4s:%6.2f%%\n",
               "NSE", nse,
               "R2", R2,
               "RMSE", RMSE,
               "MSE", MSE,
               "Re", Re * 100);
    }
    if (flag_mute == 0){printf("------------ DONE! \n");}
    free(ts_date);
    free(p_vars_in);
    free(p_var_snow);
    free(p_var_sim);
    return(0);
}



