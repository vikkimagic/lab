
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
#define ARRIVAL_RATE 1.1

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

#define N 20

struct Data
{
    double utilization;
    double fraction_served;
    double mean_number;
    double mean_delay;
};


void simulate(long int number_to_serve, struct Data *res, unsigned random_seed);

int main()
{
    const unsigned random_seed[10] = {5259140, 400069013, 1139780, 123456, 9817238, 7688798, 340289, 23487989, 19283798, 76877877};

    char *filename = "q3.csv";
    FILE *fp = fopen(filename, "w+");
    fprintf(fp, "length,utilization,fraction_served,mean_number,mean_delay");

    struct Data temp;

    double sum_utilization, sum_fraction_served, sum_mean_number, sum_mean_delay;

    for (int i = 0; i < N; i ++) {
        sum_utilization = sum_fraction_served = sum_mean_number = sum_mean_delay = 0;

        for (int j = 0; j < 10; j ++) {
            simulate((i + 1) * 10000, &temp, random_seed[10]);
            sum_utilization += temp.utilization;
            sum_fraction_served += temp.fraction_served;
            sum_mean_number += temp.mean_number;
            sum_mean_delay += temp.mean_delay;
        }

        fprintf(fp, "\n%d,%f,%f,%f,%f", (i + 1) * 10000, sum_utilization / 10, sum_fraction_served / 10,
                                        sum_mean_number / 10, sum_mean_delay/ 10);
    }

    return 0;
}

void simulate(long int number_to_serve, struct Data *res, unsigned random_seed)
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

    while (total_served < number_to_serve)
    {
        if (number_in_system == 0 || next_arrival_time < next_departure_time)
        {
            clock = next_arrival_time;
            next_arrival_time = clock + exponential_generator((double)1 / ARRIVAL_RATE);

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
