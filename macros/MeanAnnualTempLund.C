#include <iostream>
#include <fstream>
#include <map>
#include <TH1D.h>
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>

void MeanAnnualTempLund(){
    std::ifstream file("data/cleaned/lund_tm_1780-2022.dat"); 
    if (!file.is_open()) {
        std::cerr << "cannot open datafile for Lund" << std::endl;
        return;
    }

    int year, month, day, source;
    double temp, tempCorr;
    std::map<int, double> yearsum;
    std::map<int, double> yearmean;
    std::map<int, int> daycount;
    std::map<int, double> ave30y;

    int startYear = 1780;
    int endYear = 2022;
    int minDays = 330; // NEW: require at least 330 days so missing seasons don't skew the mean

    while (file >> year >> month >> day >> temp >> tempCorr >> source) {
         if (year < startYear || year > endYear) continue; // CHANGED: also skip years after 2022
        yearsum[year] += tempCorr;
        daycount[year]++;
    }

    for (int i = startYear; i <= endYear; i++) {
        if (daycount[i] >= minDays) { // CHANGED: was > 150, which allowed up to 7 months missing
            yearmean[i] = yearsum[i] / daycount[i];
        } else {
            // NEW: list excluded years (1957-1960 have no data)
            std::cout << "Excluded " << i << " (" << daycount[i] << " days)" << std::endl;

        }
    }

    // CHANGED: annual means as a line graph instead of a histogram,
    // excluded years are skipped instead of being set to -999
    TGraph* gYear = new TGraph();
    for (int i = startYear; i <= endYear; i++) {
        if (yearmean.count(i)) { // count() avoids creating a 0 entry for missing years
            gYear->AddPoint(i, yearmean[i]);
        }
    }
    gYear->SetTitle("Lund Mean Annual Temperature;Year;Mean Temperature (#circC)");

   
    // 30-year moving average graph
    TGraph* g1 = new TGraph();
    for (int y = startYear + 14; y <= endYear - 15; y++) {
        double sum = 0.0;
        int validYears = 0;
        for (int i = y - 14; i <= y + 15; i++) {
            if (yearmean.find(i) != yearmean.end()) {
                sum += yearmean[i];
                validYears++;
            }
        }
        if (validYears > 20) { // Ensure robust moving average window
            ave30y[y] = sum / validYears;
            g1->AddPoint(y, ave30y[y]);
        }
    }

    // Best fit lines
    TF1* f1 = new TF1("f1", "pol1", 1795, 1850);
    TF1* f2 = new TF1("f2", "pol1", 1850, 1970);
    TF1* f3 = new TF1("f3", "pol1", 1970, endYear - 15); // CHANGED: was endYear, moving average ends at 2007
   
    // CHANGED: style for line graph instead of histogram fill
    gYear->SetLineColor(kBlue - 7);
    gYear->SetMarkerStyle(20);
    gYear->SetMarkerSize(0.4);
    gYear->SetMarkerColor(kBlue + 2);
    g1->SetLineWidth(3);
    g1->SetLineColor(kBlack);
    f1->SetLineColor(kRed);
    f2->SetLineColor(kBlue);
    f3->SetLineColor(kGreen + 2);

    TCanvas* c1 = new TCanvas("c1", "Lund Mean Annual Temperature", 900, 600);
    // CHANGED: draw line graph with axes, set axis ranges on the graph
    gYear->Draw("ALP");
    gYear->GetXaxis()->SetLimits(startYear - 1, endYear + 1);
    gYear->GetYaxis()->SetRangeUser(4.5, 12.5);
    g1->Draw("L SAME");
    g1->Fit(f1, "R SAME");
    g1->Fit(f2, "R+ SAME");
    g1->Fit(f3, "R+ SAME");

    // NEW: print slopes in C per decade
    std::cout << "Trend 1795-1850: " << f1->GetParameter(1) * 10 << " C/decade" << std::endl;
    std::cout << "Trend 1850-1970: " << f2->GetParameter(1) * 10 << " C/decade" << std::endl;
    std::cout << "Trend 1970-2007: " << f3->GetParameter(1) * 10 << " C/decade" << std::endl;

    TLegend* legend = new TLegend(0.60, 0.70, 0.92, 0.92);
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->AddEntry(gYear, "Annual Mean Temp", "LP");
    legend->AddEntry(g1, "30-Year Moving Avg", "L");
    // CHANGED: one legend entry per fit instead of one shared entry
    legend->AddEntry(f1, "Trend 1795-1850", "L");
    legend->AddEntry(f2, "Trend 1850-1970", "L");
    legend->AddEntry(f3, "Trend 1970-2007", "L");
    legend->Draw();

}
// MeanAnnualTempLundClean();