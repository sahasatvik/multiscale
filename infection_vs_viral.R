#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(ggplot2)

trials <- 1:200
trial_p <- c(2, 3, 4, 5, 6, 8)

for (p in trial_p) {

        pdf(file = sprintf("figures/IvsV_%d.pdf", p), width = 8, height = 6)

        for (i in trials) {

                print(c(p, i))

                countdata <- read.csv(sprintf("output/p%d/countdata_%03d.dat", p, i))
                colnames(countdata) <- c("t", "S", "I", "R", "newI")
                countdata <- tibble(countdata)

                avgdata <- read.csv(sprintf("output/p%d/averagedata_%03d.dat", p, i))
                colnames(avgdata) <- c("t", "T", "T*", "V", "A")
                avgdata <- tibble(avgdata)

                df <- inner_join(countdata, avgdata, by = 't')
                df <- mutate(df, beta = newI / (S * I))

                fit <- lm(I ~ V + 0, data = df)
                print(summary(fit))

                pl <- ggplot(df) +
                        geom_line(aes(x = t, y = I, color = "Infections")) +
                        geom_line(aes(x = t, y = V, color = "Viral load"))

                print(pl)
        }

        dev.off()
}
