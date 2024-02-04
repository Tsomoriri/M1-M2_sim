// Single-server Loss system RESULts:: input rate  = 5.2, service rate = 100, number of servers = 16, runtime = 10000:-> Blocking probability = 0.00986538, Total server utilization = 0.786148, 
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <fstream>
// struct simulation declaration
struct Simulation;
// function declaration
float expon_mean(Simulation &sim);
float expon_service(Simulation &sim); 
void initialise(Simulation &sim, int c);
void timing(Simulation &sim);
void arrive(Simulation &sim , int c);
void depart(Simulation &sim);
void report(Simulation &sim, int c);
void update_time_avg_stats(Simulation &sim);
long double calculateProbability(Simulation &sim, int c);
void savedata(Simulation &sim);
// struct simulation definition
struct Simulation {
    double num_customer_required; //Number of customers to be served before the simulation ends
    int num_customer; //Number of customers who have entered the system
    int num_in_q; //Number of customers in the queue
    double blocked_in_q; //Number of customers blocked in the queue
    std::vector<int> server_status; //Server status (0 = idle, 1 = busy)
    double area_num_in_q; //Time-average number in the queue
    std::vector<double> area_server_status; //Time-average server status
    double mean_interarrival; //Mean interarrival time
    double mean_service; //Mean service time
    double sim_time; //Simulation time
    int sim_runtime; //Simulation runtime
    std::vector<double> time_last_event; //Time of last event
    std::vector<double> time_next_event; // Time of next event for each server
    int next_event_type; //Type of next event (1 = arrival, 2 = departure)
    double time_arrival[100]; //Time of arrival of the customer in the queue
    int q_limit; //Limit on the queue size
    double total_server_utilisation; //Total server utilisation
    std::vector<long double> T_blockingProbabilities; // Blocking probability vector
    std::vector<long double> T_blockingProbabilities_theoretical; //Theoretical Blocking probability vector
    std::vector<long double> arrival_rate; // arrival rate vector
    std::vector<long double> total_server_util; // total server utilisation vector

        void LoopArrivalRate(Simulation& sim){ //Loop arrival rate 0.01-0.1
            
        int c = 16;
   
        for (double i = 0.01 ; i <0.1 ; i+=0.001){
            sim.mean_interarrival = i; 
            initialise(sim, c); //Initialise the simulation
        while ( sim.sim_time < sim.sim_runtime) {
            //Invoke timing routine
            timing(sim); //Determine the necessary time for the next event
            update_time_avg_stats(sim); //Update time-average statistical accumulators
            //invoke event routine
            switch (sim.next_event_type) {
                case 0:
                    arrive(sim ,c); //next event is an arrival
                    break;
                default:
                    depart(sim); //next event is a departure
                    break;
            
            }
        } report(sim,c); //Invoke the report generator and end the simulation
        } savedata(sim); //Save the data to a csv file

        }

        void AnswerArrivalRate(Simulation& sim){ //Answer arrival rate 5.2
            
            int c = 16;
            sim.mean_interarrival = 5.2;
            initialise(sim, c); //Initialise the simulation
            while ( sim.sim_time < sim.sim_runtime) {
            //Invoke timing routine
            timing(sim); //Determine the necessary time for the next event
            update_time_avg_stats(sim); //Update time-average statistical accumulators
            //invoke event routine
            switch (sim.next_event_type) {
                case 0:
                    arrive(sim ,c); //next event is an arrival
                    break;
                default:
                    depart(sim); //next event is a departure
                    break;
            
            }
        } report(sim,c); //Invoke the report generator and end the simulation
         
        }

       
    };




float expon_mean(Simulation &sim) {
 static std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    float RAND = distribution(generator); //Generate a U(0,1) random variate
    return (-sim.mean_interarrival * log(RAND)); //Transform it to a U(0,1) random variate
}
float expon_service(Simulation &sim) {
  static std::default_random_engine generator; //Random number generator
    std::uniform_real_distribution<float> distribution(0.0, 1.0); //Uniform distribution

    float RAND = distribution(generator); //Generate a U(0,1) random variate
    return (-sim.mean_service * log(RAND)); //Transform it to a U(0,1) random variate
}

