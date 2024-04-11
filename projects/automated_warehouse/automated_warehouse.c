#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#include "projects/automated_warehouse/automated_warehouse.h"
#include "projects/automated_warehouse/aw_manager.h"

struct robot* robots;

// test code for central control node thread
void test_cnt(void){
        while(1){
                print_map(robots, 4);
                thread_sleep(1000);
                block_thread();
        }
}

// test code for robot thread
void test_thread(void* aux){
        int idx = *((int *)aux);
        int test = 0;
        while(1){
                printf("thread %d : %d\n", idx, test++);
                thread_sleep(idx * 1000);
        }
}

int parse_args(char *task_list, pair *task_pair, int max_count) {
        char *saveptr;
        char *token = strtok_r(task_list, ":", &saveptr);
        int pair_idx = 0;

        task_pair = malloc(sizeof(pair) * max_count);
        if (task_pair == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }

        while (token != NULL) {
                char number = token[0] - '0'; // TODO : check if number is valid
                char letter = token[1]; // TODO: check if letter is valid

                if (pair_idx >= max_count) {
                        printf("Too many robots\n");
                        return -1;
                }

                if (number < 0 || number > 9) {
                        printf("Invalid number format: %s\n", token);
                        return -2; 
                }
                
                if (letter < 'A' || letter > 'Z') {
                        printf("Invalid letter format: %s\n", token);
                        return -3;
                }

                if (token[2]) {
                        printf("Invalid format: %s\n", token);
                        return -4;
                }

                task_pair[pair_idx].cargo = number;
                task_pair[pair_idx].loading_dock = letter;
                pair_idx++;

                token = strtok_r(NULL, ":", &saveptr);
        }

        for (int i = 0; i < pair_idx; i++) {
                printf("%d%c\n", task_pair[i].cargo, task_pair[i].loading_dock);
        }

        return 0;
}

int initialize_robot(int num_robots, pair *task_pair) {
        struct robot* robots = malloc(sizeof(struct robot) * num_robots);
        tid_t* threads = malloc(sizeof(tid_t) * num_robots);

        for (int i = 0; i < num_robots; i++) {
                char rname[3];
                int idx = i + 1;
                snprintf(rname, 3, "R%d", i + 1);
                setRobot(&robots[i], rname, ROW_W, COL_W, 0, 0); // TODO : set robot position and payload
                threads[i] = thread_create(rname, 0, &test_thread, &idx);
                if (threads[i] == TID_ERROR) {
                        printf("Thread creation failed\n");
                        return -1;
                }
        }

        return 0;
}

void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this

        int num_robots = atoi(argv[1]);
        char *task_list = argv[2];

        pair* task_pair = NULL;
        int res = parse_args(task_list, task_pair, num_robots);
        if (res < 0) {
                printf("Parsing failed\n");
                return;
        }

        res = initialize_robot(num_robots, task_pair);
        if (res < 0) {
                printf("Robot initialization failed\n");
                return;
        }

        // if you want, you can use main thread as a central control node
        
}