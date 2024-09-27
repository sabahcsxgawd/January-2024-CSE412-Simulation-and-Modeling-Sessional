#include "../include/Simulation.h"
#include "../include/defs.h"

#include <iostream>
#include <iomanip>

Simulation::Simulation()
{
    // open outFile2
    this->outFile2.open("out2.txt");

    if (!this->outFile2)
    {
        std::cout << "Error opening output file2\n";
        exit(1);
    }

    // Specify current event number to be 0
    this->curr_event_num = 0;

    // Specify next event customer to be 0
    this->next_event_cust = 0;

    // Specify the number of events for the timing function
    this->num_events = 2;

    // Initialize the simulation clock
    this->sim_time = 0.0;

    // Initialize the state variables
    this->server_status = IDLE;
    this->num_in_q = 0;
    this->time_last_event = 0.0;

    // Initialize the statistical counters
    this->num_custs_delayed = 0;
    this->total_of_delays = 0.0;
    this->area_num_in_q = 0.0;
    this->area_server_status = 0.0;
    
    this->next_event_data.resize(this->num_events);

}

void Simulation::set_arrival_gen(int seed)
{
    this->arrival_gen = new RandGen(seed, this->mean_interarrival);
}

void Simulation::set_service_gen(int seed)
{
    this->service_gen = new RandGen(seed, this->mean_service);
}

void Simulation::init_event_list(void)
{
    // Initialize the event list with the arrival event
    this->next_event_data[0] = std::make_pair(this->sim_time + this->arrival_gen->get(), 1);
    this->next_event_data[1] = std::make_pair(INF, -1);
}

void Simulation::timing(void)
{
    int i;
    double min_next_event_data = 1.0e+29;

    this->next_event_type = -1;

    // Determine the event type of the next event to occur
    for (i = 0; i < this->num_events; ++i)
    {
        if (this->next_event_data[i].first < min_next_event_data)
        {
            min_next_event_data = this->next_event_data[i].first;
            this->next_event_type = i;
            this->next_event_cust = this->next_event_data[i].second;
        }
    }

    // Check to see if the event list is empty
    if (this->next_event_type == -1)
    {
        // The event list is empty, so stop the simulation
        std::cout << "\nEvent list empty at time " << this->sim_time << '\n';
        exit(1);
    }

    // The event list is not empty, so advance the simulation clock
    this->sim_time = min_next_event_data;
}

void Simulation::arrive(void) {
    double delay;

    // print next event : arrival
    this->outFile2 << ++this->curr_event_num << ". Next event: Customer " << this->next_event_cust << " Arrival\n";

    // Schedule next arrival
    this->next_event_data[0] = std::make_pair(this->sim_time + this->arrival_gen->get(), this->next_event_cust + 1);

    // Check to see if server is busy
    if (this->server_status == BUSY)
    {
        // Server is busy, so increment number of customers in queue
        ++this->num_in_q;        

        // There is room in the queue, so store the time of arrival of the arriving customer at the (new) end of time_arrival
        this->time_arrival.push_back(this->sim_time);
    }
    else
    {
        // Server is idle, so arriving customer has a delay of zero
        delay = 0.0;
        this->total_of_delays += delay;

        // Increment the number of customers delayed, and make server busy
        ++this->num_custs_delayed;
        this->server_status = BUSY;

        // print number of customers delayed
        this->outFile2 << "\n---------No. of customers delayed: " << this->num_custs_delayed << "--------\n\n";

        // Schedule a departure (service completion)
        this->next_event_data[1] = std::make_pair(this->sim_time + this->service_gen->get(), this->next_event_cust);
    }
}

