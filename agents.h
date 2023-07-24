#ifndef _AGENTS_H
#define _AGENTS_H

#ifndef EPSILON_COLLAPSE
#define EPSILON_COLLAPSE        1e-6
#endif


typedef enum status {
        SUSCEPTIBLE,
        INFECTED,
        RECOVERED,
        N_STATUS
} status_t;

typedef struct state {
        double T;       // Target cells                         [T]
        double I;       // Infected cells                       [T^*]
        double V;       // Viral load                           [V]
        double A;       // Immune response (antibodies)         [A]
        double W;       // Contact viral load                   [W]
} state_t;

typedef struct state_delta {
        double dT;
        double dI;
        double dV;
        double dA;
} state_delta_t;

typedef struct params {
        double b;       // Generation rate of target cells      [\beta]
        double d;       // Death rate of target cells           [\delta]
        double k;       // Infection rate of target cells       [\kappa]
        double q;       // Death rate of infected cells         [q]
        double p;       // Production rate of viral copies      [p]
        double c;       // Clearance rate of viral copies       [c]
        double b_A;     // Generation rate of antibodies        [\beta_A]
        double k_A;     // Production rate of antibodies        [\kappa_A]
        double d_A;     // Clearance rate of antibodies         [\delta_A]

        double alpha;   // Inhibition of contact                [\alpha]
        double c_A;     // Clearance rate of viral copies due to antibodies
                        //                                      [c_A]

        double eta;     // Transmission rate of virus from environment to host
                        //                                      [\eta]
        double zeta;    // Proportion of viral load transferred during contact
                        //                                      [\zeta]

        double epsilon; // Scale parameter

        double v_infect;// Minimum viral load to break barrier  [v]
        double lambdai; // Mean viral load loss                 [1 / \lambda]
} params_t;

typedef struct environment {
        int n_agents;
        struct agent **agents;

        double Z;       // Viral concentration

        double xi;      // Viral release rate
        double delta;   // Viral removal rate

        int n_agents_alloc;
} env_t;

typedef struct agent {
        int id;

        status_t status;
        state_t  *state;
        params_t *params;
        env_t    *environment;

        int n_history;
        int h_previous; // Index of previous state
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
agent_t *                       // Pointer to agent
agent_create(
        int id,                 // Identification number
        status_t status,        // Initial status
        state_t  state,         // Initial state
        params_t *parameters,   // Parameters
        int n_history,          // Number of (past) states to store in history
        int n_contacts_alloc    // Number of contacts to pre-allocate memory for
);

/* Free an agent from memory */
void
agent_free(agent_t *agent);

/* Add mutual contacts, with given strength */
void
agent_add_contact(
        agent_t *first,         // First agent, to be connected to the ...
        agent_t *second,        // ... Second agent.
        double strength         // Strength of the connection
);

/* Get the strength of the contact with a target agent */
double                          // Strength of contact
agent_contact_strength(
        agent_t *first,
        agent_t *second
);

/* Calculate an agent's state dt time later, store in history. Uses RK4. */
void
agent_step_calculate(
        agent_t *agent,
        double dt               // Time interval
);

/* Step an agent's state forward using the pre-calculated delta */
void
agent_step(agent_t *agent);


/* Create an environment, with given state and memory allocated for agents */
env_t *                         // Pointer to environment
env_create(
        double Z,               // Initial viral load
        double xi,              // Viral release rate
        double delta,           // Viral removal rate
        int n_agents_alloc      // Number of agents to pre-allocate memory for
);

/* Free an environment from memory */
void
env_free(env_t *environment);

/* Add an agent to an environment */
void
env_add_agent(
        env_t *environment,
        agent_t *agent
);

/* Calculate and step an environment's state forward */
void
env_step(
        env_t *environment,
        double dt               // Time step
);

#endif
