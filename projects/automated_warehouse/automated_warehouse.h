#ifndef __PROJECTS_AUTOMATED_WAREHOUSE_H__
#define __PROJECTS_AUTOMATED_WAREHOUSE_H__

#include "projects/automated_warehouse/robot.h"

void test_cnt(void);

void test_thread(void* aux);

int parse_args(char *task_list, pair *task_pair);

int initialize_robots(pair *task_pair);

int initialize_central_control_node();

void run_automated_warehouse(char **argv);

#endif 
