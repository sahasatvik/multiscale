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

Visualize the degree distribution of the generated network using
`./degree_distribution.R`, and see `figures/degree_distribution.pdf`.

### Multiple runs

The command `./multirun.sh START END` will run the model multiple times and
produce output files indexed by integers from `START` to `END`. For example,
`./multirun.sh 1 10` will produce files `countdata_01.dat` to
`countdata_10.dat` in the `output` folder.

Here, we have adjusted the parameter `P_INFECT` (probability of infection on
contact) as $X \times 10^{-3}$ for $X \in \\{2, 3, 4, 5, 6, 8\\}$. For each
of these, tweak `parameters.h` accordingly, execute `multirun.sh START
END`, and move the output files into folders `output/pX`. Generate figures
by executing `./multirun.R` and `./peaks.R`, tweaking the variables
`trials` and `trial_p` to reflect the number of trials and the values of
$X$.

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
