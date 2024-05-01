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
#include "projects/automated_warehouse/aw_message.h"

struct robot* robots;

/** message boxes from central control node to each robot */
struct messsage_box* boxes_from_central_control_node;
/** message boxes from robots to central control node */
struct messsage_box* boxes_from_robots;

// test code for central control node thread
void test_cnt(void *aux){
        int num_robots = *((int *)aux);
        bool *recieved = malloc(sizeof(bool) * num_robots); // TODO: destroy this
        for (int i = 0; i < num_robots; i++) {
                recieved[i] = false;
        }

        while(1){
                print_map(robots, num_robots);
                thread_sleep(1000);
                bool recieved_all = false;
                while (!recieved_all) {
                        for (int i = 0; i < num_robots; i++) {
                                if (recieved[i] == false) {
                                        struct message msg;
                                        int res = recv_message_from_robot(i, &msg);
                                        if (res == 0) {
                                                recieved[i] = true;
                                                // enum command cmd = pathfind_robot(&robots[i]);
                                                // set_cmd(&msg, cmd);
                                                send_message_to_robot(i, &msg);
                                        }
                                }
                        }
                }
                
                increase_step();
                unblock_threads();
        }
}

// test code for robot thread
void test_thread(void* aux){
        int idx = *((int *)aux);
        int test = 0;
        int res = 0;
        struct message msg;
        while(1){
                printf("thread %d : %d\n", idx, test++);
                
                res = recv_message_from_central_control_node(idx, &msg);
                if(res == 0){
                        moveRobot(&robots[idx], &msg);
                        send_message_to_central_control_node(idx, &msg);

                }
                thread_sleep((idx + 1) * 1000);
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

int initialize_robots(int num_robots, pair *task_pair) {
        robots = malloc(sizeof(struct robot) * num_robots); // TODO: destroy this
        tid_t* threads = malloc(sizeof(tid_t) * num_robots); // TODO: destroy this
        char * rnames = malloc(sizeof(char) * num_robots * 3); // TODO: destroy this
        int * robot_idxs = malloc(sizeof(int) * num_robots); // TODO: destroy this

        for (int i = 0; i < num_robots; i++) {
                robot_idxs[i] = i;
                snprintf(rnames, 3, "R%d", i + 1);
                setRobot(&robots[i], rnames, ROW_W, COL_W, 0, 0, &task_pair[i]);
                threads[i] = thread_create(rnames, 0, &test_thread, &robot_idxs[i]); // task 책임은 로봇한테 있다
                if (threads[i] == TID_ERROR) {
                        printf("Thread creation failed\n");
                        return -1;
                }
                rnames += 3;
        }

        return 0;
}

int initialize_central_control_node(int num_robots) {
        tid_t* cnt_thread = thread_create("CNT", 0, &test_cnt, &num_robots);
        if (cnt_thread == TID_ERROR) {
                printf("Thread creation failed\n");
                return -1;
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

        res = initialize_message_boxes(num_robots);
        if (res < 0) {
                printf("Message box initialization failed\n");
                return;
        }

        res = initialize_central_control_node(num_robots);
        if (res < 0) {
                printf("Central control node initialization failed\n");
                return;
        }

        res = initialize_robots(num_robots, task_pair);
        if (res < 0) {
                printf("Robot initialization failed\n");
                return;
        }
}