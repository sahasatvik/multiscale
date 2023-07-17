#!/usr/bin/env python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.integrate import odeint


def curve(t, y0, beta, gamma, mu):
    # g = lambda y, t: f(y, t, beta_I0 = beta_I0, C_V = C_V, epsilon = epsilon)
    # return ddeint(g, y0, t)[:, 1]
    def g(y, t):
        S, I, R = y
        N = S + I + R
        return -beta * S * I / N + mu * R, beta * S * I / N - gamma * I, gamma * I - mu * R
    return odeint(g, y0, t)[:, 1]

def load_curve(p, n):
    filename = "output/p%d/countdata_%03d.dat" % (p, n)
    df = pd.read_csv(filename)
    df = df.rename(columns = str.strip)
    return df[["Time", "Infected"]].to_numpy()

def load_mean_curve(p, trials = 100, t_cut = 100):
    data = list()
    tmax = 0
    for n in range(trials):
        data.append(load_curve(p, n + 1))
        tmax = max(tmax, data[n].shape[0])

    ys = list()
    for n in range(trials):
        l = data[n].shape[0]
        ys.append(np.pad(data[n][:, 1], (0, tmax - l), mode = 'constant'))

    t = np.arange(0, tmax, 1.0)
    y = np.mean(ys, axis = 0)

    y = y[t < t_cut]
    t = t[t < t_cut]

    return y, t


def show_fit(p, t_cut = 100, fig = 1):
    y, t = load_mean_curve(p, t_cut = t_cut)

    def f(t, beta, gamma, mu, I0):
        t_ = np.arange(0, t.max() + 1, 1e-2)
        y0 = (1e4 - I0, I0, 0.0)
        sol = curve(t_, y0, beta, gamma, mu)
        y = sol[t_ <= t.max()][::100]
        y[y == np.inf] = 0
        y[y == -np.inf] = 0
        return np.nan_to_num(y)

    p0 = (3.5e-1, 1 / 8, 0, 40)

    popt, pcov, infodict, mesg, ier = curve_fit(f, t, y, p0 = p0, full_output = True, bounds = (0, [1, 1, 1, 1e3]))
    print(p, popt)

    plt.figure(fig)

    plt.subplot(1, 2, 1)
    plt.plot(t, y, label = "Simulated")
    plt.plot(t, f(t, *popt), label = "Fitted")
    # plt.plot(t, f(t, *p0), label = "Approximated")
    plt.title("p = %f" % (p / 1000))
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.plot(t, infodict["fvec"], label = "Residuals")
    plt.legend()

    return popt

# p0 = (1e-5, 8.0e-9, 3.0e-1)

# popt, pcov, infodict, mesg, ier = curve_fit(f, t, y, bounds = ([0, 0, 1e-3], [1e-3, 1e-7, 10]), full_output = True, nan_policy = 'omit', check_finite = False)
# print(popt, pcov, infodict, mesg, ier)


# print(popt)

cutoffs = {
    2: 100,
    3: 100,
    4: 100,
    5: 100,
    6: 100,
    8: 100
}

coeffs = dict()

fig = 1
for p, t_cut in cutoffs.items():
    coeffs[p] = show_fit(p, t_cut = t_cut, fig = fig)
    fig += 1

beta = {p: coeff[0] for p, coeff in coeffs.items()}
# I0   = {p: coeff[3] for p, coeff in coeffs.items()}
print(beta)
# print(I0)

plt.show()
