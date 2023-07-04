#!/usr/bin/env Rscript

library(tibble)
library(tidyr)
library(dplyr)
library(ggplot2)
library(RColorBrewer)
library(zoo)

df <- tibble(t = integer(), I = double(), run = integer(), p = double())

for (p in c(2, 4, 6, 8)) {
        for (i in 1:100) {
                filename <- sprintf("output/p%d/countdata_%03d.dat", p, i)
                d <- read.csv(filename)
                df <- df %>% add_row(
                        t = d[, 1],
                        I = d[, 3],
                        run = i,
                        p = p / 1000
                )
        }
}

df <- mutate(df, p = as.factor(p))

durations <- df %>%
                group_by(run, p) %>%
                mutate(ma = rollapply(I, 30, mean, align = 'center', fill = 0.0)) %>%
                mutate(dma = ma - lag(ma)) %>%
                mutate(flip = dma * lag(dma) < 0) %>%
                summarise(flips = sum(flip, na.rm = TRUE), duration = sum(I > 0))

averages <- df %>%
                group_by(p, t) %>%
                summarise(mean_I = mean(I))

pdf(file = "multirun.pdf", width = 16, height = 8)

ggplot() +
        geom_line(
                data = df,
                aes(x = t, y = I, group = interaction(run, p), color = p),
                linewidth = 0.2,
                alpha = 0.15
        ) +
        geom_line(
                data = averages,
                aes(x = t, y = mean_I, group = p, color = p),
                linewidth = 1.5
        ) +
        scale_color_brewer(palette = "Set1") +
        theme_minimal()

ggplot() +
        geom_line(
                data = df,
                aes(x = t, y = I, group = interaction(run, p), color = p),
                alpha = 0.25
        ) +
        geom_line(
                data = averages,
                aes(x = t, y = mean_I, group = p, color = p),
                linewidth = 1.5
        ) +
        scale_color_brewer(palette = "Set1") +
        facet_grid(p ~ ., scales = "free")

ggplot() +
        geom_histogram(
                data = durations,
                aes(x = duration, fill = p)
        ) +
        scale_fill_brewer(palette = "Set1") +
        facet_wrap(vars(p))

dev.off()
