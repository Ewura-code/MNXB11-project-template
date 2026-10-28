
#include <iostream>
#include <fstream>
#include <map>


void MeanAnnualTemp(){
    // Open the data file
    std::ifstream file("data/cleaned/uppsala_tm_1722-2022.dat");
    if (!file.is_open()) {
        std::cerr << "cannot open datafile" << std::endl;
        return;
    }

    //variables
    int year, month, day, source;
    double temp, tempCorr;
    std::map<int, double> yearsum;
    std::map<int, double> yearmean;
    std::map<int, int> daycount;
    std::map<int, double> ave30y;

    //adds the temp each day of a year and counts how many entries per year
    while (file >> year >> month >> day >> temp >> tempCorr >> source) {
        if (year == 1722) continue; 
        yearsum[year] += tempCorr;
        daycount[year]++;
    }

    //calculate mean temp per year
    for (int i = 1723; i <= 2022; i++) {
        if (daycount[i] > 0) {
            yearmean[i] = yearsum[i] / daycount[i];
        }
    }

    //create histogram for mean temperature each year in uppsala 1723-2022
    TH1D* h1 = new TH1D("h1", "mean temperature each year in uppsala 1723-2022;year;mean temperature (#circC)", 300, 1723, 2023);
    for (int i = 1723; i <= 2022; i++) {
        h1 -> SetBinContent( i-1722, yearmean[i]);
    }
   
    //create 30-year moving-average graph
    TGraph* g1 = new TGraph();
    for (int y = 1723 + 14; y <= 2022 - 15; y++) {
    double sum = 0.0;
        for (int i = y - 14; i <= y + 15; i++) {
            sum += yearmean[i];
        }
    ave30y[y] = sum / 30.0;
    }
    
    for (int y = 1723 + 14; y <= 2022-15; y++) {
        g1->AddPoint(y, ave30y[y]);
    }
    g1->SetTitle("30-year average temperature");
    

    // best fit lines for 30-year average and get slope
    TF1* f1 = new TF1("f1", "pol1", 1737, 1850);
    TF1* f2 = new TF1("f2", "pol1", 1850, 1970);
    TF1* f3 = new TF1("f3", "pol1", 1970, 2007);
   
    //graphics
    g1->SetLineWidth(3);
    f1->SetLineColor(kRed);
    f2->SetLineColor(kBlue);
    f3->SetLineColor(kGreen);

    //draw all on the same canvas
    h1->Draw();
    g1->Draw("LP SAME");
    g1->Fit(f1, "R SAME");
    g1->Fit(f2, "R+ SAME");
    g1->Fit(f3, "R+ SAME");
}
    
