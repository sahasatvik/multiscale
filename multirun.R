#!/usr/bin/env Rscript

library(tibble)
library(tidyr)
library(dplyr)
library(ggplot2)
library(RColorBrewer)
library(zoo)

df <- tibble(t = integer(), I = double(), run = integer(), p = double())

for (p in c(2, 3, 4, 5, 6, 8)) {
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
df <- df %>%
                filter(I > 0) %>%
                group_by(p) %>%
                complete(run, t) %>%
                ungroup() %>%
                replace_na(list(I = 0))

durations <- df %>%
                group_by(run, p) %>%
                mutate(ma = rollapply(I, 30, mean, align = 'center', fill = 0.0)) %>%
                mutate(dma = ma - lag(ma)) %>%
                mutate(flip = dma * lag(dma) < 0) %>%
                summarise(
                        flips = sum(flip, na.rm = TRUE),
                        duration = sum(I > 0),
                )

averages <- df %>%
                group_by(p, t) %>%
                summarise(mean_I = mean(I))

peaks <- df %>%
                group_by(p, run) %>%
                mutate(KSmooth = ksmooth(t, I, "normal", bandwidth = 15, x.points = t)$y) %>%
                mutate(
                        diff1 = KSmooth - lag(KSmooth),
                        diff2 = sign(diff1) - lag(sign(diff1)),
                        peak = diff2 == -2
                )

peaks_n <- peaks %>%
                summarise(peaks = sum(peak & (I > 1000)))

peak_to_peak <- peaks %>%
                filter(peak & (I > 1000)) %>%
                # filter(sum(peak) == 2) %>%
                mutate(peak_to_peak = t - lag(t), n = row_number()) %>%
                drop_na()


heights <- df %>%
                group_by(p, run) %>%
                summarise(height = max(I))


pdf(file = "multirun.pdf", width = 16, height = 10)

sturges <- function(x) {
        n = length(x)
        ceiling(log(n, 2)) + 1
}

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
        ggtitle("Infection curves grouped by p, with averages") +
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
        ggtitle("Infection curves separated by p, with averages") +
        scale_color_brewer(palette = "Set1") +
        facet_grid(p ~ ., scales = "free")

ggplot() +
        geom_histogram(
                data = durations,
                aes(x = duration, fill = p),
                bins = sturges(durations$duration)
        ) +
        ggtitle("Epidemic durations grouped by p") +
        scale_fill_brewer(palette = "Set1") +
        facet_wrap(vars(p))

peaks_n %>% count(peaks) %>% print(n = Inf)

ggplot() +
        geom_bar(
                data = peaks_n %>%
                        mutate(peaks = factor(peaks, levels = as.character(0:10))) %>%
                        count(peaks),
                aes(x = p, y = n, fill = peaks),
                width = 1,
                stat = "identity"
        ) +
        ggtitle("Number of peaks grouped by p") +
        # coord_polar("y", start = 0) +
        scale_fill_brewer(palette = "Blues") +
        theme_minimal()
        # facet_wrap(vars(p))

ggplot() +
        geom_boxplot(
                data = peak_to_peak,
                aes(x = p, y = peak_to_peak, color = as.factor(n - 1))
        ) +
        geom_jitter(
                data = peak_to_peak,
                aes(x = p, y = peak_to_peak, color = as.factor(n - 1)),
                position = position_dodge(0.75)
        ) +
        ggtitle("Peak to peak distances") +
        scale_fill_brewer(palette = "Set1") +
        scale_color_brewer(palette = "Set1")

ggplot() +
        geom_jitter(
                data = heights,
                aes(x = p, y = height, color = p),
        ) +
        ggtitle("Epidemic heights grouped by p") +
        scale_color_brewer(palette = "Set1")

dev.off()
