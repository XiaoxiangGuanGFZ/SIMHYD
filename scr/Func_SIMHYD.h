#ifndef SIMHYD_module
#define SIMHYD_module

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
);


double fmin(
    double x1,
    double x2
);


#endif
