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
