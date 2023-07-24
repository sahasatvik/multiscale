#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(ggplot2)

df <- read.csv("output/oneagent.dat", sep = "", header = FALSE)
colnames(df) <- c("t", "T", "T*", "V", "A")
df <- tibble(df)

pdf(file = "figures/oneagent.pdf", width = 8, height = 5)

ggplot(df) +
        geom_line(aes(x = t, y = T,    color = "Target cells")) +
        geom_line(aes(x = t, y = `T*`, color = "Infected cells")) +
        geom_line(aes(x = t, y = V,    color = "Viral load")) +
        geom_line(aes(x = t, y = A,    color = "Antibodies/Immunity")) +
        ylab("Concentration") +
        scale_y_log10()

dev.off()
