#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(ggplot2)

trials <- 1:200

for (i in trials) {
        countdata <- read.csv(sprintf("output/p4/countdata_%03d.dat", i))
        colnames(countdata) <- c("t", "S", "I", "R", "newI")
        countdata <- tibble(countdata)
        # print(countdata)

        avgdata <- read.csv(sprintf("output/p4/averagedata_%03d.dat", i))
        colnames(avgdata) <- c("t", "T", "T*", "V", "A")
        avgdata <- tibble(avgdata)
        # print(avgdata)

        df <- inner_join(countdata, avgdata, by = 't')
        df <- mutate(df, beta = newI / (S * I))
        # print(df)

        # ggplot(df) +
        #         geom_point(aes(x = V, y = I, color = beta))

        p <- ggplot(df) +
                geom_line(aes(x = t, y = I, color = "Infections")) +
                geom_line(aes(x = t, y = V, color = "Viral load"))

        print(p)
}
