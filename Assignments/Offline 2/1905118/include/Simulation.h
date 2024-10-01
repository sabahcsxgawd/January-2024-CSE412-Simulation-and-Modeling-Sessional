#ifndef SIMULATION_H
#define SIMULATION_H

#define INF 1.0e+30

#include <fstream>
#include <vector>
#include "../include/RandGen.h"

class Simulation
{
public:
    Simulation();
    void initialize(void);
    void timing(void);
    void orderArrival(void);
    void demand(void);
    void evaluate(void);
    void report(void);
    void updateTimeAvgStats(void);
    void run();
private:
    int initialInventoryLevel;                         // Initial inventory level
    int currentInventoryLevel;                         // Current inventory level
    int numberOfMonths;                                // Number of months to simulate
    int numberOfPolicies;                              // Number of policies
    int numberOfEvents;                                // Number of events
    int numberOfDemandValues;                          // Number of demand values
    int smalls;                                        // Number of smalls
    int bigs;                                          // Number of bigs
    int orderAmount;                                   // Order amount
    int nextEventType;                                 // Next event type
    double simulationTime;                             // Simulation time
    double timeOfLastEvent;                            // Time of last event
    double meanInterDemandTime;                        // Mean interdemand time
    double setupCost;                                  // Setup cost
    double incrementalCost;                            // Incremental cost
    double holdingCost;                                // Holding cost
    double shortageCost;                               // Shortage cost
    double totalOrderingCost;                          // Total ordering cost
    double areaUnderHoldCostCurve;                     // Area under holding cost curve
    double areaUnderShortageCostCurve;                 // Area under shortage cost curve
    double minArrivalLag;                              // Minimum arrival lag
    double maxArrivalLag;                              // Maximum arrival lag

    std::vector<double> demandCumulativeProbabilities; // Demand cumulative probability
    std::vector<double> timeOfNextEvents;              // Time of next events

    std::ifstream inFile;                              // Input file
    std::ofstream outFile;                             // Output file

    RandGen randGen;                                   // Random number generator
};

#endif // SIMULATION_H