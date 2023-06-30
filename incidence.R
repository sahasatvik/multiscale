#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(ggplot2)

countdata <- read.csv("output/countdata.dat")
colnames(countdata) <- c("t", "S", "I", "R", "newI")
countdata <- tibble(countdata)
print(countdata)

ggplot(countdata[1:50, ]) +
        geom_point(aes(x = t, y = newI / (S * I)), color = "red")
