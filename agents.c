#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "agents.h"
#include "random.h"


agent_t *agent_create(
        int id,
        status_t status,
        state_t state,
        params_t *params,
        int n_history,
        int n_contacts_alloc
) {
        agent_t *a    = malloc(sizeof(agent_t));
        a->id         = id;
        a->status     = status;
        a->params     = params;
        a->h_previous = n_history;
        a->h_current  = 0;
        a->h_next     = 1;
        a->h_first    = 2 % (n_history + 1);
        a->history    = calloc(n_history + 1, sizeof(state_t));
        for (int i = 0; i < n_history + 1; i++)
                a->history[i] = state;
        a->state      = &(a->history[a->h_current]);
        a->n_history  = n_history;
        a->n_contacts = 0;
        a->contacts   = malloc(n_contacts_alloc * sizeof(agent_t *));
        a->strengths  = malloc(n_contacts_alloc * sizeof(double));
        a->n_contacts_alloc = n_contacts_alloc;
        return a;
}

void agent_free(agent_t *a) {
        if (a == NULL)
                return;
        free(a->history);
        free(a->contacts);
        free(a->strengths);
        free(a);
}

static void _agent_add_contact(
        agent_t *current, agent_t *target,
        double strength
) {
        /* Allocate more memory if necessary */
        if (current->n_contacts >= current->n_contacts_alloc) {
                if (current->n_contacts_alloc <= 0)
                        current->n_contacts_alloc = 1;

                current->n_contacts_alloc *= 2;
                current->contacts = realloc(
                        current->contacts,
                        current->n_contacts_alloc * sizeof(agent_t *)
                );
                current->strengths = realloc(
                        current->strengths,
                        current->n_contacts_alloc * sizeof(double)
                );
        }

        current->contacts[current->n_contacts] = target;
        current->strengths[current->n_contacts] = strength;
        current->n_contacts += 1;
}

void agent_add_contact(
        agent_t *a, agent_t *b,
        double strength
) {
        _agent_add_contact(a, b, strength);
        _agent_add_contact(b, a, strength);
}

double agent_contact_strength(agent_t *current, agent_t *target) {
        for (int i = 0; i < current->n_contacts; i++)
                if (current->contacts[i] == target)
                        return current->strengths[i];
        return 0.0;
}


env_t *env_create(
        double Z,
        double xi, double delta,
        int n_agents_alloc
) {
        env_t *e    = malloc(sizeof(env_t));
        e->n_agents = 0;
        e->Z        = Z;
        e->xi       = xi;
        e->delta    = delta;
        e->agents   = malloc(n_agents_alloc * sizeof(agent_t *));
        e->n_agents_alloc = n_agents_alloc;
        return e;
}

void env_free(env_t *e) {
        if (e == NULL)
                return;
        free(e->agents);
        free(e);
}

void env_add_agent(env_t *e, agent_t *a) {
        /* Allocate more memory if necessary */
        if (e->n_agents >= e->n_agents_alloc) {
                if (e->n_agents_alloc <= 0)
                        e->n_agents_alloc = 1;

                e->n_agents_alloc *= 2;
                e->agents = realloc(
                        e->agents,
                        e->n_agents_alloc * sizeof(agent_t *)
                );
        }

        e->agents[e->n_agents] = a;
        e->n_agents += 1;

        a->environment = e;
}


/* Calculate derivative at a given point, multiplied with dt */
static void derivative(
        state_delta_t *target,
        double T, double I, double V, double A,
        double V_delay, double A_delay,
        double W, double Z,
        double dt,
        params_t *params
) {
        double v_external = params->eta * Z + W;
        target->dT = (
                params->b
                - params->k * V * T / (1 + params->alpha * A)
                - params->d * T
        ) * dt / params->epsilon;
        target->dI = (
                params->k * V * T / (1 + params->alpha * A)
                - params->q * I
        ) * dt / params->epsilon;
        target->dV = (
                ((v_external > params->v_infect)? v_external : 0)
                + params->p * I
                - params->c * V
                - params->c_A * A * V
        ) * dt / params->epsilon;
        target->dA = (
                params->b_A
                - params->d_A * A
                + params->k_A * A_delay * V_delay
        ) * dt / params->epsilon;
}


void agent_step_calculate(agent_t *a, double dt) {
        state_delta_t hk1, hk2, hk3, hk4;
        double V_approx = (
                a->history[a->h_first].V
                + a->history[(a->h_first + 1) % (a->n_history + 1)].V
        ) / 2.0;
        double A_approx = (
                a->history[a->h_first].A
                + a->history[(a->h_first + 1) % (a->n_history + 1)].A
        ) / 2.0;
        derivative(
                &hk1,
                a->state->T,
                a->state->I,
                a->state->V,
                a->state->A,
                a->history[a->h_first].V,
                a->history[a->h_first].A,
                a->state->W,
                a->environment->Z,
                dt,
                a->params
        );
        derivative(
                &hk2,
                a->state->T     + hk1.dT / 2,
                a->state->I     + hk1.dI / 2,
                a->state->V     + hk1.dV / 2,
                a->state->A     + hk1.dA / 2,
                V_approx,
                A_approx,
                a->state->W,
                a->environment->Z,
                dt,
                a->params
        );
        derivative(
                &hk3,
                a->state->T     + hk2.dT / 2,
                a->state->I     + hk2.dI / 2,
                a->state->V     + hk2.dV / 2,
                a->state->A     + hk2.dA / 2,
                V_approx,
                A_approx,
                a->state->W,
                a->environment->Z,
                dt,
                a->params
        );
        derivative(
                &hk4,
                a->state->T     + hk3.dT,
                a->state->I     + hk3.dI,
                a->state->V     + hk3.dV,
                a->state->A     + hk3.dA,
                a->history[(a->h_first + 1) % (a->n_history + 1)].V,
                a->history[(a->h_first + 1) % (a->n_history + 1)].A,
                a->state->W,
                a->environment->Z,
                dt,
                a->params
        );


        a->history[a->h_next].T = a->state->T + (hk1.dT + 2 * hk2.dT + 2 * hk3.dT + hk4.dT) / 6;
        a->history[a->h_next].I = a->state->I + (hk1.dI + 2 * hk2.dI + 2 * hk3.dI + hk4.dI) / 6;
        a->history[a->h_next].V = a->state->V + (hk1.dV + 2 * hk2.dV + 2 * hk3.dV + hk4.dV) / 6;
        a->history[a->h_next].A = a->state->A + (hk1.dA + 2 * hk2.dA + 2 * hk3.dA + hk4.dA) / 6;
        a->history[a->h_next].W = a->state->W;

        double c = random_exponential(a->params->lambdai);
        a->history[a->h_next].V += -c * dt / a->params->epsilon;

        if (a->history[a->h_next].V < 0.0)
                a->history[a->h_next].V = 0.0;
}

void agent_step(agent_t *a) {
        a->h_previous = a->h_current;
        a->h_current  = a->h_next;
        a->h_next     = a->h_first;
        a->h_first    = (a->h_first + 1) % (a->n_history + 1);
        a->state      = &(a->history[a->h_current]);
}

void env_step(env_t *e, double dt) {
        e->Z -= e->delta * e->Z * dt;
        for (int i = 0; i < e->n_agents; i++)
                e->Z += e->xi * e->agents[i]->state->V * dt;
}
