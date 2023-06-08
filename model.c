#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "agents.h"
#include "parameters.h"

#define N_THREADS       4
#define SHOW_EVERY      2                       // skip steps in output
#define STATUS_EVERY    (10 * STEPS_PER_DAY)    // show quick status


agent_t *agents[N_AGENTS];
env_t   *environments[N_ENVS];


/* Flip a biased coin */
int bernoulli(double p) {
        return rand() < (p * RAND_MAX);
}

/* Initialize agent and environment data from data files */
void initialize_model() {

        /* Load agent data */
        FILE *netdata = fopen("network.dat", "r");
        fscanf(netdata, "%*d\n");
        for (int i = 0; i < N_AGENTS; i++) {
                int m, n, j;
                double s;
                fscanf(netdata, "[%d, %d] : ", &n, &m);
                agents[i] = agent_create(
                        i,                                      // id
                        SUSCEPTIBLE,                            // default status
                        default_state,                          // default state
                        &default_params,                        // default parameters
                        (int) (ABDY_DELAY * STEPS_PER_DAY),     // keep a state history
                        n
                );
                for (int l = 0; l < m; l++) {
                        fscanf(netdata, "{%d %lf} ", &j, &s);
                        agent_add_contact(
                                agents[i],
                                agents[j],
                                s
                        );
                }
        }
        fclose(netdata);

        /* Infect a single agent */
        agents[0]->status = INFECTED;
        agents[0]->state->V = V_INFECT;


        /* Load environment data */
        FILE *envdata = fopen("environments.dat", "r");
        fscanf(envdata, "%*d %*d\n");
        for (int k = 0; k < N_ENVS; k++) {
                int n, i;
                fscanf(envdata, "[%d] : ", &n);
                environments[k] = env_create(
                        default_environment.Z,
                        default_environment.xi_E,
                        default_environment.xi_I,
                        default_environment.delta,
                        n
                );
                for (int l = 0; l < n; l++) {
                        fscanf(envdata, "%d ", &i);
                        env_add_agent(environments[k], agents[i]);
                }
        }
        fclose(envdata);
}


typedef struct {
        int start;
        int end;
        bool day;
} thread_data_t;

thread_data_t thread_data[N_THREADS];
pthread_t thread_ids[N_THREADS];

void *step_calculate(void *args) {

        thread_data_t *d = (thread_data_t *) args;

        /* Loop through all assigned agents */
        for (int i = d->start; i < d->end; i++) {
                agent_step_calculate(agents[i], TIME_STEP);
                /* Calculate infection spread via contact once per day */
                if (!d->day)
                        continue;
                agents[i]->history[agents[i]->h_next].W = 0.0;
                for (int j = 0; j < agents[i]->n_contacts; j++) {
                        if (bernoulli(P_INFECT * agents[i]->strengths[j])) {
                                agents[i]->history[agents[i]->h_next].W +=
                                        agents[j]->params->zeta * agents[j]->state->V;
                        }
                }
        }

        pthread_exit(NULL);
        return NULL;
}

void *step(void *args) {

        thread_data_t *d = (thread_data_t *) args;

        /* Loop through all assigned agents */
        for (int i = d->start; i < d->end; i++) {
                agent_step(agents[i]);
                switch (agents[i]->status) {
                        case SUSCEPTIBLE:
                                if (agents[i]->state->V > V_INFECTUOUS)
                                        agents[i]->status = INFECTED;
                                break;
                        case INFECTED:
                                if (agents[i]->state->V < 1e-3)
                                        agents[i]->status = RECOVERED;
                                break;
                        case RECOVERED:
                                if (agents[i]->state->V > V_INFECTUOUS)
                                        agents[i]->status = INFECTED;
                                else if (agents[i]->state->A < A_RECOVERED *
                                        agents[i]->params->b_A / agents[i]->params->d_A)
                                        agents[i]->status = SUSCEPTIBLE;
                                break;
                        default:
                                break;
                }
        }

        pthread_exit(NULL);
        return NULL;
}

void show(double t) {
        printf(
                "%f %f %f %f %f %f %f ",
                t,
                agents[0]->state->T / default_state.T,
                agents[0]->state->U / default_state.T,
                agents[0]->state->V / V_INFECT,
                (
                        agents[0]->state->W
                        + agents[0]->params->eta * agents[0]->environment->Z
                ) / V_INFECT,
                agents[0]->state->A,
                agents[0]->environment->Z
        );
        printf(
                "%f %f %f %f %f %f %f ",
                t,
                agents[1]->state->T / default_state.T,
                agents[1]->state->U / default_state.T,
                agents[1]->state->V / V_INFECT,
                (
                        agents[1]->state->W
                        + agents[1]->params->eta * agents[1]->environment->Z
                ) / V_INFECT,
                agents[1]->state->A,
                agents[1]->environment->Z
        );
        printf(
                "%f %f %f %f %f %f %f ",
                t,
                agents[50]->state->T / default_state.T,
                agents[50]->state->U / default_state.T,
                agents[50]->state->V / V_INFECT,
                (
                        agents[50]->state->W
                        + agents[50]->params->eta * agents[50]->environment->Z
                ) / V_INFECT,
                agents[50]->state->A,
                agents[50]->environment->Z
        );

        int count[N_STATUS] = { 0 };
        for (int i = 0; i < N_AGENTS; i++)
                count[agents[i]->status]++;
        for (int j = 0; j < N_STATUS; j++)
                printf("%d ", count[j]);

        printf("\n");
}

int main(int argc, const char *argv[]) {

        srand(time(NULL));

        /* Partition information for threads */
        for (int j = 0; j < N_THREADS; j++) {
                thread_data[j].start = (j * N_AGENTS) / N_THREADS;
                thread_data[j].end   = ((j + 1) * N_AGENTS) / N_THREADS;
                thread_data[j].day   = false;
        }

        /* Initialize agent states and contacts */
        initialize_model();

        /* Step through time */
        int steps = 0;
        for (double t = 0.0; t <= N_DAYS; t += TIME_STEP, steps += 1) {
                /* Output data to stdout */
                if (steps % SHOW_EVERY == 0)
                        show(t);
                /* Output a brief status summary to stderr */
                if (steps % STATUS_EVERY == 0)
                        fprintf(stderr, "Day %d\n", steps / STEPS_PER_DAY);

                /* Each thread deals with a subset of agents */
                /* Calculate each agent's next state */
                for (int j = 0; j < N_THREADS; j++) {
                        thread_data[j].day = (steps % STEPS_PER_DAY == 0);
                        pthread_create(&(thread_ids[j]), NULL, step_calculate, &(thread_data[j]));
                }
                for (int j = 0; j < N_THREADS; j++)
                        pthread_join(thread_ids[j], NULL);


                /* Update each agent's state */
                for (int j = 0; j < N_THREADS; j++)
                        pthread_create(&(thread_ids[j]), NULL, step, &(thread_data[j]));
                for (int j = 0; j < N_THREADS; j++)
                        pthread_join(thread_ids[j], NULL);

                /* Calculate and update each environment */
                for (int i = 0; i < N_ENVS; i++)
                        env_step(environments[i], TIME_STEP);
        }

        for (int i = 0; i < N_AGENTS; i++)
                agent_free(agents[i]);
        for (int i = 0; i < N_ENVS; i++)
                env_free(environments[i]);

        return 0;
}