void Simulation::depart(void) {
    int i;
    double delay;

    // print next event: departure    
    this->outFile2 << ++this->curr_event_num << ". Next event: Customer " << this->next_event_cust << " Departure\n";

    // Check to see if queue is empty
    if (this->num_in_q == 0)
    {
        // The queue is empty, so make the server idle and eliminate the departure (service completion) event from consideration
        this->server_status = IDLE;
        this->next_event_data[1] = std::make_pair(INFINITY, -1);
    }
    else
    {
        // The queue is nonempty, so decrement the number of customers in queue
        --this->num_in_q;

        // Compute the delay of the customer who is beginning service and update the total delay accumulator
        delay = (this->sim_time - this->time_arrival[0]);
        this->total_of_delays += delay;

        // Increment the number of customers delayed, and schedule the departure
        ++this->num_custs_delayed;
        this->next_event_data[1] = std::make_pair(this->sim_time + this->service_gen->get(), this->next_event_cust + 1);

        // print number of customers delayed
        this->outFile2 << "\n---------No. of customers delayed: " << this->num_custs_delayed << "--------\n\n";

        // Move each customer in queue (if any) up one place
        this->time_arrival.erase(this->time_arrival.begin());
    }
}

void Simulation::update_time_avg_stats(void) {
    double time_since_last_event;

    // Compute time since last event, and update last-event-time marker
    time_since_last_event = (this->sim_time - this->time_last_event);
    this->time_last_event = this->sim_time;

    // Update area under number-in-queue function
    this->area_num_in_q += (this->num_in_q * time_since_last_event);

    // Update area under server-busy indicator function
    this->area_server_status += (this->server_status * time_since_last_event);    
}

void Simulation::report(void) {
    // Compute and write estimates of desired measures of performance
    this->outFile1 << "\n\n"
                   << std::left << std::setw(30) << "Average delay in queue:" << std::right << std::setw(10) << (this->total_of_delays / this->num_custs_delayed) << " minutes\n"
                   << std::left << std::setw(30) << "Average number in queue:" << std::right << std::setw(10) << (this->area_num_in_q / this->sim_time) << '\n'
                   << std::left << std::setw(30) << "Server utilization:" << std::right << std::setw(10) << (this->area_server_status / this->sim_time) << '\n'
                   << std::left << std::setw(30) << "Time simulation ended:" << std::right << std::setw(10) << this->sim_time << " minutes\n";
}

void Simulation::run(void) {
    this->inFile.open("in.txt");
    this->outFile1.open("out1.txt");

    if (!this->inFile)
    {
        std::cout << "Error opening input file\n";
        exit(1);
    }

    if (!this->outFile1)
    {
        std::cout << "Error opening output file1\n";
        exit(1);
    }

    // Read input parameters
    this->inFile >> this->mean_interarrival >> this->mean_service >> this->num_delays_required;

    // arrival_gen and service_gen are initialized in the constructor
    this->set_arrival_gen(42);
    this->set_service_gen(24);

    // Write report heading and input parameters
    this->outFile1 << "Single-server queueing system\n\n";
    this->outFile1 << std::left << std::setw(30) << "Mean interarrival time:" << std::right << std::setw(10) << this->mean_interarrival << " minutes\n";
    this->outFile1 << std::left << std::setw(30) << "Mean service time:" << std::right << std::setw(10) << this->mean_service << " minutes\n";
    this->outFile1 << std::left << std::setw(30) << "Number of customers:" << std::right << std::setw(10) << this->num_delays_required << '\n';

    // close input file
    this->inFile.close();

    // Initialize the simulation
    this->init_event_list();

    // Run the simulation while more delays are still needed
    while (this->num_custs_delayed < this->num_delays_required)
    {
        // Determine the next event
        this->timing();

        // Update time-average statistical accumulators
        this->update_time_avg_stats();

        // Invoke the appropriate event function
        switch (this->next_event_type)
        {
        case 0:
            this->arrive();
            break;
        case 1:
            this->depart();
            break;
        default:
            std::cout << "Error: next_event_type is invalid\n";
            exit(1);
        }
    }

    // Invoke the report generator and end the simulation
    this->report();

    // close output files
    this->outFile1.close();
    this->outFile2.close();

    delete this->arrival_gen;
    delete this->service_gen;

}