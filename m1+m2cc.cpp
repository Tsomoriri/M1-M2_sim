#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>

 // struct Simulation and struct SimResults declaration
struct Simulation;
struct SimResults;
// Function declarations
float expon_mean(Simulation &sim, double arrival_rate);
float expon_service(Simulation &sim);
void initialise(Simulation &sim, int c);
void timing(Simulation &sim);
void arrive(Simulation &sim, SimResults &r , int c);
void depart(Simulation &sim);
void update_time_avg_stats(Simulation &sim);
void report(Simulation &sim, SimResults &results, int c);
void save_to_csv(SimResults &results, const std::string &filename);
void theo_loss_prob(Simulation &sim, SimResults &r, int c);
double factorial(int n);
// struct SimResults definition
struct SimResults {
    std::vector<double> arrival_rate_handover; // Arrival rate of handover calls
    std::vector<double> arrival_rate_newcall; // Arrival rate of new calls
    std::vector<double> utilisation_handover; // Utilisation of handover servers
    std::vector<double> utilisation_newcall; // Utilisation of newcall servers
    std::vector<long double> loss_probability_handover; // Loss probability of handover calls
    std::vector<long double> loss_probability_newcall; // Loss probability of new calls
    std::vector<long double> aggregate_loss_probability; // Aggregate loss probability
    std::vector<long double> theo_loss_probability_handover; // Theoretical loss probability of handover calls
    std::vector<long double> theo_loss_probability_newcall; // Theoretical loss probability of new calls
    std::vector<long double> theo_aggregate_loss_probability; // Theoretical aggregate loss probability
    std::vector<double> time;
   
};
// struct Simulation definition
struct Simulation {
    // Simulation parameters and variables
    double sim_time; // Simulation time
    double sim_runtime; // Simulation runtime eg = 10,000
    double mean_interarrival_handover; // Mean interarrival time of handover calls
    double mean_interarrival_newcall; // Mean interarrival time of new calls
    double mean_service; // Mean service time
    double arrival_rate_handover; // Arrival rate of each handover calls
    double arrival_rate_newcall; // Arrival rate of each new calls
    double service_rate; // Service rate of each server
    int q_limit; // Queue limit
    int num_in_q; // Number of customers in queue
    int blocked_in_q; // Number of customers blocked in queue
    int num_customer_handover;  // Number of handover customers
    int num_customer_newcall; // Number of newcall customers
    int next_event_type; // Type of next event
    int n; // Number of available servers
    double num_newcall_loss; // Number of newcall loss
    double num_handover_loss; // Number of handover loss
    double total_newcall_arrivals; // Total number of newcall arrivals
    double total_handover_arrivals; // Total number of handover arrivals
    std::vector<int> server_status;  // Status of each server
    std::vector<double> time_last_event; // Time of last event
    std::vector<double> time_next_event; // Time of next event
    std::vector<double> time_arrival; // Time of arrival
    double area_num_in_q; // Area of number in queue
    std::vector<double> area_server_status; // Area of server status
    double total_server_utilisation; // Total server utilisation
    double aggregate_loss_probability; // Aggregate loss probability

    // Binary search method for finding newcall rate
    double findNewcallRate(double handoverRate, SimResults& results) {
        double lowerLimit = 0.0001;
        double upperLimit = 3.0;
        double threshold = 0.01;
        double abp = 0.0;
        double newcallRate = 0.0;
        int num_servers = 16;
        ;    
        while (upperLimit - lowerLimit > threshold) {
            double midPoint = (lowerLimit + upperLimit) / 2.0;
            mean_interarrival_handover = handoverRate;
            mean_interarrival_newcall = 1.0 / midPoint;
            // Run simulation
            initialise(*this, num_servers); // Initialise simulation
            while (sim_time < sim_runtime) { // Run simulation until sim_time reaches sim_runtime
                timing(*this); // Determine the next event type
                switch (next_event_type) {
                    case 0: // If next event is an arrival
                        arrive(*this, results, num_servers);
                        break;
                    default:
                        depart(*this); // If next event is a departure
                        break;
                }
                update_time_avg_stats(*this); // Update time average statistics
                report(*this, results, num_servers); // Report statistics
            }

            // Check if abp < 0.02
            if (aggregate_loss_probability < 0.02) { 
                newcallRate = mean_interarrival_newcall;
                lowerLimit = midPoint;
            } else {
                upperLimit = midPoint;
            }

        } save_to_csv(results, "FindNewCAllRATE.csv"); // Save results to csv file

        return newcallRate; // Return newcall rate
    }

