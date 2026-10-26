#include <fstream>
#include <iostream>
#include <map>

void coldDays()
{
    std::ifstream file("data/cleaned/uppsala_tm_1722-2022.dat");
    if (!file) { std::cout << "File not found" << std::endl; return; }
    std::map<int, int> cold;

    int year, month, day, source;
    double temp, tempCorr;
    while (file >> year >> month >> day >> temp >> tempCorr >> source) {
        if (year == 1722) continue; // data starts on 12 January
        if (tempCorr < 0) cold[year]++;
    }

    double early = 0, late = 0;
    for (int y = 1723; y <= 1772; y++) early += cold[y];
    for (int y = 1973; y <= 2022; y++) late += cold[y];
    std::cout << "Mean 1723-1772: " << early / 50 << std::endl;
    std::cout << "Mean 1973-2022: " << late / 50 << std::endl;

    auto hist = new TH1I("hist", ";Year;Days below 0 #circC", 300, 1722.5, 2022.5);
    for (int y = 1723; y <= 2022; y++) hist->SetBinContent(y - 1722, cold[y]);
    hist->Fit("pol1");

    // 31-year moving average
    auto avg = new TGraph();
    for (int y = 1738; y <= 2007; y++) {
        double sum = 0;
        for (int i = y - 15; i <= y + 15; i++) sum += cold[i];
        avg->SetPoint(avg->GetN(), y, sum / 31);
    }
    avg->SetLineWidth(3);
    avg->Draw("L");

    gSystem->mkdir("figures");
    gPad->SaveAs("figures/coldDays.png");
}