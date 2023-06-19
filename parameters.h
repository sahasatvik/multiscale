#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include "agents.h"


/* Set number of agents and environments in simulation */
#define N_AGENTS        10000   // Number of agents
#define N_ENVS          100     // Number of environments

/* Set parameters for network generation */
typedef enum {
        ERDOS_RENYI,
        ALBERT_BARABASI
} graph_methods;

#define GRAPH_METHOD    ERDOS_RENYI
#define ER_GRAPH_P      1e-2    // Probability of an edge between two agents appearing
#define AB_GRAPH_M      25      // Number of new edges created on vertex addition
#define AB_GRAPH_M0     25      // Number of vertices in initial core

/* Set simulation duration and time steps */
#define N_DAYS          365     // Number of days simulated
#define STEPS_PER_DAY   100
#define TIME_STEP       (1.0 / STEPS_PER_DAY)

/* Set probabilities, thresholds, delays */
#define P_INFECT        4e-3    // Probability of infection on contact
#define V_INFECT        1e0     // Viral load barrier
#define V_INFECTUOUS    (1e2 * V_INFECT)
#define A_RECOVERED     1e1     // Factor by which antibody level is raised in recovered
#define ABDY_DELAY      3e0     // Antibody response delay, in days


/* States and parameters for agents, environments */

state_t default_state = {
        .T = 1e4,               // Target cells
        .U = 0.0,               // Infected cells
        .V = 0.0,               // Viral load
        .A = 1.0,               // Immune response (antibodies)
        .W = 0.0,               // Contact viral load
};

params_t default_params = {
        .b     = 1e+2,          // Generation rate of target cells
        .d     = 1e-2,          // Death rate of target cells
        .k     = 1e-5,          // Infection rate of target cells
        .q     = 2e+0,          // Death rate of infected cells

        .p     = 2e+1,          // Production rate of viral cells
        .c     = 1e-1,          // Clearance rate of viral cells

        .b_A   = 2e-2,          // Generation rate of antibodies
        .d_A   = 2e-2,          // Clearance rate of antibodies
        .k_A   = 1e-4,          // Production rate of antibodies

        .alpha = 5e-1,          // Inhibition of contact
        .c_A   = 5e-2,          // Clearance of viral cells due to antibodies

        .epsilon = 2e-1,        // Scale parameter

        .eta   = 5e-7,          // Transmission rate of virus from environment to host
        .zeta  = 5e-3,          // Proportion of viral load transferred during contact

        .v_infect = 1e0,        // Minimum viral load to break barrier
};

env_t default_environment = {
        .Z = 0.0,               // Initial viral concentration

        .xi    = 1.5,           // Viral release rate
        .delta = 1.0,           // Viral removal rate
};

#endif