    // Binary search method for finding handover rate
    double findHandoverRate(double newcallRate, SimResults& results) {
        double lowerLimit = 0.0001;
        double upperLimit = 3.0;
        double threshold = 0.01;
        double abp = 0.0;
        double handoverRate = 0.0;
        int num_servers = 16;
        while (upperLimit - lowerLimit > threshold) {
            double midPoint = (lowerLimit + upperLimit) / 2.0;
            mean_interarrival_handover = 1.0 / midPoint;
            mean_interarrival_newcall = newcallRate;
            // Run simulation
            initialise(*this, num_servers); // Initialise simulation
            while (sim_time < sim_runtime) {
                timing(*this); // Determine the next event type
                switch (next_event_type) {
                    case 0: // If next event is an arrival
                        arrive(*this, results, num_servers);
                        break;
                    default:
                        depart(*this); // If next event is a departure
                        break;
                }
                update_time_avg_stats(*this); // Update time average statistics
                report(*this,results, num_servers); // Report statistics
            }
            save_to_csv(results, "FindHandoverRATE.csv"); // Save results to csv file
            // Check if abp < 0.02 
            if (aggregate_loss_probability < 0.02) {
                handoverRate = mean_interarrival_handover;
                lowerLimit = midPoint;
            } else {
                upperLimit = midPoint;
            }
        } 

        return handoverRate; // Return handover rate
    }
};







float expon_mean(Simulation &sim, double arrival_rate) {
    double u = static_cast<double>(rand()) / RAND_MAX; // Uniform random number between 0 and 1
    return -log(1 - u) / arrival_rate; 
}

float expon_service(Simulation &sim) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 1.0); // Uniform random number between 0 and 1

    float RAND = distribution(generator);
    return (-1.0 / sim.mean_service * log(RAND));
}

void initialise(Simulation &sim, int c) {
    sim.sim_time = 0.0; // Initialise simulation time
    sim.sim_runtime = 10000; // Initialise simulation runtime
    sim.mean_service = 100; // Initialise mean service time 
    sim.arrival_rate_handover = 0.0; // Initialise arrival rate of handover calls
    sim.arrival_rate_newcall = 0.0; // Initialise arrival rate of new calls
    sim.service_rate = 0.0; // Initialise service rate
    sim.q_limit = c; // Initialise queue limit
    sim.num_in_q = 0; // no customers in the queue, as queue =0 for all M/M/c/c
    sim.blocked_in_q = 0; // Initialise number of customers blocked in queue
    sim.n = 0; // Initialise number of available servers
    sim.num_customer_handover = 0; // Initialise number of handover customers
    sim.num_customer_newcall = 0; // Initialise number of newcall customers
    sim.num_newcall_loss = 0.0; // Initialise number of newcall loss
    sim.num_handover_loss = 0.0; // Initialise number of handover loss
    sim.total_newcall_arrivals = sim.mean_interarrival_newcall * sim.sim_runtime; // Initialise total number of newcall arrivals
    sim.total_handover_arrivals = sim.mean_interarrival_handover * sim.sim_runtime; // Initialise total number of handover arrivals
    sim.server_status.resize(c, 0); // Initialise status of each server
    sim.area_num_in_q = 0.0; // Initialise area of number in queue
    sim.area_server_status.resize(c, 0.0); // Initialise area of server status
    sim.time_last_event.resize(c + 1, 0.0); // Initialise time of last event
    sim.time_next_event.resize(c + 1, 1.0e+30); // Initialise time of next event
    sim.time_next_event[0] = sim.sim_time + expon_mean(sim, sim.mean_interarrival_handover); // Initialise time of next handover arrival
    sim.time_next_event[1] = sim.sim_time + expon_mean(sim, sim.mean_interarrival_newcall); // Initialise time of next newcall arrival
    sim.aggregate_loss_probability = 0.0; // Initialise aggregate loss probability
}

void timing(Simulation &sim) {
 // Determine the next event type
    double min_time_next_event = 1.0e+30;
    for (int i = 0; i < sim.time_next_event.size(); i++) {
        if (sim.time_next_event[i] <= min_time_next_event) {
            sim.next_event_type = i;
            min_time_next_event = sim.time_next_event[i]; // determine the event type of the next event to occur (0 = arrival, 1-15 = departure)
        } 
    }

    sim.time_last_event[sim.next_event_type] = sim.sim_time; // Update time since last event occurred
    sim.sim_time = sim.time_next_event[sim.next_event_type]; // Advance the simulation time clock
}

