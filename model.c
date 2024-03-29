#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "agents.h"
#include "parameters.h"
#include "random.h"

#define N_THREADS       4
#define SHOW_EVERY      (STEPS_PER_DAY / 10)    // skip steps in output
#define SHOW_N_AGENTS   3
#define STATUS_EVERY    (10 * STEPS_PER_DAY)    // show quick status

bool showstatus = false;


agent_t *agents[N_AGENTS];
env_t   *environments[N_ENVS];


/* Initialize agent and environment data from data files */
void initialize_model() {

        /* Load agent data */
        FILE *netdata = fopen("data/network.dat", "r");
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

        /* Load environment data */
        FILE *envdata = fopen("data/environments.dat", "r");
        fscanf(envdata, "%*d %*d\n");
        for (int k = 0; k < N_ENVS; k++) {
                int n, i;
                fscanf(envdata, "[%d] : ", &n);
                environments[k] = env_create(
                        default_environment.Z,
                        default_environment.xi,
                        default_environment.delta,
                        n
                );
                for (int l = 0; l < n; l++) {
                        fscanf(envdata, "%d ", &i);
                        env_add_agent(environments[k], agents[i]);
                }
        }
        fclose(envdata);

        /* Infect a single agent */
        agents[0]->status = INFECTED;
        for (double t = 0.0; t <= 1.0; t += TIME_STEP) {
                agents[0]->state->W = 10.0;
                agent_step_calculate(agents[0], TIME_STEP);
                agent_step(agents[0]);
        }
        agents[0]->state->W = 0.0;
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
                        if (random_bernoulli(P_INFECT * agents[i]->strengths[j])) {
                                agents[i]->history[agents[i]->h_next].W +=
                                        agents[j]->params->zeta * agents[j]->state->V;
                        }
                }
        }

        pthread_exit(NULL);
        return NULL;
}

_Atomic int newI;

void *step(void *args) {

        thread_data_t *d = (thread_data_t *) args;

        /* Loop through all assigned agents */
        for (int i = d->start; i < d->end; i++) {
                agent_step(agents[i]);
                if (!d->day)
                        continue;
                switch (agents[i]->status) {
                        case SUSCEPTIBLE:
                                if (agents[i]->state->V > V_INFECTUOUS) {
                                        agents[i]->status = INFECTED;
                                        newI++;
                                }
                                break;
                        case INFECTED:
                                if (agents[i]->state->V < 1e-3)
                                        agents[i]->status = RECOVERED;
                                break;
                        case RECOVERED:
                                if (agents[i]->state->V > V_INFECTUOUS) {
                                        agents[i]->status = INFECTED;
                                        newI++;
                                } else if (agents[i]->state->A < A_RECOVERED *
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


FILE *agentdata;
FILE *countdata;
FILE *averagedata;
FILE *environmentdata;

bool show(double t, bool day) {

        fprintf(agentdata, "%f", t);
        for (int i = 0; i < SHOW_N_AGENTS; i++)
                fprintf(
                        agentdata,
                        ", %f, %f, %f, %f, %f",
                        agents[i]->state->T / default_state.T,
                        agents[i]->state->I / default_state.T,
                        agents[i]->state->V / V_INFECT,
                        (
                                agents[i]->state->W
                                + agents[i]->params->eta * agents[i]->environment->Z
                        ) / V_INFECT,
                        agents[i]->state->A
                );
        fprintf(agentdata, "\n");

        int count[N_STATUS] = { 0 };

        /* Compute totals */
        double T = 0.0;
        double I = 0.0;
        double V = 0.0;
        double A = 0.0;

        for (int i = 0; i < N_AGENTS; i++) {
                count[agents[i]->status]++;

                T += agents[i]->state->T;
                I += agents[i]->state->I;
                V += agents[i]->state->V;
                A += agents[i]->state->A;
        }

        /* Only show status counts once per day */
        if (day) {
                fprintf(countdata, "%f", t);
                for (int j = 0; j < N_STATUS; j++)
                        fprintf(countdata, ", %d", count[j]);
                fprintf(countdata, ", %d\n", newI);
                newI = 0;
        }

        fprintf(
                averagedata,
                "%f, %f, %f, %f, %f \n",
                t,
                T / (N_AGENTS * default_state.T),
                I / (N_AGENTS * default_state.T),
                V / N_AGENTS,
                A / N_AGENTS
        );

        fprintf(environmentdata, "%f", t);
        for (int j = 0; j < N_ENVS; j++)
                fprintf(environmentdata, ", %f", environments[j]->Z);
        fprintf(environmentdata, "\n");

        return count[INFECTED] == 0;
}

void argparse(int argc, char *argv[]) {
        int opt;
        while ((opt = getopt(argc, argv, "hsp:")) != -1) {
                switch (opt) {
                        case 's':
                                showstatus = true;
                                break;
                        case 'p':
                                P_INFECT = atof(optarg);
                                break;
                        case 'h':
                        default:
                                fprintf(
                                        stderr,
                                        "Usage : %s [-hs] [-p infection_probability]\n"
                                        "    -h         Show this help message\n"
                                        "    -s         Show status every SHOW_STATUS steps\n"
                                        "    -p infection_probability\n"
                                        "               Set infection probability on contact between individuals\n",
                                        argv[0]
                                );
                                exit(EXIT_FAILURE);
                }
        }
}

int main(int argc, char *argv[]) {

        argparse(argc, argv);

        srand(time(NULL));

        /* Partition information for threads */
        for (int j = 0; j < N_THREADS; j++) {
                thread_data[j].start = (j * N_AGENTS) / N_THREADS;
                thread_data[j].end   = ((j + 1) * N_AGENTS) / N_THREADS;
                thread_data[j].day   = false;
        }

        /* Initialize agent states and contacts */
        initialize_model();

        /* Open data files */
        agentdata       = fopen("output/agentdata.dat", "w");
        countdata       = fopen("output/countdata.dat", "w");
        averagedata     = fopen("output/averagedata.dat", "w");
        environmentdata = fopen("output/environmentdata.dat", "w");

        /* Prepare data file headers */
        fprintf(agentdata, "Time, ");
        for (int i = 0; i < SHOW_N_AGENTS; i++)
                fprintf(agentdata, "Target, Infected, Viral, \"Viral (External)\", Antibodies, ");
        fprintf(agentdata, "\n");

        fprintf(countdata, "Time, Susceptible, Infected, Recovered, New Infected\n");
        fprintf(averagedata, "Time, Target, Infected, Viral, Antibodies\n");
        fprintf(environmentdata, "Time, ");
        for (int j = 0; j < N_ENVS; j++)
                fprintf(environmentdata, "Z_%d, ", j);
        fprintf(environmentdata, "\n");

        /* Step through time */
        int steps = 0;
        bool extinct = false;
        for (double t = 0.0; t <= N_DAYS && !extinct; t += TIME_STEP, steps += 1) {
                /* Output data to stdout */
                if (steps % SHOW_EVERY == 0)
                        extinct = show(t, (steps % STEPS_PER_DAY) == 0);
                /* Output a brief status summary to stderr */
                if (showstatus && (steps % STATUS_EVERY == 0))
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

        fprintf(stderr, "Total days : %d\n", steps / STEPS_PER_DAY);

        /* Close output files */
        fclose(agentdata);
        fclose(countdata);
        fclose(averagedata);
        fclose(environmentdata);

        /* Free agents, environments */
        for (int i = 0; i < N_AGENTS; i++)
                agent_free(agents[i]);
        for (int i = 0; i < N_ENVS; i++)
                env_free(environments[i]);

        return 0;
}
