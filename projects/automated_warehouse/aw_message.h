#ifndef _PROJECTS_PROJECT1_AW_MESSAGE_H__
#define _PROJECTS_PROJECT1_AW_MESSAGE_H__

#define UPPER(num) ((num >> 16) & 0xFFFF)
#define LOWER(num) (num & 0xFFFF)

enum command {
    /** do nothing */
    CMD_NOP = 0,
    /** move up */
    CMD_UP,
    /** move down */
    CMD_DOWN,
    /** move left */
    CMD_LEFT,
    /** move right */
    CMD_RIGHT,
};

/**
 * For easy to implement, combine robot and central control node message
 * If you want to modify message structure, don't split it
 */
struct message {
    //
    // To central control node
    //
    /** current row of robot */
    int row;
    /** current column of robot */
    int col;
    /** current payload of robot */
    int current_payload;
    /** required paylod of robot */
    int required_payload;

    //
    // To robots
    //
    /** next command for robot */
    enum command cmd;
};

/** 
 * Simple message box which can receive only one message from sender
*/
struct messsage_box {
    /** check if the message was written by others */
    int dirtyBit;
    /** stored message */
    struct message msg;
};

/** message boxes from central control node to each robot */
extern struct messsage_box* boxes_from_central_control_node;
/** message boxes from robots to central control node */
extern struct messsage_box* boxes_from_robots;

int NUM_ROBOTS;

enum MessageEndpoint {
    ROBOT,
    CENTRAL_CONTROL
};

/* 메시지 전송 함수 */
int send_message(int robotIdx, struct message* msg, enum MessageEndpoint endpoint);

/* 메시지 수신 함수 */
int recv_message(int robotIdx, struct message* msg, enum MessageEndpoint direction);

/** initialize message boxes */
int initialize_message_boxes(int num_robots);

void set_cmd(struct message* msg, enum command cmd);

#endif