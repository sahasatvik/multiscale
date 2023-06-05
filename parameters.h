#ifndef _PARAMETERS_H
#define _PARAMETERS_H


#define N_AGENTS        1000    // Number of agents
#define N_ENVS          100     // Number of environments

#define N_DAYS          120     // Number of days simulated
#define STEPS_PER_DAY   100
#define TIME_STEP       (1.0 / STEPS_PER_DAY)

#define P_INFECT        5e-3    // Probability of infection on contact
#define P_GRAPH_EDGE    1e-2    // Probability of an edge between two agents appearing

#define V_INFECT        1e1     // Viral load barrier
#define V_INFECTUOUS    (1e2 * V_INFECT)

#define IFN_DELAY       0.33    // IFN response delay, in days


state_t default_state = {
        .T = 6e4,               // Target cells
        .U = 0.0,               // Infected cells
        .V = 0.0,               // Viral load
        .I = 0.0,               // Immune response
        .W = 0.0,               // Contact viral load
};

params_t default_params = {
        .b = 6e2,               // Generation rate of target cells
        .d = 1e-2,              // Death rate of target cells
        .k = 6e-6,              // Infection rate of target cells
        .q = 2e+0,              // Death rate of infected cells
        .p = 12e0,              // Production rate of viral cells
        .c = 3e-1,              // Clearance rate of viral cells
        .r = 1e-1,              // Production rate of IFN
        .f = 1e0,               // Clearance rate of IFN

        .e1 = 1e-3,             // Inhibition of contact
        .e2 = 1e-3,             // Inhibition of viral production

        .eta = 5e-6,            // Transmission rate of virus from environment to host
        .zeta = 1e-2,           // Proportion of viral load transferred during contact

        .alpha = 1.4e-1,        // Inverse of incubation period
        .gamma = 9.6e-2,        // Recovery rate
        .omega = 1.6e-2,        // Disease induced death rate

        .epsilon = 0.5,         // Scale parameter

        .v_infect = 1e1,        // Minimum viral load to break barrier
};

env_t default_environment = {
        .Z = 0.0,               // Initial viral concentration

        .xi_E = 2.3,            // Viral release rate from pre-symptomatic
        .xi_I = 1.5,            // Viral release rate from symptomatic
        .delta = 1.0,           // Viral removal rate
};

#endif
