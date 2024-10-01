#include "../include/Simulation.h"

#include <iostream>
#include <iomanip>

Simulation::Simulation() {}

void Simulation::initialize(void)
{
    // Initialize the simulation clock
    this->simulationTime = 0.0;

    // Initialize the state variables
    this->currentInventoryLevel = this->initialInventoryLevel;
    this->timeOfLastEvent = 0.0;

    // Initialize the statistical counters
    this->areaUnderHoldCostCurve = 0.0;
    this->areaUnderShortageCostCurve = 0.0;
    this->totalOrderingCost = 0.0;

    // Initialize the event list
    this->timeOfNextEvents[0] = INF;
    this->timeOfNextEvents[1] = this->simulationTime + this->randGen.getExponential(this->meanInterDemandTime);
    this->timeOfNextEvents[2] = this->numberOfMonths;
    this->timeOfNextEvents[3] = 0.0;
}

void Simulation::timing(void)
{
    int i;
    double minNextEventTime = 1.0e+29;

    // Determine the event type of the next event to occur
    this->nextEventType = -1;

    for(i = 0; i < this->numberOfEvents; i++)
    {
        if(this->timeOfNextEvents[i] < minNextEventTime)
        {
            minNextEventTime = this->timeOfNextEvents[i];
            this->nextEventType = i;
        }
    }

    // Check to see if the event list is empty
    if (this->nextEventType == -1)
    {
        // The event list is empty, so stop the simulation
        std::cout << "\nEvent list empty at time " << this->simulationTime << '\n';
        exit(1);
    }

    // The event list is not empty, so advance the simulation clock
    this->simulationTime = minNextEventTime;
}

void Simulation::orderArrival(void)
{
    // Increment the inventory level by the order amount
    this->currentInventoryLevel += this->orderAmount;

    // Since no order is outstanding, eliminate the order arrival event from consideration
    this->timeOfNextEvents[0] = INF;
}

void Simulation::demand(void)
{
    // Decrement the inventory level by the demand amount
    this->currentInventoryLevel -= this->randGen.getRandomInt(this->demandCumulativeProbabilities);

    // Schedule the next demand event
    this->timeOfNextEvents[1] = this->simulationTime + this->randGen.getExponential(this->meanInterDemandTime);
}

void Simulation::evaluate(void)
{
    // Check whether the inventory level is less than smalls
    if(this->currentInventoryLevel < this->smalls) {
        // The inventory level is less than smalls, so place an order for appropriate amount
        this->orderAmount = this->bigs - this->currentInventoryLevel;
        this->totalOrderingCost += this->setupCost + this->incrementalCost * this->orderAmount;

        // Schedule the order arrival event
        this->timeOfNextEvents[0] = this->simulationTime + this->randGen.getUniform(this->minArrivalLag, this->maxArrivalLag);
    }

    // Regardless of whether an order is placed, schedule the next evaluation event
    this->timeOfNextEvents[3] = this->simulationTime + 1.0;
}

void Simulation::report(void)
{
    // Compute and write estimates of desired measures of performance
    double avgHoldCost = this->areaUnderHoldCostCurve / this->numberOfMonths;
    double avgShortageCost = this->areaUnderShortageCostCurve / this->numberOfMonths;
    double avgOrderingCost = this->totalOrderingCost / this->numberOfMonths;

    this->outFile << '(' << this->smalls << ", " << this->bigs << ')';
    this->outFile << std::setw(20) << avgHoldCost + avgShortageCost + avgOrderingCost;
    this->outFile << std::setw(20) << avgOrderingCost;
    this->outFile << std::setw(20) << avgHoldCost;
    this->outFile << std::setw(20) << avgShortageCost << "\n\n";    
}

void Simulation::updateTimeAvgStats(void)
{
    double timeSinceLastEvent = this->simulationTime - this->timeOfLastEvent;
    this->timeOfLastEvent = this->simulationTime;

    if(this->currentInventoryLevel < 0) {
        this->areaUnderShortageCostCurve -= this->currentInventoryLevel * timeSinceLastEvent;
    } else {
        this->areaUnderHoldCostCurve += this->currentInventoryLevel * timeSinceLastEvent;
    }
}

void Simulation::run()
{
    // open input and output files
    this->inFile.open("in.txt");
    this->outFile.open("out.txt");

    // check if the files are opened successfully
    if(!this->inFile.is_open() || !this->outFile.is_open()) {
        std::cout << "Error opening files\n";
        exit(1);
    }

    // read the input parameters
    this->inFile >> this->initialInventoryLevel;
    this->inFile >> this->numberOfMonths;
    this->inFile >> this->numberOfPolicies;
    this->inFile >> this->numberOfDemandValues;

    this->demandCumulativeProbabilities.resize(this->numberOfDemandValues);

    this->numberOfEvents = 4;
    this->timeOfNextEvents.resize(this->numberOfEvents);

    this->inFile >> this->meanInterDemandTime;
    this->inFile >> this->setupCost;
    this->inFile >> this->incrementalCost;
    this->inFile >> this->holdingCost;
    this->inFile >> this->shortageCost;
    this->inFile >> this->minArrivalLag;
    this->inFile >> this->maxArrivalLag;

    for(int i = 0; i < this->numberOfDemandValues; i++) {
        this->inFile >> this->demandCumulativeProbabilities[i];
    }

    this->outFile << std::fixed << std::setprecision(2);

    this->outFile << "------Single-Product Inventory System------\n\n";
    this->outFile << "Initial inventory level: " << this->initialInventoryLevel << " items\n\n";
    this->outFile << "Number of demand sizes: " << this->numberOfDemandValues << "\n\n";
    this->outFile << "Distribution function of demand sizes: ";
    for(int i = 0; i < this->numberOfDemandValues; i++) {
        this->outFile << this->demandCumulativeProbabilities[i] << " ";
    }
    this->outFile << "\n\n";
    this->outFile << "Mean inter-demand time: " << this->meanInterDemandTime << " months\n\n";
    this->outFile << "Delivery lag range: " << this->minArrivalLag << " to " << this->maxArrivalLag << " months\n\n";
    this->outFile << "Length of simulation: " << this->numberOfMonths << " months\n\n";
    this->outFile << "Costs:\n";
    this->outFile << "K = " << this->setupCost << " \n";
    this->outFile << "i = " << this->incrementalCost << " \n";
    this->outFile << "h = " << this->holdingCost << " \n";
    this->outFile << "pi = " << this->shortageCost << " \n\n";

    this->outFile << "Number of policies: " << this->numberOfPolicies << "\n\n";

    this->outFile << "Policies: \n";

    this->outFile << "--------------------------------------------------------------------------------------------------\n";
    this->outFile << " Policy        Avg_total_cost     Avg_ordering_cost      Avg_holding_cost     Avg_shortage_cost\n";
    this->outFile << "--------------------------------------------------------------------------------------------------\n\n";

    for(int i = 0; i < this->numberOfPolicies; i++) {
        this->inFile >> this->smalls >> this->bigs;

        this->initialize();

        do {
            this->timing();

            this->updateTimeAvgStats();

            switch(this->nextEventType) {
                case 0:
                    this->orderArrival();
                    break;
                case 1:
                    this->demand();
                    break;
                case 3:
                    this->evaluate();
                    break;
                case 2:
                    this->report();
                    break;
            }
        } while(this->nextEventType != 2);
    }

        this->outFile << "--------------------------------------------------------------------------------------------------";

    // close the files
    this->inFile.close();
    this->outFile.close();

}