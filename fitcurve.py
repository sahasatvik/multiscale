#!/usr/bin/env python

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.integrate import odeint


def curve(t, y0, beta, gamma, mu, alpha):
    # g = lambda y, t: f(y, t, beta_I0 = beta_I0, C_V = C_V, epsilon = epsilon)
    # return ddeint(g, y0, t)[:, 1]
    def g(y, t):
        S, I, R = y
        N = S + I + R
        return -beta * S * I / (N * (1 + alpha * I)) + mu * R, \
                beta * S * I / (N * (1 + alpha * I)) - gamma * I, \
                gamma * I - mu * R
    return odeint(g, y0, t)[:, 1]

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

    return np.array(ys)


def load_curves_aligned(p, trials, t_cut = 100):
    ys = load_curves(p, trials, t_cut = t_cut)
    tmax = ys.shape[1]

    # Align peaks
    imax = ys[:, :t_cut].argmax(axis = 1)
    tmax += imax.max() - imax.min()

    yss = []
    for i in range(len(ys)):
        yss.append(np.pad(ys[i, :], (imax.max() - imax[i], imax[i] - imax.min()), mode = 'constant'))

    # for i in range(len(yss)):
    #     plt.plot(t, yss[i])
    # plt.plot(t, y)
    # plt.show()

    return np.array(yss)


def load_mean_curve(p, trials, t_cut = 100, align = False):
    if align:
        ys = load_curves_aligned(p, trials, t_cut = 100)
    else:
        ys = load_curves(p, trials, t_cut = 100)

    tmax = ys.shape[1]

    t = np.arange(0, tmax, 1.0)
    y = np.mean(ys, axis = 0)

    y = y[t < t_cut]
    t = t[t < t_cut]

    return y, t


def show_fit(p, t_cut = 100, fig = 1, trials = range(1, 101), align = False):
    y, t = load_mean_curve(p, t_cut = t_cut, trials = trials, align = align)

    def f(t, beta, gamma, mu, alpha, I0):
        t_ = np.arange(0, t.max() + 1, 1e-2)
        y0 = (1e4 - I0, I0, 0.0)
        sol = curve(t_, y0, beta, gamma, mu, alpha)
        y = sol[t_ <= t.max()][::100]
        y[y == np.inf] = 0
        y[y == -np.inf] = 0
        return np.nan_to_num(y)

    p0 = (3.5e-1, 1 / 8, 0, 0, 1)

    popt, pcov, infodict, mesg, ier = curve_fit(f, t, y, p0 = p0, full_output = True, bounds = (0, [1, 1, 1, 1, 1e3]))
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


cutoffs = {
    # 2: 100,
    3: 130,
    4: 100,
    5: 100,
    6: 150,
    8:  60
}

coeffs = dict()

fig = 1
for p, t_cut in cutoffs.items():
    coeffs[p] = show_fit(p, t_cut = t_cut, fig = fig, align = False)
    fig += 1

beta = {p: coeff[0] for p, coeff in coeffs.items()}
# I0   = {p: coeff[3] for p, coeff in coeffs.items()}
print(beta)
# print(I0)

plt.show()
