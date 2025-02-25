#ifndef SIMULATION_H
#define SIMULATION_H

#include <fstream>
#include <vector>
#include <utility>
#include "RandGen.h"

class Simulation
{

public:
    Simulation();
    void run(void);

private:
    int next_event_type, num_custs_delayed, num_delays_required, num_events,
        num_in_q, server_status, curr_event_num, next_event_cust;
    double area_num_in_q, area_server_status, mean_interarrival, mean_service,
        sim_time, time_last_event, total_of_delays;

    std::vector<double> time_arrival;
    std::vector<std::pair<double, int>> next_event_data;

    std::ifstream inFile;
    std::ofstream outFile1, outFile2;

    RandGen rand_gen;

    void init_event_list(void);
    void timing(void);
    void arrive(void);
    void depart(void);
    void report(void);
    void update_time_avg_stats(void);  
};

#endif // SIMULATION_H