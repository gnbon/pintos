#include "projects/automated_warehouse/central_node.h"
#include "projects/automated_warehouse/aw_manager.h"

#define MAX_QUEUE_SIZE (ROW_MAX * COL_MAX)

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
    queue->front = -1;
    queue->rear = -1;
}

bool isQueueEmpty(Queue* queue) {
    return queue->front == -1;
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
    if (queue->front == -1) {
        queue->front = queue->rear;
    }
}

Point dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        Point emptyPoint = {-1, -1};
        return emptyPoint;
    }
    Point point = queue->data[queue->front];
    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    }
    return point;
}

#define DEBUG(...) printf(__VA_ARGS__)

int find_path(struct robot* robot) {
    if (is_loaded(robot)) {
        if (map_draw_default[robot->row][robot->col] == robot->current_payload) {
            return CMD_NOP;
        }
    } else {
        if (map_draw_default[robot->row][robot->col] == robot->current_payload) {
            return CMD_NOP;
        }
    }
    
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    Queue queue;
    initQueue(&queue);
    
    bool visited[ROW_MAX][COL_MAX] = {false};
    Point prev[ROW_MAX][COL_MAX] = {0};
    
    Point start = {robot->row, robot->col};
    DEBUG("Start: %d %d\n", start.row, start.col);
    enqueue(&queue, start);
    visited[start.row][start.col] = true;
    
    while (!isQueueEmpty(&queue)) {
        Point current = dequeue(&queue);
        DEBUG("Current: %d %d\n", current.row, current.col);
        
        for (int i = 0; i < 4; i++) {
            int nr = current.row + dr[i];
            int nc = current.col + dc[i];
            DEBUG("nr: %d, nc: %d\n", nr, nc);
            if (nr < ROW_MIN || nr >= ROW_MAX || nc < COL_MIN || nc >= COL_MAX || map_draw_default[nr][nc] == 'X' || visited[nr][nc]) {
                DEBUG("nr < ROW_MIN || nr >= ROW_MAX || nc < COL_MIN || nc >= COL_MAX || map_draw_default[nr][nc] == 'X' || visited[nr][nc]\n");
                continue;
            }

            if (map_draw_default[nr][nc] >= '0' && map_draw_default[nr][nc] <= '9') {
                if (robot->current_payload != map_draw_default[nr][nc]) {
                    continue;
                }
            }

            if (map_draw_default[nr][nc] >= 'A' && map_draw_default[nr][nc] <= 'Z' && map_draw_default[nr][nc] != 'W' && map_draw_default[nr][nc] != 'S') {
                if (robot->current_payload != map_draw_default[nr][nc]) {
                    continue;
                }
            }
            
            Point next = {nr, nc};
            enqueue(&queue, next);
            visited[nr][nc] = true;
            prev[nr][nc] = current;
            DEBUG("current: %c\n", map_draw_default[nr][nc]);
        }
    }
    
    Point target;
    bool found = false;
    for (int i = 0; i < ROW_MAX; i++) {
        for (int j = 0; j < COL_MAX; j++) {
            if (visited[i][j] && (map_draw_default[i][j] == robot->current_payload || 
                                map_draw_default[i][j] == robot->required_payload)) {
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

    while (prev[target.row][target.col].row != robot->row || 
        prev[target.row][target.col].col != robot->col) {
        target = prev[target.row][target.col];
        DEBUG("Target: %d %d\n", target.row, target.col);
    }

    if (target.row == robot->row - 1) {
        DEBUG("UP\n");
        return CMD_UP;
    } else if (target.row == robot->row + 1) {
        DEBUG("DOWN\n");
        return CMD_DOWN;
    } else if (target.col == robot->col - 1) {
        DEBUG("LEFT\n");
        return CMD_LEFT;
    } else if (target.col == robot->col + 1) {
        DEBUG("RIGHT\n");
        return CMD_RIGHT;
    }
    DEBUG("NOP\n");
    return CMD_NOP;
}