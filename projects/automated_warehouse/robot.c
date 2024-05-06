#include <stdio.h>

#include "projects/automated_warehouse/aw_debug.h"
#include "projects/automated_warehouse/robot.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

/**
 * A function setting up robot structure
 */
void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload) {
    if (_robot != NULL) {
        _robot->name = name;
    }
    _robot->row = row;
    _robot->col = col;
    _robot->required_payload = required_payload;
    _robot->current_payload = current_payload;
    INFO("robot", "robot %s is set at (%d, %d), required_load: %c, required_dock: %c current_payload: %d", name, row, col, LOWER(required_payload), UPPER(required_payload), current_payload);
}

void update_msg(struct message* msg, struct robot* robot) {
    msg->row = robot->row;
    msg->col = robot->col;
    msg->current_payload = robot->current_payload;
    msg->required_payload = robot->required_payload;
    INFO("robot", "update msg row: %d, col: %d, current_payload: %d, required_payload: %x", msg->row, msg->col, msg->current_payload, msg->required_payload);
}

int moveRobot(struct robot* robots, int robot_idx, int number_of_robots, struct message* msg) {
    struct robot* robot = &robots[robot_idx];
    update_msg(msg, robot);

    if (is_loaded(robot) && map_draw_default[robot->row][robot->col] == UPPER(robot->required_payload)) {
        return 0;
    }

    enum command cmd = msg->cmd;

    int new_row = robot->row;
    int new_col = robot->col;

    switch (cmd) {
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
            robot->nop_count++;
            break;
        default:
            ERROR("robot", "robot %s received invalid command %d", robot->name, cmd);
            return -1; // Invalid command
    }
    
    char new_pos = map_draw_default[new_row][new_col];

    if (is_out_of_bound(new_row, new_col)) {
        ERROR("robot", "error(is_out_of_bound) row: %d, col: %d", new_row, new_col);
        return -1;
    }

    // if (is_wall(new_row, new_col)) {
    //     ERROR("robot", "error(is_wall) row: %d, col: %d", new_row, new_col);
    //     ERROR("robot", "map[%d][%d] = %c", new_row, new_col, map_draw_default[new_row][new_col]);
    //     return -1;
    // }

    if (is_another_payload(new_row, new_col, robot->required_payload, robot->current_payload)) {
        ERROR("robot", "error(is_another_payload) row: %d, col: %d", new_row, new_col);
        return -1;
    }

    // if (is_hit_robot(new_row, new_col, robots, robot_idx)) {
    //     ERROR("robot", "error(is_hit_robot) row: %d, col: %d", new_row, new_col);
    //     return -1;
    // }
             
    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;
    INFO("robot", "new position: (%d, %d), current_payload: %d", new_row, new_col, robot->current_payload);
    
    if (new_pos == LOWER(robot->required_payload)) {
        robot->current_payload = 1;
    }

    update_msg(msg, robot);
    INFO("robot", "move %s to (%d, %d)", robot->name, new_row, new_col);
    return 0;
}