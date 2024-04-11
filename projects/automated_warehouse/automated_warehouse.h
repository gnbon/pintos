#ifndef __PROJECTS_AUTOMATED_WAREHOUSE_H__
#define __PROJECTS_AUTOMATED_WAREHOUSE_H__

typedef struct {
    unsigned char cargo;
    char loading_dock;
} pair;

void test_cnt(void);

void test_thread(void* aux);

int parse_args(char *task_list, pair *task_pair, int max_count);

int initialize_robot(int num_robots, pair *task_pair);

void run_automated_warehouse(char **argv);

#endif 
