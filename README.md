# MNXB11 project - Group 6

Run on COSMOS inside the course container (ROOT 6.28/10):

    /projects/hep/fs11/share/mnxb11/bin/startmnxb11container.sh bash

All commands are run from the repository root.

## Data cleaning

    bash scripts/clean_data.sh

Extracts the datasets into data/raw and writes the cleaned files to data/cleaned.

## Cold days in Uppsala

    root -l -b -q macros/coldDays.C

Counts the days per year with a daily mean below 0 C, fits a straight line and draws a 31-year moving average. The plot is saved in figures/coldDays.png.

## Warm  days in Uppsala

The ROOT macro macros/WarmDays.C analyses the annual frequency of warm days (urban-corrected mean temperature ≥20°C) in Uppsala between 1722 and 2022, using the filtered dataset in data/cleaned/. Years with fewer than 330 observations are excluded, and a trailing 30-year moving average is calculated using at least 27 valid years per window.

 To run the macro, ensure ROOT is available and execute: 

    root -l -b -q macros/WarmDays.C

The macro generates warm_days.png and warm_days.pdf, displaying the annual frequencies and moving average.

## Mean Annual Temperature in Lund

The ROOT macro macros/MeanAnnualTempLundClean.C calculates and plots the historical mean annual temperature in Lund from 1780 to 2022 using the cleaned dataset (data/cleaned/lund_tm_1780-2022.dat).

To prevent missing seasonal data from skewing annual averages, years with fewer than 330 valid daily observations (1957–1960) are excluded. Rather than using a histogram with filled bins, the annual means are represented via a TGraph (ALP style) to cleanly handle missing years without introducing zero-value artifacts.

A 30-year centered moving average is calculated across windows containing at least 21 valid annual data points. Linear polynomial fits are applied over three historical era to quantify long-term warming rates per decade:

1795–1850: Pre-industrial baseline trend

1850–1970: Early industrialization trend

1970–2007: Accelerated modern warming trend

To run the macro in batch mode, ensure ROOT is available and execute:

    root -l -b -q macros/MeanAnnualTempLundClean.C

The macro outputs decade slope statistics to stdout and generates the plot with a multi-fit legend.
