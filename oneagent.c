#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "agents.h"


#define N_DAYS          120     // Number of days simulated
#define STEPS_PER_DAY   10
#define TIME_STEP       (1.0 / STEPS_PER_DAY)

#define ABDY_DELAY      3e0     // Antibody response delay, in days


agent_t *agent;
env_t *environment;

state_t init_state = {
        .T = 1e4,               // Target cells
        .A = 1.0,               // Antibodies
};

params_t params = {
        .b     = 1e+2,          // Generation rate of target cells
        .d     = 1e-2,          // Death rate of target cells
        .k     = 1e-5,          // Infection rate of target cells
        .q     = 2e+0,          // Death rate of infected cells

        .p     = 2e+1,          // Production rate of viral copies
        .c     = 1e-1,          // Clearance rate of viral copies

        .b_A   = 2e-2,          // Generation rate of antibodies
        .d_A   = 2e-2,          // Clearance rate of antibodies
        .k_A   = 1e-4,          // Production rate of antibodies

        .alpha = 5e-1,          // Inhibition of contact
        .c_A   = 5e-2,          // Clearance of viral copies due to antibodies

        .lambdai = 4e-1,        // Mean viral load loss
        .epsilon = 2e-1,        // Scale parameter
};


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

        environment = env_create(0.0, 0.0, 0.0, 1);

        env_add_agent(environment, agent);
}

void show(double t) {
        printf(
                "%f %f %f %f %f\n",
                t,
                agent->state->T / init_state.T,
                agent->state->I / init_state.T,
                agent->state->V,
                agent->state->A
        );
}

bool run_model() {

        double max_V = 0.0;
        double delay = 47.0;

        int steps = 0;
        for (double t = 0.0; t <= N_DAYS; t += TIME_STEP, steps += 1) {
                show(t);

                agent_step_calculate(agent, TIME_STEP);
                agent_step(agent);

                /* Infect and reinfect at scheduled times */
                if (steps == (10 * STEPS_PER_DAY)
                 || steps == ((int) ((10 + delay) * STEPS_PER_DAY)))
                        agent->state->W = 1.0;
                if (steps == ((10 + 1) * STEPS_PER_DAY)
                 || steps == ((int) ((10 + delay + 1) * STEPS_PER_DAY)))
                        agent->state->W = 0.0;

                if (steps > ((10 + delay) * STEPS_PER_DAY)
                 && agent->state->V > max_V)
                        max_V = agent->state->V;
        }

        return max_V > 100.0;
}

void free_model() {
        agent_free(agent);
        env_free(environment);
}


int main(int argc, const char *argv[]) {

        srand(time(NULL));

        initialize_model();
        run_model();
        free_model();

        return 0;
}
