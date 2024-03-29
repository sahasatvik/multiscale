# A multiscale model of viral infection

## Usage

Tweak parameters in `parameters.h`.

```sh
make
./gendata       # Generate random data (environments, contacts)
./model         # Run model, save data
./plot.p        # Visualize
```

Model outputs are present in the `output` folder as `.dat` files, in `csv`
format.

_Note_: Plotting currently requires `gnuplot`.

Run `./model -h` for help on more options.

Visualize the degree distribution of the generated network using
`./degree_distribution.R`, and see `figures/degree_distribution.pdf`.

### Multiple runs

Execute `./multirun.sh 1 200` to recreate model outputs expected by the
scripts `multirun.R` and `peaks.R` which generate figures.

The command `./multirun.sh START END` will run the model multiple times for
different values for `P_INFECT`, and produce output files indexed by integers
from `START` to `END`. For example, `./multirun.sh 1 10` will produce files
`countdata_01.dat` to `countdata_10.dat` in the `output/p2` to `output/p8`
folders.

Here, we have adjusted the parameter `P_INFECT` (probability of infection on
contact) as $X \times 10^{-3}$ for $X \in \\{2, 3, 4, 5, 6, 8\\}$.

Change the statement `P_INFECT=(2 3 4 5 6 8)` in `multirun.sh` to use other
values for $X$; the corresponding output files will be placed in the
`output/pX` folder.

Tweak the variables `trials` and `trial_p` in `multirun.R` and `peaks.R` to
reflect the number of trials and the values of $X$.

### One agent

Tweak parameters in `oneagent.c`, then `make` and run
`./oneagent.c > output/oneagent.dat`. Visualize using `oneagent.p`,
or `oneagent.R` which produces `figures/oneagent.pdf`.


## Description

The model specification can be found in `report/model.pdf`.

## TODO
- [ ] Implement status ($S, I, R$) and shedding dynamics based on actual viral load curves
- [x] Implement immune response
- [ ] Implement mortality
- [ ] Create variation among agents, environments
- [ ] Better contact networks

## References

1. Wang X, Wang S, Wang J, Rong L. [A Multiscale Model of COVID-19 Dynamics](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9360740/). Bull Math Biol. 2022 Aug 9;84(9):99. doi: 10.1007/s11538-022-01058-8.

2. Ciupe SM, Heffernan JM. [In-host modeling](https://pubmed.ncbi.nlm.nih.gov/29928736/). Infect Dis Model. 2017 Apr 29;2(2):188-202. doi: 10.1016/j.idm.2017.04.002.
