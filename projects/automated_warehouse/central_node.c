#include "projects/automated_warehouse/central_node.h"
#include "projects/automated_warehouse/aw_debug.h"
#include "projects/automated_warehouse/aw_manager.h"

#define MAX_QUEUE_SIZE ((ROW_MAX+1) * (COL_MAX+1))

typedef struct {
    int row;
    int col;
} Point;

typedef struct {
    Point data[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue* queue) {
    queue->front = 0;
    queue->rear = 0;
}

bool isQueueEmpty(Queue* queue) {
    return queue->front == queue->rear;
}

bool isQueueFull(Queue* queue) {
    return (queue->rear + 1) % MAX_QUEUE_SIZE == queue->front;
}

void enqueue(Queue* queue, Point point) {
    if (isQueueFull(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->data[queue->rear] = point;
}

Point dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        Point emptyPoint = {-1, -1};
        return emptyPoint;
    }
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    Point point = queue->data[queue->front];
    return point;
}
#define DEBUG(...) printf(__VA_ARGS__)

bool c_isOutOfBound(int row, int col) {
    return row < ROW_MIN || row > ROW_MAX || col < COL_MIN || col > COL_MAX;
}

bool c_isWall(int row, int col) {
    return map_draw_default[row][col] == 'X';
}

bool c_isAnotherCurrentPayload(int row, int col, int current_payload) {
    return map_draw_default[row][col] >= '0' && map_draw_default[row][col] <= '9' && 
        map_draw_default[row][col] != current_payload;
}

bool c_isAnotherRequiredPayload(int row, int col, int current_payload) {
    return map_draw_default[row][col] >= 'A' && map_draw_default[row][col] <= 'Z' && 
        map_draw_default[row][col] != 'W' && map_draw_default[row][col] != 'S' && 
        map_draw_default[row][col] != current_payload;
}

bool msg_is_loaded(struct message* msg) {
    return msg->current_payload > msg->required_payload;
}

// int find_path(struct robot* robot) {
int find_path(struct message* msg) {
    INFO("cnt", "find_path row: %d, col: %d, current_payload: %c, required_payload: %c", msg->row, msg->col, msg->current_payload, msg->required_payload);
    int row = msg->row;
    int col = msg->col;
    int current_payload = msg->current_payload;
    int required_payload = msg->required_payload;
    
    if (msg_is_loaded(msg) && map_draw_default[row][col] == current_payload) {
            return CMD_NOP;
    }
    
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    Queue queue;
    initQueue(&queue);
    
    bool visited[ROW_MAX+1][COL_MAX+1] = {false};
    Point prev[ROW_MAX+1][COL_MAX+1] = {0};
    
    Point start = {row, col};
    enqueue(&queue, start);
    visited[start.row][start.col] = true;
    
    while (!isQueueEmpty(&queue)) {
        Point current = dequeue(&queue);
        for (int i = 0; i < 4; i++) {
            int nr = current.row + dr[i];
            int nc = current.col + dc[i];
            
            if (c_isOutOfBound(nr, nc)) {
                INFO("cnt", "skip(isOutOfBound) row: %d, col: %d", nr, nc);
                continue;
            }

            if (c_isWall(nr, nc)) {
                INFO("cnt", "skip(isWall) row: %d, col: %d", nr, nc);
                continue;
            }

            if (visited[nr][nc]) {
                INFO("cnt", "skip(visited) row: %d, col: %d", nr, nc);
                continue;
            }

            if (c_isAnotherCurrentPayload(nr, nc, current_payload)) {
                INFO("cnt", "skip(isAnotherCurrentPayload) row: %d, col: %d", nr, nc);
                continue;
            }

            if (c_isAnotherRequiredPayload(nr, nc, current_payload)) {
                INFO("cnt", "skip(isAnotherRequiredPayload) row: %d, col: %d", nr, nc);
                continue;
            }
            
            Point next = {nr, nc};
            enqueue(&queue, next);
            INFO("cnt", "enqueue row: %d, col: %d", nr, nc);
            visited[nr][nc] = true;
            prev[nr][nc] = current;
        }
    }
    
    Point target;
    bool found = false;
    for (int i = 0; i < ROW_MAX+1; i++) {
        for (int j = 0; j < COL_MAX+1; j++) {
            if (visited[i][j] && map_draw_default[i][j] == current_payload) {
                target.row = i;
                target.col = j;
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    if (!found) {
        return CMD_NOP;
    }

    while (prev[target.row][target.col].row != row || 
        prev[target.row][target.col].col != col) {
        target = prev[target.row][target.col];
    }

    if (target.row == row - 1) {
        return CMD_UP;
    } else if (target.row == row + 1) {
        return CMD_DOWN;
    } else if (target.col == col - 1) {
        return CMD_LEFT;
    } else if (target.col == col + 1) {
        return CMD_RIGHT;
    }
    return CMD_NOP;
}