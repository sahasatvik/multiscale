#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "agents.h"


#define N_DAYS           80     // Number of days simulated
#define STEPS_PER_DAY   100
#define TIME_STEP       (1.0 / STEPS_PER_DAY)

#define ABDY_DELAY      3e0     // Antibody response delay, in days

#define INFECT_ON       (10 * STEPS_PER_DAY)
                                // Infect the agent at specified time


agent_t *agent;

state_t init_state = {
        .T = 1e4,               // Target cells
        .A = 1.0,               // Antibodies
};

params_t params = {
        .b     = 1e2,           // Generation rate of target cells
        .d     = 1e-2,          // Death rate of target cells
        .k     = 1e-5,          // Infection rate of target cells
        .q     = 2e+0,          // Death rate of infected cells

        .p     = 12e0,          // Production rate of viral cells
        .c     = 3e-1,          // Clearance rate of viral cells

        .b_A   = 1e0,           // Generation rate of antibodies
        .d_A   = 1e0,           // Clearance rate of antibodies
        .k_A   = 3e-3,          // Production rate of antibodies
        .alpha = 1e-2,          // Inhibition of contact
        .c_A   = 1e-2,          // Clearance of viral cells due to antibodies

        .epsilon = 2e-1,        // Scale parameter
};


/* Non-interacting environment */
env_t environment = { 0.0 };


/* Initialize agent and environment data */
void initialize_model() {

        agent = agent_create(
                0,              // id
                SUSCEPTIBLE,    // default status
                init_state,     // default state
                &params,        // default parameters
                (int) (ABDY_DELAY * STEPS_PER_DAY),
                                // state history size
                0               // contacts
        );

        env_add_agent(&environment, agent);
}


void show(double t) {
        printf(
                "%f %f %f %f %f\n",
                t,
                agent->state->T / init_state.T,
                agent->state->U / init_state.T,
                agent->state->V,
                agent->state->A
        );
}

int main(int argc, const char *argv[]) {

        initialize_model();

        int steps = 0;
        for (double t = 0.0; t <= N_DAYS; t += TIME_STEP, steps += 1) {
                show(t);

                agent_step_calculate(agent, TIME_STEP);
                agent_step(agent);

                /* Infect at scheduled time */
                if (steps == INFECT_ON)
                        agent->state->V += 1e-2;
        }

        agent_free(agent);

        return 0;
}
