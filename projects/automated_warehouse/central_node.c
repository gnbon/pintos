#include "projects/automated_warehouse/central_node.h"
#include "projects/automated_warehouse/aw_manager.h"
#include "projects/automated_warehouse/aw_map.h"
#include "projects/automated_warehouse/aw_debug.h"

#define MAX_QUEUE_SIZE ((ROW_MAX+1) * (COL_MAX+1))

typedef struct {
    int row;
    int col;
} point;

typedef struct {
    point data[MAX_QUEUE_SIZE];
    int front;
    int rear;
} queue;

void init_queue(queue* queue) {
    queue->front = 0;
    queue->rear = 0;
}

bool is_empty(queue* queue) {
    return queue->front == queue->rear;
}

bool is_full(queue* queue) {
    return (queue->rear + 1) % MAX_QUEUE_SIZE == queue->front;
}

void enqueue(queue* queue, point point) {
    if (is_full(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->data[queue->rear] = point;
}

point dequeue(queue* queue) {
    if (is_empty(queue)) {
        point emptyPoint = {-1, -1};
        return emptyPoint;
    }
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    point point = queue->data[queue->front];
    return point;
}

int find_path(struct robot* robots_info, int robot_idx) {
    struct robot* robot = &robots_info[robot_idx];
    int current_row = robot->row;
    int current_col = robot->col;
    int required_payload = robot->required_payload;
    int current_payload = robot->current_payload;

    if (is_loaded(current_payload) && is_arrived(current_row, current_col, required_payload)) {
            return CMD_NOP;
    }
    
    INFO("cnt", "find_path row: %d, col: %d, required_payload: %x, current_payload: %d", current_row, current_col, required_payload, current_payload);
 
    int dir_row[] = {-1, 1, 0, 0};
    int dir_col[] = {0, 0, -1, 1};
    
    queue queue;
    init_queue(&queue);
    
    bool visited[ARR_ROW][ARR_COL] = {false};
    point prev[ARR_ROW][ARR_COL] = {0};
    
    point start = {current_row, current_col};
    enqueue(&queue, start);
    visited[start.row][start.col] = true;
    
    while (!is_empty(&queue)) {
        point current = dequeue(&queue);
        for (int i = 0; i < 4; i++) {
            int new_row = current.row + dir_row[i];
            int new_col = current.col + dir_col[i];

            if (is_out_of_bound(new_row, new_col)) {
                INFO("cnt", "skip(is_out_of_bound) row: %d, col: %d", new_row, new_col);
                continue;
            }

            if (is_wall(new_row, new_col)) {
                INFO("cnt", "skip(is_wall) row: %d, col: %d", new_row, new_col);
                continue;
            }
            
            if (visited[new_row][new_col]) {
                INFO("cnt", "skip(visited) row: %d, col: %d", new_row, new_col);
                continue;
            }

            if (is_another_payload(new_row, new_col, required_payload, current_payload)) {
                INFO("cnt", "skip(is_another_payload) row: %d, col: %d", new_row, new_col);
                continue;
            }

            if (is_hit_robot(new_row, new_col, robots_info, robot_idx)) {
                INFO("cnt", "skip(is_hit_robot) row: %d, col: %d", new_row, new_col);
                continue;
            }
            
            INFO("cnt", "enqueue row: %d, col: %d", new_row, new_col);
            point next = {new_row, new_col};
            enqueue(&queue, next);
            visited[new_row][new_col] = true;
            prev[new_row][new_col] = current;
            INFO("cnt", "prev row: %d, col: %d", prev[new_row][new_col].row, prev[new_row][new_col].col);
        }
    }
    
    point target;
    bool target_found = false;
    for (int i = 0; i < ARR_ROW; i++) {
        for (int j = 0; j < ARR_COL; j++) {
            if (!visited[i][j]) {
                continue;
            }
            if (is_loaded(current_payload)) {
                if (map_draw_default[i][j] == UPPER(required_payload)) {
                    target.row = i;
                    target.col = j;
                    target_found = true;
                    break;
                }
            } else {
                if (map_draw_default[i][j] == LOWER(required_payload)) {
                    target.row = i;
                    target.col = j;
                    target_found = true;
                    break;
                } 
            }
        }
    }
    INFO("cnt", "target row: %d, col: %d", target.row, target.col);

    if (!target_found) {
        return CMD_NOP;
    }

    while (prev[target.row][target.col].row != current_row || 
        prev[target.row][target.col].col != current_col) {
        target = prev[target.row][target.col];
    }
    
    robot->row = target.row;
    robot->col = target.col;
    if (map_draw_default[target.row][target.col] == LOWER(required_payload)) {
        robot->current_payload = 1;
    }
    INFO("cnt", "R%d move to row: %d, col: %d", robot_idx, target.row, target.col);

    if (target.row == current_row - 1) {
        return CMD_UP;
    } else if (target.row == current_row + 1) {
        return CMD_DOWN;
    } else if (target.col == current_col - 1) {
        return CMD_LEFT;
    } else if (target.col == current_col + 1) {
        return CMD_RIGHT;
    }
    return CMD_NOP;
}