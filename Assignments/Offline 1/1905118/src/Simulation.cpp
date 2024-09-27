#include "../include/Simulation.h"
#include "../include/defs.h"

Simulation::Simulation()
{
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
    
    this->time_next_event.resize(this->num_events);

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
    this->time_next_event[0] = this->sim_time + this->arrival_gen->get();
    this->time_next_event[1] = INF;
}

void Simulation::timing(void)
{
    int i;
    double min_time_next_event = 1.0e+29;

    this->next_event_type = -1;

    // Determine the event type of the next event to occur
    for (i = 0; i < this->num_events; ++i)
    {
        if (this->time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = this->time_next_event[i];
            this->next_event_type = i;
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
    this->sim_time = min_time_next_event;
}

void Simulation::arrive(void) {
    double delay;

    // Schedule next arrival
    this->time_next_event[0] = (this->sim_time + this->arrival_gen->get());

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

        // Schedule a departure (service completion)
        this->time_next_event[1] = (this->sim_time + this->service_gen->get());
    }
}

void Simulation::depart(void) {
    int i;
    double delay;

    // Check to see if queue is empty
    if (this->num_in_q == 0)
    {
        // The queue is empty, so make the server idle and eliminate the departure (service completion) event from consideration
        this->server_status = IDLE;
        this->time_next_event[1] = (INFINITY);
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
        this->time_next_event[1] = (this->sim_time + this->service_gen->get());

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
    this->outFile << "\n\nAverage delay in queue: " << (this->total_of_delays / this->num_custs_delayed) << '\n';
    this->outFile << "Average number in queue: " << (this->area_num_in_q / this->sim_time) << '\n';
    this->outFile << "Server utilization: " << (this->area_server_status / this->sim_time) << '\n';
    this->outFile << "Time simulation ended: " << this->sim_time << '\n';
}

void Simulation::run(void) {
    this->inFile.open("in.txt");
    this->outFile.open("out1.txt");

    if (!this->inFile)
    {
        std::cout << "Error opening input file\n";
        exit(1);
    }

    if (!this->outFile)
    {
        std::cout << "Error opening output file\n";
        exit(1);
    }

    // Read input parameters
    this->inFile >> this->mean_interarrival >> this->mean_service >> this->num_delays_required;

    // arrival_gen and service_gen are initialized in the constructor
    this->set_arrival_gen(42);
    this->set_service_gen(24);

    // Write report heading and input parameters
    this->outFile << "Single-server queueing system\n\n";
    this->outFile << "Mean interarrival time: " << this->mean_interarrival << '\n';
    this->outFile << "Mean service time: " << this->mean_service << '\n';
    this->outFile << "Number of customers: " << this->num_delays_required << '\n';

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

    // close output file
    this->outFile.close();

    delete this->arrival_gen;
    delete this->service_gen;

}