void initialise(Simulation &sim, int c) {
    sim.sim_time = 0.0;
    
    //Specify the number of events for the timing function
    std::cout << "Single-server Loss queueing system\n\n";
  
    sim.sim_runtime = 10000; //Specify the number of events for the timing function
  

    sim.num_customer_required = sim.mean_interarrival * sim.sim_runtime; //Specify the number of events for the timing function

    sim.mean_service = 100; //Specify the mean service time
    //Specify the queue limit
    sim.q_limit = c; //Specify the queue limit
    //initialise the state variables
    sim.num_in_q = 0; // no customers in the queue as queue =0 for all M/M/c/c
    sim.blocked_in_q = 0.0; //Initially, no blocked calls 
    sim.num_customer = 0; //Initially, no customers in the system
    sim.server_status.resize(c,0); //Idle all servers
    //initialise the statistical counters
    sim.area_num_in_q = 0.0; //Initialise the area under the number-in-queue function
    sim.area_server_status.resize(c,0.0);
    // Since no customers are present, the departure (service completion) event is eliminated from consideration
    // Initialize the event list
    sim.time_last_event.resize(c + 1 , 0.0); // Initialize all events to zero

    // Initialize the event list
    sim.time_next_event.resize(c + 1, 1.0e+30); // Initialize all events to a large value
    sim.time_next_event[0] = sim.sim_time + expon_mean(sim);//determine the next arrival time
    // Since no customers are present, the departure (service completion) event is eliminated from consideration
    
    

}

void timing(Simulation &sim){
    // Determine the next event type
    double min_time_next_event = 1.0e+30;
    for (int i = 0; i < sim.time_next_event.size(); i++) {
        if (sim.time_next_event[i] <= min_time_next_event) {
            sim.next_event_type = i;
            min_time_next_event = sim.time_next_event[i]; //Determine the next event type (0=arrival, 1-15=departure)
            
        } 
    }

    sim.time_last_event[sim.next_event_type] = sim.sim_time; // Update time since last event occurred
    sim.sim_time = sim.time_next_event[sim.next_event_type]; // Advance the simulation time
}

void arrive(Simulation &sim, int c){
    //Schedule next arrival
    sim.time_next_event[0] = sim.sim_time + expon_mean(sim);
    //Find the first idle server from the server status vector
    int idle_server = -1;
    int count = 0; // count the number of busy servers
    for (int i =0; i < sim.server_status.size(); i++) {
        if (sim.server_status[i] == 0) {
            idle_server = i; //Found idle server
            
            break;
        }
    }
    if (idle_server != -1) {
        // An idle server was found, so arriving customer has a delay of zero.
        sim.server_status[idle_server] = 1; //Mark the idle server as busy
        sim.time_next_event[idle_server] = sim.sim_time + expon_service(sim); //Schedule a departure (service completion)
        sim.num_customer++; //Increment the number of customers in the queue
       
    } else if (sim.num_in_q == sim.q_limit) {
      
        return;
    } else {
      
        sim.blocked_in_q++; //Increment the number of customers blocked
        // There is still room in the queue, so store the time of arrival of the arriving customer at the (new) end of time_arrival
        sim.time_arrival[sim.num_in_q] = sim.sim_time; //Increment the number of customers in the queue
        for (int i =0; i < sim.server_status.size(); i++) { // count the number of busy servers
        if (sim.server_status[i] == 1) { //Found idle server
            count++; // increment the number of busy servers
        }
    } 
        long double T_blockingProbability = calculateProbability(sim, count); // calculate the blocking probability
        sim.T_blockingProbabilities_theoretical.push_back(T_blockingProbability); // push the blocking probability to the vector

    }


}

