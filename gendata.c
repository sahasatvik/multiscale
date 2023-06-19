#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "agents.h"
#include "parameters.h"


agent_t *agents[N_AGENTS];
env_t   *environments[N_ENVS];

double weights[N_AGENTS];
int    choices[N_AGENTS];

/* Flip a biased coin */
int bernoulli(double p) {
        return rand() < (p * RAND_MAX);
}

/* Get a random index from 0 to max - 1 */
int randindex(int max) {
        return rand() % max;
}

/* Get a uniformly random number from [0, 1] */
double uniform() {
        return (double) rand() / (double) RAND_MAX;
}

/* Choose k indices from n, with or without repetition, according to
   supplied weights */
void choose(int n, int k, bool repeat, double *weights, int *choices) {
        double total = 0.0;
        for (int i = 0; i < n; i++) {
                total += weights[i];
                choices[i] = 0;
        }

        /* Compute cdf */
        double *cdf = calloc(n, sizeof(double));
        cdf[0] = weights[0] / total;
        for (int i = 1; i < n; i++)
                cdf[i] = cdf[i - 1] + weights[i] / total;

        for (int trial = 0; trial < k; trial++) {
                int choice = 0;
                do {
                        /* Generate a random number, translate to
                           (weighted) random index */
                        double x = uniform();
                        for (int i = 0; i < n; i++) {
                                if (x <= cdf[i]) {
                                        choice = i;
                                        break;
                                }
                        }
                } while (!repeat && choices[choice]);
                // If no repeats allowed and generated index already
                // chosen, keep trying

                choices[choice]++;
        }

        free(cdf);
}

/* Create randomized contacts, as an Erdos-Renyi random graph */
void erdos_renyi(double p) {

        for (int k = 0; k < N_ENVS; k++) {
                environments[k] = env_create(
                        default_environment.Z,
                        default_environment.xi,
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

/* Create randomized contacts, as an Albert-Barabasi random graph */
void albert_barabasi(int m, int m0) {

        for (int k = 0; k < N_ENVS; k++) {
                environments[k] = env_create(
                        default_environment.Z,
                        default_environment.xi,
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
                        m * m
                );
                env_add_agent(environments[randindex(N_ENVS)], agents[i]);
        }

        /* Initial complete core of m0 vertices */
        for (int i = 0; i < m0; i++)
                for (int j = 0; j < i; j++)
                        agent_add_contact(
                                agents[i],
                                agents[j],
                                1.0
                        );

        /* Preferentially attached remaining vertices */
        for (int i = m0; i < N_AGENTS; i++) {
                for (int j = 0; j < i; j++)
                        weights[j] = agents[j]->n_contacts;
                choose(i, m, false, weights, choices);
                for (int j = 0; j < i; j++)
                        if (choices[j])
                                agent_add_contact(
                                        agents[i],
                                        agents[j],
                                        1.0
                                );
        }
}

/* Save environment and contact network data */
void write_data() {

        /* Save environment assignments */
        FILE *envdata = fopen("data/environments.dat", "w");
        fprintf(envdata, "%d %d\n", N_AGENTS, N_ENVS);
        for (int k = 0; k < N_ENVS; k++) {
                fprintf(envdata, "[%d] : ", environments[k]->n_agents);
                for (int i = 0; i < environments[k]->n_agents; i++)
                        fprintf(envdata, "%d ", environments[k]->agents[i]->id);
                fprintf(envdata, "\n");
        }
        fclose(envdata);

        /* Save contact network assignments */
        FILE *netdata = fopen("data/network.dat", "w");
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

        /* Initialize agent states and contacts, based on chosen method */
        switch(GRAPH_METHOD) {
                case ERDOS_RENYI:
                        erdos_renyi(ER_GRAPH_P);
                        break;
                case ALBERT_BARABASI:
                        albert_barabasi(AB_GRAPH_M, AB_GRAPH_M0);
                        break;
        }

        /* Write to files */
        write_data();

        /* Output degrees to stderr, for analysis */
        for (int i = 0; i < N_AGENTS; i++)
                fprintf(stderr, "%d %d\n", i, agents[i]->n_contacts);

        /* Output edge list to stderr, for analysis */
        /* for (int i = 0; i < N_AGENTS; i++) */
        /*         for (int j = 0; j < agents[i]->n_contacts; j++) */
        /*                 fprintf(stderr, "%d %d ", i, agents[i]->contacts[j]->id); */
        /* fprintf(stderr, "\n"); */

        for (int i = 0; i < N_AGENTS; i++)
                agent_free(agents[i]);
        for (int k = 0; k < N_ENVS; k++)
                env_free(environments[k]);

        return 0;
}