void theo_loss_prob(Simulation &sim, SimResults &r, int c) {
    double p0 = 0.0; // Probability of 0 customers in the system
    long double ploss1 = 0.0; // Probability of loss of newcall calls
    long double ploss2 = 0.0; // Probability of loss of handover calls
    double p01 = 0.0;
    double p02 = 0.0;

    for (int k = 0; k < c - sim.n; k++) {
        p01 += pow(((sim.arrival_rate_handover + sim.arrival_rate_newcall) / sim.service_rate), k) / factorial(k);
    }

    for (int k = c - sim.n + 1; k < c; k++) {
        p02 += pow(((sim.arrival_rate_handover + sim.arrival_rate_newcall) / sim.service_rate), c - sim.n) * (pow(sim.arrival_rate_handover / sim.service_rate, k - c - sim.n)) / (factorial(k));
    }

    p0 = 1.0 / (p01 + p02);

    for (int k = c - sim.n + 1; k < c; k++) {
        ploss1 += pow(((sim.arrival_rate_handover + sim.arrival_rate_newcall) / sim.service_rate), c - sim.n) * pow(sim.arrival_rate_newcall / sim.service_rate, k - c - sim.n) * p0 / factorial(k);
    }

    for (int k = 0 ; k < c - sim.n; k++) {
        ploss2 = pow(((sim.arrival_rate_handover + sim.arrival_rate_newcall) / sim.service_rate), k) * (p0) / factorial(k);
        ploss2 += ploss2;
    }

    long double loss_probability_handover = ploss2; // Loss probability of handover calls
    long double loss_probability_newcall = ploss1; // Loss probability of new calls
    long double abp = loss_probability_newcall + loss_probability_handover * 10; // Aggregate loss probability
    // Check if variables are NaN and change them to 0
    if (std::isnan(loss_probability_handover)) {
        loss_probability_handover = 0.0; 
    }
    if (std::isnan(loss_probability_newcall)) {
        loss_probability_newcall = 0.0;
    }
    if (std::isnan(abp)) {
        abp = 0.0;
    }

    r.theo_loss_probability_handover.push_back(loss_probability_handover); // Theoretical loss probability of handover calls
    r.theo_loss_probability_newcall.push_back(loss_probability_newcall); // Theoretical loss probability of new calls
    r.theo_aggregate_loss_probability.push_back(abp); // Theoretical aggregate loss probability
}
void arrive(Simulation &sim, SimResults &r,int c) {
    int available_servers = 0; // Number of available servers
    int handover_server = -1; // Server for handover call
    int newcall_server = -1; // Server for newcall call
    for (int i = 0; i < c; i++) {
        if (sim.server_status[i] == 0) { // If server is available
            available_servers++; // Increment number of available servers
            if ((handover_server == -1) && (available_servers > 0)) { // If handover server is available
                handover_server = i; // Assign handover server
            } else if ((newcall_server == -1) && (available_servers > 2)) { // If newcall server is available
                newcall_server = i; // Assign newcall server
            }
        }
    }
   
    sim.n = available_servers; // Update number of available servers
    theo_loss_prob(sim,r, c); // Calculate theoretical loss probability     
    if (available_servers > 0 && sim.num_customer_handover < sim.total_handover_arrivals) { // If handover server is available
        if (handover_server != -1) { // If handover server is available
            sim.server_status[handover_server] = 1; // Update server status
            sim.service_rate = expon_service(sim); // Update service rate
            sim.time_next_event[handover_server] = sim.sim_time + expon_service(sim); // Update time of next event
            sim.num_customer_handover++; // Increment number of handover customers
        } else {
        sim.num_handover_loss++; // Increment number of handover loss
        }
        return;
    } else {
        sim.num_handover_loss++;} // Increment number of handover loss
       
    

    if (available_servers > 2 && sim.num_customer_newcall < sim.total_newcall_arrivals) { // If newcall server is available
        if (newcall_server != -1) { // If newcall server is available
            sim.server_status[newcall_server] = 1; // Update server status
            sim.service_rate = expon_service(sim); // Update service rate
            sim.time_next_event[newcall_server] = sim.sim_time + expon_service(sim); // Update time of next event
            sim.num_customer_newcall++; // Increment number of newcall customers
        } else {
        sim.num_newcall_loss++; // Increment number of newcall loss
        
   
    }
        return;
    } else {
        sim.num_newcall_loss++; // Increment number of newcall loss
    }   
    sim.arrival_rate_handover = expon_mean(sim, sim.mean_interarrival_handover); // Update arrival rate of handover calls
    sim.arrival_rate_newcall = expon_mean(sim, sim.mean_interarrival_newcall); // Update arrival rate of new calls
    sim.time_next_event[0] = sim.sim_time + sim.arrival_rate_handover; // Update time of next handover arrival
    sim.time_next_event[1] = sim.sim_time + sim.arrival_rate_newcall; // Update time of next newcall arrival
}

