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

    while (file >> year >> month >> day >> temp >> tempCorr >> source) {
        if (year < startYear) continue; 
        yearsum[year] += tempCorr;
        daycount[year]++;
    }

    for (int i = startYear; i <= endYear; i++) {
        if (daycount[i] > 150) { // Require at least ~150 days of data to avoid skewed means
            yearmean[i] = yearsum[i] / daycount[i];
        }
    }

    int nBins = endYear - startYear + 1;
    TH1D* h1 = new TH1D("h1", "Lund Mean Annual Temperature;Year;Mean Temperature (#circC)", nBins, startYear - 0.5, endYear + 0.5);
    
    // Set minimum y-axis range so missing data doesn't plunge to 0
    h1->SetMinimum(4.5);
    h1->SetMaximum(12.5);

    for (int i = startYear; i <= endYear; i++) {
        if (yearmean.find(i) != yearmean.end()) {
            h1->SetBinContent(h1->FindBin(i), yearmean[i]);
        } else {
            // Set missing years to a safe baseline or leave blank
            h1->SetBinContent(h1->FindBin(i), -999); 
        }
    }
   
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
    TF1* f3 = new TF1("f3", "pol1", 1970, endYear);
   
    //h1->SetFillColor(kBlue - 9);
    h1->SetLineColor(kBlue + 2);
    g1->SetLineWidth(3);
    g1->SetLineColor(kBlack);
    f1->SetLineColor(kRed);
    f2->SetLineColor(kBlue);
    f3->SetLineColor(kGreen + 2);

    TCanvas* c1 = new TCanvas("c1", "Lund Mean Annual Temperature", 900, 600);
    h1->Draw("HIST");
    g1->Draw("L SAME");
    g1->Fit(f1, "R SAME");
    g1->Fit(f2, "R+ SAME");
    g1->Fit(f3, "R+ SAME");

    // Add a clean legend (following project guidelines)
    TLegend* legend = new TLegend(0.65, 0.75, 0.92, 0.90);
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->AddEntry(h1, "Annual Mean Temp", "F");
    legend->AddEntry(g1, "30-Year Moving Avg", "L");
    legend->AddEntry(f1, "Linear Trend Fits", "L");
    legend->Draw();
}