void depart(Simulation &sim) {
    int server = sim.next_event_type; // Server from which a customer is departing
    double delay;

    // Check if there are customers in the queue, M/M/c/c has no queue so check if the server is busy, sim.num_in_q ==0 always.
    if (sim.num_in_q > 0) { 
        // The queue is nonempty, so decrement the number of customers in queue
        sim.num_in_q--;

        // Compute the delay of the customer who is beginning service and update the total delay accumulator
        delay = sim.sim_time - sim.time_arrival[1];
        sim.area_num_in_q += sim.num_in_q * (sim.sim_time - sim.time_last_event[server]);
        // Increment the number of customers delayed, and schedule departure
        sim.num_customer++;
        sim.time_next_event[server] = sim.sim_time + expon_service(sim);

        // Move each customer in queue (if any) up one place
        for (int i = 1; i <= sim.num_in_q; ++i) {
            sim.time_arrival[i] = sim.time_arrival[i + 1];
        }
    } else {
        //  If The queue is empty so make the server idle and eliminate the departure (service completion) event from consideration
        sim.server_status[server] = 0;
        sim.time_next_event[server] = 1.0e+30;
    }
}


void update_time_avg_stats(Simulation &sim){
    int server = sim.next_event_type; // Server from which a customer is departing
    double time_since_last_event;
    //Compute time since last event, and update last-event-time- marker
    time_since_last_event = sim.sim_time - sim.time_last_event[server];
    //Update area under number-in-queue function

    //Update area under server-busy indicator function
    for (int i = 0; i < sim.server_status.size(); i++) {
    sim.area_server_status[i] += sim.server_status[i] * time_since_last_event;
}
}

void report(Simulation &sim, int c){
    //Compute and write estimates of desired measures of performance
    
    std::cout << "Utilisation of Each server: ";
    for (int i = 0; i < sim.area_server_status.size(); i++) {
        std::cout <<  "\n";
        std::cout << i << "->";
        std::cout << sim.area_server_status[i] / sim.sim_time << "\n";
        sim.total_server_utilisation += sim.area_server_status[i]; 
    }
    std::cout << "\n";
    std::cout << "Blocking probabilities theoretical: ";
    std::cout << sim.T_blockingProbabilities_theoretical[0] << "->";
    std::cout << "\n";
    std::cout << "Total server utilization : " << sim.total_server_utilisation / sim.sim_time / c << "\n";
    std::cout << "Time simulation ended : " << sim.sim_time << "\n";
    std::cout << "Total Number of customers blocked: " << sim.blocked_in_q << "\n";
    std::cout << "Average customers blocked probabilty: " << sim.blocked_in_q / sim.num_customer_required << "\n";
    sim.T_blockingProbabilities.push_back(sim.blocked_in_q / sim.num_customer_required);
    sim.arrival_rate.push_back(sim.mean_interarrival);
    sim.total_server_util.push_back(sim.total_server_utilisation / sim.sim_time / c);

 
}

float factorial(int n) { // calculate the factorial of a number
    float result = 1;
    for(int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

long double calculateProbability(Simulation &sim, int c){ // calculate the blocking probability theoretically
    long double rho = sim.mean_interarrival / sim.mean_service;
    long double probability_numerator = pow(rho, c) / factorial(c);
   
    long double probability_denominator = 0.00f;
    for (int k = 0; k <= c; k++) {
        probability_denominator += pow(rho, k) / factorial(k);
    }
    long double p = probability_numerator / (probability_denominator);

    return p;
}

void savedata(Simulation &sim){
    std::string filename = "mmcc_find.csv";
    std::ofstream file(filename);

    // Write the header
    file << "Arrival Rate,Blocked Calls Probability,Theoretical Blocking Probability,Server Utilisation\n";

    // Write the data
    for (size_t i = 0; i < sim.arrival_rate.size(); ++i) {
        file << sim.arrival_rate[i] << ","
             << sim.T_blockingProbabilities[i] << ","
             << sim.T_blockingProbabilities_theoretical[i] << ","
             << sim.total_server_util[i] << "\n";
    }
}


int main() {
    Simulation sim;
    int c = 16;
    char answer;
    std::cout << "Multi-server Loss queueing system\n\n";
    std::cout << "Loop Arrival Rate (Y/N): ";
    std::cin >> answer;
    
    if (answer == 'Y') {
        sim.LoopArrivalRate(sim);
    }

    std::cout << "Answer Input Rate (Y/N): ";
    std::cin >> answer;
    if (answer == 'Y') {
        sim.AnswerArrivalRate(sim);
    }

    return 0;
}