#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(ggplot2)

df <- read.csv("output/reinfection.dat", sep = "", header = FALSE)
colnames(df) <- c("Delay", "Reinfection Probability")
df <- tibble(df)

pdf(file = "figures/reinfection.pdf", width = 8, height = 5)

ggplot(df) +
        geom_line(aes(x = Delay, y = `Reinfection Probability`))

dev.off()
