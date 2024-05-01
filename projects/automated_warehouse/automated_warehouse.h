#ifndef __PROJECTS_AUTOMATED_WAREHOUSE_H__
#define __PROJECTS_AUTOMATED_WAREHOUSE_H__

#include "projects/automated_warehouse/robot.h"

void test_cnt(void* aux);

void test_thread(void* aux);

int parse_args(char *task_list, pair *task_pair, int max_count);

int initialize_robots(int num_robots, pair *task_pair);

int initialize_central_control_node(int num_robots);

void run_automated_warehouse(char **argv);

#endif 
