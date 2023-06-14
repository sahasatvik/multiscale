# A multiscale model of viral infection

## Usage

Tweak parameters in `parameters.h`.

```sh
make
./gendata       # Generate random data (environments, contacts)
./model         # Run model, save data
./plot.p        # Visualize
```

To visualize the progression of a single agent, tweak parameters in
`oneagent.c`.
```sh
./oneagent > output/oneagent.dat
./oneagent.p
```

_Note_: Plotting currently requires `gnuplot`.

## Description

Each agent $i$ has $T_i$ target cells, $`T^*_i`$ infected cells, $V_i$ viral copies, and $A_i$ antibodies. Each agent belongs to some environment $j$, with $Z_j$ viral copies. These evolve as
```math
\begin{align*}
\frac{dT_i}{dt}   &= \frac{1}{\epsilon}\left( b - \frac{\kappa}{1 + \alpha A_i} V_iT_i - d\, T_i \right), \\
\frac{dT^*_i}{dt} &= \frac{1}{\epsilon}\left( \frac{\kappa}{1 + \alpha A_i} V_iT_i - qT^*_i \right), \\
\frac{dV_i}{dt}   &= \frac{1}{\epsilon}\left( h_i(\eta Z_j + W_i) + pT^*_i - cV_i - c_A A_iV_i \right), \\
\frac{dA_i}{dt}   &= \frac{1}{\epsilon}\left( b_A + \kappa_A A_i(t - \tau) V_i(t - \tau) - d_A\, A_i \right).
\end{align*}
```

Here, $h(x) = x\, \mathbb{1}(x > x_0)$ for some threshold $x_0$.

If $\mathscr{I}_j$ are the indices of the agents in the $j$-th environment, then $Z_j$ gets updated over a small time interval $\Delta t$ as
```math
\frac{\Delta Z_j}{\Delta t} = \sum_{i \in \mathscr{I}_j} \xi_i(t) V_i - \delta Z_j.
```

Here, the shedding coefficients $\xi_i$ may depend on the stage of infection of agent $i$.

_(All parameters can vary across individuals/environments; subscripts $`i, j`$ have been omitted for clarity.)_

Agents form the nodes of a contact network, where edges have weights $s_{ii'}$. Once per day, the variable $W_i$ for an agent $i$ is reset to zero and updated as follows: for each neighbour $i'$ of $i$, a viral load $\zeta_j V_{i'}$ is added to $W_i$ with probability $p_\text{inf} s_{ii'}$.

## TODO
- [ ] Implement status ($S, I, R$) and shedding dynamics based on actual viral load curves
- [x] Implement immune response
- [ ] Implement mortality
- [ ] Create variation among agents, environments
- [ ] Better contact networks

## References

1. Wang X, Wang S, Wang J, Rong L. [A Multiscale Model of COVID-19 Dynamics](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9360740/). Bull Math Biol. 2022 Aug 9;84(9):99. doi: 10.1007/s11538-022-01058-8.

2. Ciupe SM, Heffernan JM. [In-host modeling](https://pubmed.ncbi.nlm.nih.gov/29928736/). Infect Dis Model. 2017 Apr 29;2(2):188-202. doi: 10.1016/j.idm.2017.04.002.
