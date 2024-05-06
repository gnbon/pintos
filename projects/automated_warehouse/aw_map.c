#include "projects/automated_warehouse/aw_map.h"
#include "projects/automated_warehouse/aw_manager.h"
#include "projects/automated_warehouse/aw_debug.h"
#include "aw_map.h"

bool is_out_of_bound(int row, int col) {
    return row < 0 || row > ROW_MAX || col < 0 || col > COL_MAX;
}

bool is_wall(int row, int col) {
    return map_draw_default[row][col] == 'X';
}

bool is_loaded(int current_payload) {
    return current_payload == 1;
}

bool _is_another_load(int row, int col, short required_load) {
    return map_draw_default[row][col] >= '0' && map_draw_default[row][col] <= '9' && 
        map_draw_default[row][col] != required_load;
}

bool _is_another_dock(int row, int col, short required_dock) {
    return map_draw_default[row][col] >= 'A' && map_draw_default[row][col] <= 'Z' && 
        map_draw_default[row][col] != 'W' && map_draw_default[row][col] != 'S' && 
        map_draw_default[row][col] != required_dock;
}

bool is_another_payload(int new_row, int new_col, int required_payload, int current_payload) {
    if (is_loaded(current_payload)) {
        return _is_another_dock(new_row, new_col, UPPER(required_payload));
    }
    else {
        return _is_another_load(new_row, new_col, LOWER(required_payload));
    }
}


bool _is_waiting(int row, int col) {
    return map_draw_default[row][col] == 'W';
}

bool is_arrived(int row, int col, int required_payload) {
    short dock = UPPER(required_payload);
    return dock == map_draw_default[row][col];
}

bool is_hit_robot(int row, int col, struct robot* robots, int robot_idx) {
    struct robot* robot = &robots[robot_idx];
    int required_payload = robot->required_payload;

    for (int i = 0; i < NUM_ROBOTS; i++) {
        if (i == robot_idx) { // current_robot
            continue;
        }
        struct robot* other_robot = &robots[i];
        int other_robot_row = other_robot->row;
        int other_robot_col = other_robot->col;
        if (other_robot_row != row || other_robot_col != col) {
            continue;
        }
        if (_is_waiting(row, col) || is_arrived(row, col, required_payload)) {
            continue;
        }
        return true;
    }
    
    return false;
}