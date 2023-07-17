#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
from ddeint import ddeint
from scipy.integrate import odeint

def beta_I(V, I, beta_I0 = 1e-4, C_V = 1e-6, C_I = 0):
    return (beta_I0 + C_V * V) / (1 + C_I * I)
    # return 1e-8 * V
    # return 1e-4 / (1 + 1e-3 * I)
    # return (1e-6 + 1e-3 * V) / (1 + 1e2 * I)

def beta_Z(Z):
    return 0.0
    # return 1e-6 / (1 + 1e3 * Z)

def f(
    y, t,
    tau = 3e0,
    T0 = 1e4, delta = 1e-2, kappa = 1e-5,
    q = 2e0, p = 2e1, c = 1e-1,
    A0 = 1e0, delta_A = 2e-2, kappa_A = 1e-4,
    alpha = 5e-1, c_A = 5e-2,
    epsilon = 2e-1,
    gamma = 1e-1, mu = 1e-2,
    xi = 1.0, delta_Z = 1.0, eta = 1e-6,
    beta_I0 = 0.0, C_V = 1e-8, C_I = 0
):
    S, I, R, Z, T, U, V, A = y(t)
    _, _, _, _, _, _, V_, A_ = y(t - tau)
    return np.array([
        -beta_I(V, I, beta_I0 = beta_I0, C_V = C_V, C_I = C_I) * S * I - beta_Z(Z) * S * Z + mu * R, \
        beta_I(V, I, beta_I0 = beta_I0, C_V = C_V, C_I = C_I) * S * I + beta_Z(Z) * S * Z - gamma * I, \
        gamma * I - mu * R, \
        xi * V - delta_Z * Z, \
        (delta * (T0 - T) - kappa * T * V / (1 + alpha * A)) / epsilon, \
        (kappa * T * V / (1 + alpha * A) - q * U) / epsilon, \
        (eta * I + p * U - (c + c_A * A) * V) / epsilon, \
        (delta_A * (A0 - A) + kappa_A * A_ * V_) / epsilon \
    ])


def curve(t, y0, beta, gamma, mu):
    # g = lambda y, t: f(y, t, beta_I0 = beta_I0, C_V = C_V, epsilon = epsilon)
    # return ddeint(g, y0, t)[:, 1]
    def g(y, t):
        S, I, R = y
        N = S + I + R
        return -beta * S * I / N + mu * R, beta * S * I / N - gamma * I, gamma * I - mu * R
    return odeint(g, y0, t)[:, 1]


if __name__ == '__main__':
    t = np.arange(0, 100, 1e-2)
    y0 = lambda t: (1e4, 1.0, 0, 0, 1e4, 0, 1e-1, 1.0)
    sol = ddeint(f, y0, t)

    plt.figure(1)
    plt.yscale("log")
    plt.plot(t, sol[:, 3:], label = ["Z", "T", "T*", "V", "A"])
    plt.ylim([1e-4, 1e5])
    plt.legend()

    plt.figure(2)
    plt.plot(t, sol[:, :3], label = ["S", "I", "R"])
    plt.legend()
    plt.show()
