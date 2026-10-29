// Computes the relative annual frequency of warm days in Uppsala 
// Computes the moving average of the annual frequency of warm days
// Draws the annual frequency and moving average into a combined graph
// Run from the repository root

#include <iostream>
#include <fstream>
#include <vector>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>

void WarmDays() {
    // Read the Uppsala dataset 
    std::ifstream file("data/cleaned/filtered_uppsala_tm_1722-2022.dat");
    if (!file) {
        std::cerr << "Uppsala file not found" << std::endl; 
        return;
    }

    // Analyse the annual frequency of warm days
    int year, month, day, source_id;
    double observed_temperature, corrected_temperature;
    
    std::vector<int> total_days(301, 0);
    std::vector<int> warm_days(301, 0);
    std::vector<double> frequency(301, 0.0);

    // Tabulate total and warm days for each year
    while (file >> year >> month >> day
                >> observed_temperature >> corrected_temperature >> source_id) {
                    
            if (year < 1722 || year > 2022) {
            continue;
            }

            int index = year - 1722;

            total_days[index]++;

            if (corrected_temperature >= 20.0) {
                warm_days[index]++;
            }
        }


    // Compute relative frequency of warm days for each year
    for (int i = 0; i < 301; i++) {

        // Skips years where there are no measurements
        if (total_days[i] == 0) {
            continue;
        }

        frequency[i] = (static_cast<double>(warm_days[i]) / total_days[i]) * 100.0;
    }

    // Create a trailing 30-year average
    std::vector<double> moving_average(301, 0);
    std::vector<bool> valid_average(301, false);

    for (int i = 29; i < 301; i++) {

        double sum = 0.0;
        int valid_years = 0;

        for (int j = 0; j < 30; j++) {
            // Include years with at least 330 days
            if (total_days[i - j] >= 330) {
                sum += frequency[i - j];
                valid_years++;
            }
        }

        if (valid_years >= 27) {
            // Require at least 27 valid years in a 30 day window
            moving_average[i] = sum / valid_years;
            valid_average[i] = true;
        }

    }
    
    // Prepare a 2D Graph of year vs frequency
    TGraph graph;
    int point = 0;

    for (int i = 0; i < 301; i++) {

        // Include years with at least 330 days
        if (total_days[i] < 330) {
            continue;
        }

        int x_coordinate = 1722 + i;
        double y_coordinate = frequency[i];

        graph.SetPoint(point, x_coordinate, y_coordinate);
        point++;
    }

    // Prepare a second graph for the 30-year moving average
    TGraph moving_average_graph;
    int average_point = 0;

    for (int i = 29; i < 301; i++) {

        // Require at least 27 valid years in a 30-year window
        if (!valid_average[i]) {
            continue;
        }

        int x_coordinate = 1722 + i;
        double y_coordinate = moving_average[i];

        moving_average_graph.SetPoint(average_point, x_coordinate, y_coordinate);
        average_point++;
    }

    // Customise and save both graphs
    TCanvas canvas("canvas", "Uppsala Warm Days", 1000, 600);

    // Title
    graph.SetTitle(
    "Warm days in Uppsala (1722-2022);Year;Days with mean T >= 20 C (%)"
    );

    // Annual frequency points
    graph.SetMarkerStyle(20);
    graph.SetMarkerSize(0.5);
    graph.SetMarkerColor(kBlue);

    // 30-year moving average
    moving_average_graph.SetLineColor(kRed);
    moving_average_graph.SetLineWidth(3);

    // Draw both graphs
    graph.Draw("AP");
    moving_average_graph.Draw("L SAME"); 

    // Draw legends for both graphs
    TLegend legend(0.55, 0.75, 0.88, 0.88);

    legend.AddEntry(&graph, "Annual frequency", "p");
    legend.AddEntry(&moving_average_graph, "Trailing 30-year average", "l");

    legend.Draw();

    // Save the result
    canvas.SaveAs("warm_days.png"); // for quick viewing
    canvas.SaveAs("warm_days.pdf"); // for LaTeX

}