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
#include "projects/automated_warehouse/central_node.h"

struct robot* robots;
int num_robots;

#define DEBUG(...) printf(__VA_ARGS__)

/** message boxes from central control node to each robot */
struct messsage_box* boxes_from_central_control_node;
/** message boxes from robots to central control node */
struct messsage_box* boxes_from_robots;

// test code for central control node thread
void test_cnt(void){
        bool *received = malloc(sizeof(bool) * num_robots); // TODO: destroy this
        for (int i = 0; i < num_robots; i++) {
                received[i] = false;
        }

        while(1){
                DEBUG("print map\n");
                print_map(robots, num_robots);
                thread_sleep(100);
                bool received_all = false;
                while (!received_all) {
                        received_all = true;
                        for (int i = 0; i < num_robots; i++) {
                                DEBUG("Checking robot %d\n", i);
                                if (received[i]) {
                                        continue;
                                }
                                received_all = false;
                                // not received yet
                                struct message msg;
                                int res = recv_message_from_robot(i, &msg);

                                if (res == 0) {
                                        enum command cmd = find_path(&robots[i]);
                                        set_cmd(&msg, cmd);
                                        send_message_to_robot(i, &msg);
                                        received[i] = true;
                                } 
                                
                        }
                }
                for (int i = 0; i < num_robots; i++) {
                        received[i] = false;
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
                        moveRobot(robots, idx, num_robots, &msg);
                        send_message_to_central_control_node(idx, &msg);

                }
                block_thread();
                // thread_sleep((idx + 1) * 1000);
        }
}

int parse_args(char *task_list, pair *task_pair) {
        char *saveptr;
        char *token = strtok_r(task_list, ":", &saveptr);
        int count = 0;

        while (token != NULL) {
                char number = token[0]; // TODO : check if number is valid
                char letter = token[1]; // TODO: check if letter is valid

                if (count >= num_robots) {
                        printf("Too many robots\n");
                        return -1;
                }

                if (number < '0' || number > '9') {
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

                task_pair[count].cargo = number;
                task_pair[count].loading_dock = letter;
                count++;

                token = strtok_r(NULL, ":", &saveptr);
        }

        for (int i = 0; i < count; i++) {
                printf("%c%c\n", task_pair[i].cargo, task_pair[i].loading_dock);
        }

        return 0;
}

int initialize_robots(pair *task_pair) {
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

int initialize_central_control_node() {
        tid_t* cnt_thread = thread_create("CNT", 0, &test_cnt, NULL);
        if (cnt_thread == TID_ERROR) {
                printf("Thread creation failed\n");
                return -1;
        }

        return 0;
}

void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this

        num_robots = atoi(argv[1]);
        char *task_list = argv[2];

        pair* task_pair = malloc(sizeof(pair) * num_robots);
        if (task_pair == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }

        int res = parse_args(task_list, task_pair);
        if (res < 0) {
                printf("Parsing failed\n");
                return;
        }

        res = initialize_message_boxes(num_robots);
        if (res < 0) {
                printf("Message box initialization failed\n");
                return;
        }

        res = initialize_central_control_node();
        if (res < 0) {
                printf("Central control node initialization failed\n");
                return;
        }
        res = initialize_robots(task_pair);
        if (res < 0) {
                printf("Robot initialization failed\n");
                return;
        }
}