
/*
 *
 * Simulation of Single Server Queueing System
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA,
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include "simlib.h"

/*******************************************************************************/

/*
 * Simulation Parameters
 */

#define RANDOM_SEED 5259140
#define NUMBER_TO_SERVE 50e6

#define SERVICE_TIME 1
#define ARRIVAL_RATE 0.1

#define BLIP_RATE 10000

/*******************************************************************************/

/*
 * main() uses various simulation parameters and creates a clock variable to
 * simulate real time. A loop repeatedly determines if the next event to occur
 * is a customer arrival or customer departure. In either case the state of the
 * system is updated and statistics are collected before the next
 * iteration. When it finally reaches NUMBER_TO_SERVE customers, the program
 * outputs some statistics such as mean delay.
 */

struct Data
{
    double utilization;
    double fraction_served;
    double mean_number;
    double mean_delay;
};


void simulate(double arrival_rate, struct Data *res, unsigned random_seed);

int main()
{
    char *filename = "q2.csv";
    FILE *fp = fopen(filename, "w+");
    fprintf(fp, "arrival_rate,utilization,fraction_served,mean_number,mean_delay");

    const double arrival_rate[9] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    const unsigned random_seed[10] = {5259140, 400069013, 1139780, 123456, 9817238, 7688798, 340289, 23487989, 19283798, 76877877};

    struct Data temp;

    double sum_utilization, sum_fraction_served, sum_mean_number, sum_mean_delay;

    for (int i = 0; i < 9; i++)
    {
        sum_utilization = sum_fraction_served = sum_mean_number = sum_mean_delay = 0;

        for (int j = 0; j < 10; j++) {
            simulate(arrival_rate[i], &temp, random_seed[j]);
            sum_utilization += temp.utilization;
            sum_fraction_served += temp.fraction_served;
            sum_mean_number += temp.mean_number;
            sum_mean_delay += temp.mean_delay;
        }
        
        fprintf(fp, "\n%f,%f,%f,%f,%f", arrival_rate[i], sum_utilization / 10, sum_fraction_served / 10,
                                        sum_mean_number / 10, sum_mean_delay/ 10);
    }

    fclose(fp);

    return 0;
}

void simulate(double arrival_rate, struct Data *res, unsigned random_seed)
{
    double clock = 0;

    int number_in_system = 0;
    double next_arrival_time = 0;
    double next_departure_time = 0;

    long int total_served = 0;
    long int total_arrived = 0;

    double total_busy_time = 0;
    double integral_of_n = 0;
    double last_event_time = 0;

    random_generator_initialize(random_seed);

    while (total_served < NUMBER_TO_SERVE)
    {
        if (number_in_system == 0 || next_arrival_time < next_departure_time)
        {
            clock = next_arrival_time;
            next_arrival_time = clock + exponential_generator((double)1 / arrival_rate);

            integral_of_n += number_in_system * (clock - last_event_time);
            last_event_time = clock;

            number_in_system++;
            total_arrived++;

            if (number_in_system == 1)
                next_departure_time = clock + SERVICE_TIME;
        }
        else
        {
            clock = next_departure_time;

            integral_of_n += number_in_system * (clock - last_event_time);
            last_event_time = clock;

            number_in_system--;
            total_served++;
            total_busy_time += SERVICE_TIME;

            if (number_in_system > 0)
                next_departure_time = clock + SERVICE_TIME;
        }
    }

    res->utilization = total_busy_time / clock;
    res->mean_delay = integral_of_n / total_served;
    res->mean_number = integral_of_n / clock;
    res->fraction_served = (double)total_served / total_arrived;
}
