#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "agents.h"
#include "parameters.h"


agent_t *agents[N_AGENTS];
env_t   *environments[N_ENVS];


/* Flip a biased coin */
int bernoulli(double p) {
        return rand() < (p * RAND_MAX);
}

/* Get a random index from 0 to max - 1 */
int randindex(int max) {
        return rand() % max;
}

/* Create randomized contacts, as an Erdos-Renyi random graph */
void agents_randomize_contacts(double p) {

        for (int k = 0; k < N_ENVS; k++) {
                environments[k] = env_create(
                        default_environment.Z,
                        default_environment.xi_E,
                        default_environment.xi_I,
                        default_environment.delta,
                        N_AGENTS / N_ENVS
                );
        }

        for (int i = 0; i < N_AGENTS; i++) {
                agents[i] = agent_create(
                        i,                                      // id
                        SUSCEPTIBLE,                            // default status
                        default_state,                          // default state
                        &default_params,                        // default parameters
                        (int) (ABDY_DELAY * STEPS_PER_DAY),     // keep a state history
                        (int) (N_AGENTS * p * (2 - p))          // pre-allocate memory for
                                                                // ~ mean + 1 std contacts
                );
                env_add_agent(environments[randindex(N_ENVS)], agents[i]);
        }


        /* Each edge in the contact network has a probability p of appearing */
        for (int i = 0; i < N_AGENTS; i++)
                for (int j = 0; j < i; j++)
                        if (bernoulli(p))
                                agent_add_contact(
                                        agents[i],
                                        agents[j],
                                        1.0
                                );
}

/* Save environment and contact network data */
void write_data() {

        /* Save environment assignments */
        FILE *envdata = fopen("environments.dat", "w");
        fprintf(envdata, "%d %d\n", N_AGENTS, N_ENVS);
        for (int k = 0; k < N_ENVS; k++) {
                fprintf(envdata, "[%d] : ", environments[k]->n_agents);
                for (int i = 0; i < environments[k]->n_agents; i++)
                        fprintf(envdata, "%d ", environments[k]->agents[i]->id);
                fprintf(envdata, "\n");
        }
        fclose(envdata);

        /* Save contact network assignments */
        FILE *netdata = fopen("network.dat", "w");
        fprintf(netdata, "%d\n", N_AGENTS);
        for (int i = 0; i < N_AGENTS; i++) {
                int m = 0;
                for (int j = 0; j < agents[i]->n_contacts; j++) {
                        if (i < agents[i]->contacts[j]->id)
                                continue;
                        m++;
                }
                fprintf(netdata, "[%d, %d] : ", agents[i]->n_contacts, m);
                for (int j = 0; j < agents[i]->n_contacts; j++) {
                        if (i < agents[i]->contacts[j]->id)
                                continue;
                        fprintf(
                                netdata,
                                "{%d %f} ",
                                agents[i]->contacts[j]->id,
                                agents[i]->strengths[j]
                        );
                }
                fprintf(netdata, "\n");
        }
        fclose(netdata);
}


int main(int argc, const char *argv[]) {

        srand(time(NULL));

        /* Initialize agent states and contacts */
        agents_randomize_contacts(P_GRAPH_EDGE);

        /* Write to files */
        write_data();

        for (int i = 0; i < N_AGENTS; i++)
                agent_free(agents[i]);
        for (int k = 0; k < N_ENVS; k++)
                env_free(environments[k]);

        return 0;
}
