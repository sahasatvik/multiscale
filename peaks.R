#!/usr/bin/env Rscript

library(tibble)
library(dplyr)
library(tidyr)
library(ggplot2)
library(zoo)

disp <- function(p, n, ma = 25, mm = 5, thresh = 1000) {
        df <- read.csv(sprintf("output/p%d/countdata_%03d.dat", p, n))
        df <- df %>%
                mutate(KSmooth = ksmooth(Time, Infected, "normal", bandwidth = 15, x.points = Time)$y) %>%
                mutate(Moving.Mean = rollapply(Infected, ma, mean, align = 'center', fill = 0.0)) %>%
                mutate(Moving.Median = rollapply(Infected, mm, median, align = 'center', fill = 0.0))
                # mutate(Moving.Max = rollapply(Infected, mm, max, align = 'center', fill = 0.0))
        peaks <- df %>%
                        mutate(
                                diff1 = KSmooth - lag(KSmooth),
                                diff2 = sign(diff1) - lag(sign(diff1)),
                                peak = diff2 == -2
                        ) %>%
                        filter(peak & (Infected > thresh)) %>%
                        # filter(peak & (Infected > 0.5 * max(Infected))) %>%
                        select(Time, Infected)
        print(sprintf("%d %d", i, nrow(peaks)))
        df <- df %>%
                select(!Moving.Mean) %>%
                select(!Moving.Median) %>%
                # select(!Susceptible) %>%
                pivot_longer(!Time, values_to = "Count", names_to = "Type")
        ggplot(df) +
                geom_line(aes(x = Time, y = Count, group = Type, color = Type)) +
                geom_point(data = peaks, aes(x = Time, y = Infected), color = "red")
}


for (n in c(2, 3, 4, 5, 6, 8)) {
        pdf(file = sprintf("peaks_%d.pdf", n), width = 8, height = 6)
        print(n / 1000)
        for (i in 1:100) {
                print(disp(n, i))
        }
        dev.off()
}
