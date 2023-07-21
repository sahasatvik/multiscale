#!/usr/bin/env Rscript

library(stringr)

pdf("figures/degree_distribution.pdf")

dat <- read.csv2("data/network.dat")
x   <- as.numeric(str_extract(dat[, 1], "[0-9]+"))
f   <- ecdf(x)

plot(sort(x), 1 - f(sort(x)), log = 'xy', xlab = 'degree', ylab = 'frequency')
title("Complementary Cumulative Distribution Function")

dev.off()
