#!/usr/bin/env python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.integrate import odeint

from numba import njit

@njit
def g(y, t, beta, gamma, mu):
    S, I, R = y
    N = S + I + R
    return -beta * S * I / N + mu * R, \
            beta * S * I / N - gamma * I, \
            gamma * I - mu * R
def curve(t, y0, beta, gamma, mu):
    return odeint(g, y0, t, args = (beta, gamma, mu))[:, 1]

def load_curve(p, n):
    filename = "output/p%d/countdata_%03d.dat" % (p, n)
    df = pd.read_csv(filename)
    df = df.rename(columns = str.strip)
    return df[["Time", "Infected"]].to_numpy()

def load_curves(p, trials, t_cut = 100):
    data = list()
    tmax = 0
    for n in trials:
        c = load_curve(p, n)

        # Discard extinctions
        if c[:t_cut, 1].max() < 1e3:
            continue
        data.append(c)
        tmax = max(tmax, c.shape[0])

    ys = list()
    for d in data:
        l = d.shape[0]
        ys.append(np.pad(d[:, 1], (0, tmax - l), mode = 'constant'))

    return np.array(ys)[:, :t_cut]



def show_fit(p, t_cut = 100, fig = 1, trials = range(1, 101)):
    ys = load_curves(p, t_cut = t_cut, trials = trials)
    tmax = ys.shape[1]

    def f(t, beta, gamma, mu, *I0s):
        sols = []
        t_ = np.arange(0, tmax + 1, 1e-2)
        for I0 in I0s:
            y0 = (1e4 - I0, I0, 0.0)
            sol = curve(t_, y0, beta, gamma, mu)
            sol = sol[:100 * tmax:100]
            sols.append(sol)

        return np.hstack(sols)

    p0 = np.hstack([
        [3.5e-1, 1 / 8, 0],
        np.ones(len(ys))
    ])
    bounds = (
        np.hstack([
            [0, 0, 0],
            np.zeros(len(ys))
        ]),
        np.hstack([
            [1, 1, 1],
            1e3 * np.ones(len(ys))
        ])
    )

    y = np.hstack([ysi for ysi in ys])
    t = np.arange(0, y.shape[0], 1.0)

    print(ys.shape, y.shape, f(t, *p0).shape)

    popt, pcov, infodict, mesg, ier = curve_fit(f, t, y, p0 = p0, full_output = True, bounds = bounds)
    print(p, popt)

    # t_ = np.arange(0, tmax, 1.0)
    # y_ = ys.mean(axis = 0)

    beta, gamma, mu = popt[:3]
    I0s = popt[3:]

    plt.figure(fig)

    t_ = np.arange(0, tmax, 1.0)
    rows = int(np.ceil(np.sqrt(len(ys))))
    cols = int(np.ceil(len(ys) / rows))
    for i in range(len(ys)):
        y_ = ys[i]
        y0 = (1e4 - I0s[i], I0s[i], 0.0)
        # plt.subplot(len(ys), 1, i)
        ax = plt.subplot2grid((rows, cols), (i % rows, i // rows))
        ax.plot(t_, y_, label = "Simulated")
        ax.plot(t_, curve(t_, y0, beta, gamma, mu), label = "Fitted")
        # plt.plot(t, f(t, *p0), label = "Approximated")

    plt.suptitle("p = %f" % (p / 1000))
    # plt.legend()

    # plt.subplot(1, 2, 2)
    # plt.plot(t, infodict["fvec"], label = "Residuals")
    # plt.legend()

    return popt


cutoffs = {
    # 2: 100,
    3: 130,
    4: 130,
    5: 100,
    6: 100,
    8:  60
}

coeffs = dict()

fig = 1
for p, t_cut in cutoffs.items():
    coeffs[p] = show_fit(p, t_cut = t_cut, fig = fig, trials = range(1, 100))
    fig += 1

beta = {p: coeff[0] for p, coeff in coeffs.items()}
# I0   = {p: coeff[3] for p, coeff in coeffs.items()}
print(beta)
# print(I0)

plt.show()
