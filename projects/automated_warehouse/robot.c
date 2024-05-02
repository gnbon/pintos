#include "projects/automated_warehouse/robot.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

#define DEBUG(...) printf(__VA_ARGS__)

/**
 * A function setting up robot structure
 */
void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload, pair* task) {
    DEBUG("setRobot(%s)\n", name);
    DEBUG("row: %d, col: %d, required_payload: %d, current_payload: %d\n", row, col, required_payload, current_payload);
    _robot->name = name;
    _robot->row = row;
    _robot->col = col;
    _robot->required_payload = required_payload;
    _robot->current_payload = current_payload;
    _robot->task = task;
    DEBUG("_robot->name: %s, _robot->row: %d, _robot->col: %d, _robot->required_payload: %d, _robot->current_payload: %d\n", _robot->name, _robot->row, _robot->col, _robot->required_payload, _robot->current_payload);
    DEBUG("task->cargo: %c, task->loading_dock: %c\n", task->cargo, task->loading_dock);
    DEBUG("_robot->task->cargo: %c, _robot->task->loading_dock: %c\n", _robot->task->cargo, _robot->task->loading_dock);
}


int moveRobot(struct robot* robots, int idx, int number_of_robots, struct message* msg) {
    struct robot* robot = &robots[idx];
    DEBUG("moveRobot(%s)\n", robot->name);
    enum command required_cmd = msg->cmd;
    msg->required_payload = required_cmd;
    int* cur = &msg->current_payload;

    msg->row = robot->row;
    msg->col = robot->col;

    int new_row = robot->row;
    int new_col = robot->col;

    DEBUG("required_cmd: %d\n", required_cmd);
    switch (required_cmd) {
        case CMD_UP:
            new_row--;
            break;
        case CMD_DOWN:
            new_row++;
            break;
        case CMD_LEFT:
            new_col--;
            break;
        case CMD_RIGHT:
            new_col++;
            break;
        case CMD_NOP:
            *cur = CMD_NOP;
            return 0;
        default:
            return -1; // Invalid command
    }
    DEBUG("new_row: %d, new_col: %d\n", new_row, new_col);
    // Check if the new position is within the map boundaries
    if (new_row < ROW_MIN || new_row >= ROW_MAX || new_col < COL_MIN || new_col >= COL_MAX) {
        *cur = CMD_NOP;
        return 0;
    }

    char new_pos = map_draw_default[new_row][new_col];

    // Check if the new position is 'X'
    if (new_pos == 'X') {
        *cur = CMD_NOP;
        return 0;
    }

    // Check if the new position is '0', '2', '3', etc.
    if (new_pos >= '0' && new_pos <= '9') {
        if (robot->task->cargo != new_pos) {
            *cur = CMD_NOP;
            return 0;
        }
    }

    // Check if the new position is 'A', 'B', 'C', etc. (excluding 'W' and 'S')
    if (new_pos >= 'A' && new_pos <= 'Z' && new_pos != 'W' && new_pos != 'S') {
        if (robot->task->loading_dock != new_pos) {
            *cur = CMD_NOP;
            return 0;
        }
    }

    // Check another robot is already there
    for (int robotIdx = 0; robotIdx < number_of_robots; robotIdx++){
        struct robot* __robot = &robots[robotIdx];
        if (__robot->col == new_col && __robot->row == new_row){
            *cur = CMD_NOP;
            return 0;
        }
    }
                    
    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;
    *cur = required_cmd;
    msg->row = new_row;
    msg->col = new_col;

    if (map_draw_default[robot->row][robot->col] == robot->task->cargo) {
            robot->task->is_loaded = true;
    }
    
    return 0;
}