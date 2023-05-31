#ifndef _AGENTS_H
#define _AGENTS_H


typedef enum status {
        SUSCEPTIBLE,
        INFECTED,
        N_STATUS
} status_t;

typedef struct state {
        double T;       // Target cells
        double U;       // Infected cells
        double V;       // Viral load
        double I;       // Immune response
} state_t;

typedef struct state_delta {
        double dT;
        double dU;
        double dV;
        double dI;
} state_delta_t;

typedef struct params {
        double b;       // Generation rate of target cells
        double d;       // Death rate of target cells
        double k;       // Infection rate of target cells
        double q;       // Death rate of infected cells
        double p;       // Production rate of viral cells
        double c;       // Clearance rate of viral cells

        double eta;     // Transmission rate of virus from environment to host

        double alpha;   // Inverse of incubation period
        double gamma;   // Recovery rate
        double omega;   // Disease induced death rate

        double epsilon; // Scale parameter
} params_t;

typedef struct environment {
        int n_agents;
        struct agent **agents;

        double Z;       // Viral concentration

        double xi_E;    // Viral release rate from pre-symptomatic
        double xi_I;    // Viral release rate from symptomatic
        double delta;   // Viral removal rate

        int n_agents_alloc;
} env_t;

typedef struct agent {
        status_t status;
        state_t  *state;
        params_t *params;
        env_t    *environment;

        int n_history;
        int h_current;  // Index of current state
        int h_next;     // Index of next (future) state
        int h_first;    // Index of first (oldest) state in history
        state_t *history;

        int n_contacts;
        struct agent **contacts;
        double *strengths;

        int n_contacts_alloc;
} agent_t;


/* Create an agent, with given state and memory allocated for contacts */
agent_t *agent_create(
        status_t status,        // Initial status
        state_t  state,         // Initial state
        params_t *parameters,   // Parameters
        int n_history,          // Number of (past) states to store in history
        int n_contacts_alloc    // Number of contacts to pre-allocate memory for
);
/* Free an agent from memory */
void agent_free(agent_t *agent);

/* Add mutual contacts, with given strength */
void agent_add_contact(
        agent_t *first,         // First agent, to be connected to the ...
        agent_t *second,        // ... Second agent.
        double strength         // Strength of the connection
);
/* Get the strength of the contact with a target agent */
double agent_contact_strength(
        agent_t *first,
        agent_t *second
);

/* Calculate an agent's state dt time later, store in history. Uses RK4. */
void agent_step_calculate(
        agent_t *agent,
        double dt               // Time interval
);
/* Step an agent's state forward using the pre-calculated delta */
void agent_step(agent_t *agent);


/* Create an environment, with given state and memory allocated for agents */
env_t *env_create(
        double Z,               // Initial viral load
        double xi_E,            // Viral release rate from pre-symptomatic
        double xi_I,            // Viral release rate from symptomatic
        double delta,           // Viral removal rate
        int n_agents_alloc      // Number of agents to pre-allocate memory for
);
/* Free an environment from memory */
void env_free(env_t *environment);

/* Add an agent to an environment */
void env_add_agent(
        env_t *environment,
        agent_t *agent
);

/* Calculate and step an environment's state forward */
void env_step(
        env_t *environment,
        double dt               // Time step
);

#endif
