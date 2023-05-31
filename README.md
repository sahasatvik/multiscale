# A multiscale model of viral infection

## Usage

Tweak parameters in `parameters.h`.

```
make
./model > model.dat
./plot.p
```

_Note_: Plotting requires `gnuplot`.

## Description

Each agent $i$ has $T_i$ target cells, $`T^*_i`$ infected cells, $V_i$ viral copies. Each agent belongs to some environment $j$, with $Z_j$ viral copies. These evolve as
```math
\begin{align*}
\frac{dT_i}{dt} &= \frac{1}{\epsilon}\left(b - \kappa V_iT_i - d\, T_i\right), \\
\frac{dT^*_i}{dt} &= \frac{1}{\epsilon}\left(\kappa V_iT_i - qT^*_i\right), \\
\frac{dV_i}{dt} &= \frac{1}{\epsilon}\left(\eta Z_j + pT^* - cV\right).
\end{align*}
```

If $E_j$ individuals in environment $j$ are pre-symptomatic infected and $I_j$ are symptomatic infected, then $Z_j$ gets updated over a small time interval $\Delta t$ as
```math
\frac{\Delta Z_j}{\Delta t} = \xi_E E_j + \xi_I I_j - \delta_j Z_j.
```

_(All parameters can vary across individuals/environments; subscripts $`i, j`$ have been omitted for clarity.)_

Agents form the nodes of a contact network, where edges have weights $s_{ii'}$. Once per day, an agent $i$ can receive a viral load $V_\text{inf}$ from an infected neighbour $i'$ with probability $p_\text{inf} s_{ii'}$.

## TODO
- [ ] Implement status ($S, I, R$) and shedding dynamics based on actual viral load curves
- [ ] Implement immune response
- [ ] Implement mortality
- [ ] Create variation among agents, environments
- [ ] Better contact networks

## References

1. Wang X, Wang S, Wang J, Rong L. [A Multiscale Model of COVID-19 Dynamics](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9360740/). Bull Math Biol. 2022 Aug 9;84(9):99. doi: 10.1007/s11538-022-01058-8.