void depart(Simulation &sim) {
    int server = sim.next_event_type; // Server from which a customer is departing
    sim.server_status[server] = 0; // Update server status
    sim.time_next_event[server] = 1.0e+30; // Update time of next event
}

void update_time_avg_stats(Simulation &sim) { // Update area accumulators for time-average statistics
    int server = -1; // Server from which a customer is departing
    for (int i = 0; i < sim.server_status.size(); i++) { // Find server from which a customer is departing
        if (sim.server_status[i] == 1) { // If server is busy
            server = i; // Assign server
            break;
        }
    }

    double time_since_last_event = sim.sim_time - sim.time_last_event[server]; // Time since last event occurred
    sim.area_num_in_q += sim.num_in_q * time_since_last_event; // Update area of number in queue

    for (int i = 0; i < sim.server_status.size(); i++) { // Update area of server status
        sim.area_server_status[i] += sim.server_status[i] * time_since_last_event; 
    }
}

void report(Simulation &sim, SimResults &results, int c) { // Report generator function
    double utilisation_handover = sim.area_server_status[0] / sim.sim_time; // Utilisation of handover servers
    double utilisation_newcall = (sim.area_server_status[1] + sim.area_server_status[2]) / sim.sim_time; // Utilisation of newcall servers
    long double loss_probability_handover = sim.num_handover_loss / sim.total_handover_arrivals; // Loss probability of handover calls
    long double loss_probability_newcall = sim.num_newcall_loss / sim.total_newcall_arrivals; // Loss probability of new calls
    long double abp = loss_probability_newcall + loss_probability_handover *10; // Aggregate loss probability
   
    results.arrival_rate_handover.push_back(sim.arrival_rate_handover); // Arrival rate of handover calls
    results.arrival_rate_newcall.push_back(sim.arrival_rate_newcall); // Arrival rate of new calls
    results.utilisation_handover.push_back(utilisation_handover); // Utilisation of handover servers
    results.utilisation_newcall.push_back(utilisation_newcall); // Utilisation of newcall servers
    results.loss_probability_handover.push_back(loss_probability_handover); // Loss probability of handover calls
    results.loss_probability_newcall.push_back(loss_probability_newcall); // Loss probability of new calls
    results.aggregate_loss_probability.push_back(abp); // Aggregate loss probability
    results.time.push_back(sim.sim_time); // Simulation time
}

void save_to_csv(SimResults &results, const std::string &filename) { // Save results to csv file
    std::ofstream file(filename); // Open file

    file << "time,ahr,anr,uh,un,lph,lpn,tlph,tlpn,tlpa,alp\n"; // Write header to file

    for (size_t i = 0; i < 998; ++i) {
        file << results.time[i] << ","
                << results.arrival_rate_handover[i] << ","
                << results.arrival_rate_newcall[i] << ","
                << results.utilisation_handover[i] << ","
                << results.utilisation_newcall[i] << ","
                << results.loss_probability_handover[i] << ","
                << results.loss_probability_newcall[i] << ","
                << results.theo_loss_probability_handover[i] << ","
                << results.theo_loss_probability_newcall[i] << ","
                << results.theo_aggregate_loss_probability[i] << ","
                << results.aggregate_loss_probability[i] << "\n";
               
    }
}
double factorial(int n) { // Calculate factorial of n
    double result = 1;
    for(int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    Simulation sim;
    SimResults results;
    int num_servers = 16; // Number of servers

    // Get user input for handover rate
    double handoverRate;
    std::cout << "Enter handover rate: ";
    std::cin >> handoverRate;

    // Find newcall rate such that abp < 0.02
    double newcallRate = sim.findNewcallRate(handoverRate, results);
    std::cout << " Updated Newcall rate: " << newcallRate << std::endl;

    // Get user input for newcall rate
    std::cout << "Enter newcall rate: ";
    std::cin >> newcallRate;

    // Find handover rate such that abp < 0.02
    double updatedHandoverRate = sim.findHandoverRate(newcallRate, results);
    std::cout << "Updated handover rate: " << updatedHandoverRate << std::endl;

    return 0;
}
