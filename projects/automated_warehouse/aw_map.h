#ifndef _PROJECTS_PROJECT1_AW_MAP_H__
#define _PROJECTS_PROJECT1_AW_MAP_H__

#include "projects/automated_warehouse/robot.h"

#endif

bool _is_another_load(int row, int col, short required_load);

bool _is_another_dock(int row, int col, short required_dock);

bool _is_waiting(int row, int col);

bool is_out_of_bound(int row, int col);

bool is_wall(int row, int col);

bool is_loaded(int current_payload);

bool is_another_payload(int new_row, int new_col, int required_payload, int current_payload);

bool is_arrived(int row, int col, int required_payload);

bool is_hit_robot(int row, int col, struct robot *robots, int robot_idx);
