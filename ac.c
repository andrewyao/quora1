#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define VALID        1 
#define INVALID      0
#define UNDEFINED    255

typedef unsigned char uchar;
typedef char bool;

typedef enum RoomTypeEnum {
    BASIC,
    TOP_LEFT,
    TOP_EDGE,
    TOP_RIGHT,
    LEF_EDGE,
    RIG_EDGE,
    BOT_LEFT,
    BOT_EDGE,
    BOT_RIGHT,
    IGNORE
} RoomType;

// A linked list of steps, representing a path.
typedef struct StepStruct {
    uchar position;
    struct StepStruct * next;
} Step;

// A data structure for the problem.
typedef struct DuctStruct {
    uchar  height;      // width of the data center.
    uchar  width;       // height of the data center.
    uchar* mask;        // a 1D array of masks, 1 means the room has been visited or those we do not own.
    Step*  tip;         // a linked list of steps, last position first.
    uchar  max_length;  // how many steps needed to complete the path.
    uchar  delta;       // how many steps to go in a given solution.
    uchar  start;       // starting position.
    uchar  end;         // ending position.
    RoomType* rooms;    // a 1D array of all the room types.
    bool special;       // true if the starting position is an edge room.
} Duct;

/**
 * Step related functions.
 */
Step* step_init(uchar position) {
    Step* step = malloc(sizeof(Step));
    if (NULL != step) {
        step->position = position;
    }
    return step;
}

char step_dir(Step* step, Step* prev, uchar width) {
    if (NULL == prev) {
        return '*';
    }
    uchar curr_pos = step->position;
    uchar prev_pos = prev->position;
    if (curr_pos - prev_pos == width) {
        return '^'; 
    } else if (prev_pos - curr_pos == width) {
        return 'v';
    } else if (curr_pos - prev_pos == 1) {
        return '<';
    } else if (prev_pos - curr_pos == 1) {
        return '>';
    } else {
        return '*';
    }
}

void step_destroy(Step* step) {
    if (NULL != step) {
        free(step);
        step = NULL;
    }
}

/**
 * Duct related functions.
 */
void duct_read(Duct* duct) {

    int width = 0;
    int height = 0;

    if (scanf("%d", &(width)) != 1 ||
        scanf("%d", &(height)) != 1) {
        printf("Unable to read width or height.\n");
        exit(1); 
    }

    if (width <= 0 || height<= 0) {
        printf("The width or height is invalid: %d %d.\n", width, height);
        exit(1);
    }

    if (width == 1 || height == 1) {
        printf("The room is too small.\n");
        exit(1);
    }

    duct->width = width;
    duct->height = height;
    duct->rooms = malloc(sizeof(RoomType) * duct->width * duct->height);
    duct->mask  = malloc(sizeof(uchar) * duct->width * duct->height);

    if (NULL == duct->rooms || NULL == duct->mask) {
        printf("Unable to allocate memory\n");
        exit(1);
    } 

    RoomType* rooms = duct->rooms;
    uchar ignore_count = 0;
    int n = 0;
    for (uchar i = 0, x = 0, y = 0; scanf("%d", &n) == 1; ++i) {

        if (n == 1) {
            duct->mask[i] = 1;

            rooms[i] = IGNORE;
            ignore_count++;
        } else {
            duct->mask[i] = 0;

            rooms[i] = BASIC;
            if ((0 < x && x < duct->width - 1)) {
                if (0 == y) {
                    rooms[i] = TOP_EDGE;
                } else if (y == duct->height - 1) {
                    rooms[i] = BOT_EDGE;
                }
            } else if (0 < y && y < duct->height - 1) {
                if (0 == x) {
                    rooms[i] = LEF_EDGE;
                } else if (x == duct->width - 1) {
                    rooms[i] = RIG_EDGE;
                }
            } else if (0 == x) {
                if (0 == y) {
                    rooms[i] = TOP_LEFT;
                } else if (y == duct->height - 1) {
                    rooms[i] = BOT_LEFT;
                }
            } else if (x == duct->width - 1) {
                if (0 == y) {
                    rooms[i] = TOP_RIGHT;
                } else if (y == duct->height - 1) {
                    rooms[i] = BOT_RIGHT;
                }
            }

            if (n == 0) {
                // normal condition.
            } else if (n == 2) {
                if (duct->start == UNDEFINED) {
                    duct->start = i;
                    if (x == 0 || y == 0 || x == duct->width - 1 || y == duct->height - 1) {
                        // starting room is on the edge.
                        duct->special = 1;
                    }
                } else {
                    printf("The start room has already been initialized.\n");
                    exit(1);
                }
            } else if (n == 3) {
                if (duct->end == UNDEFINED) {
                    duct->end = i;
                } else {
                    printf("The end room has already been initialized.\n");
                    exit(1);
                }
            } else {
                printf("Invalid input.\n");
                exit(1);
            }
        }

        if (++x == duct->width) {
            // start the next row.
            x = 0;
            y++;
        }
    }
    // the max length of a good duct is fixed.
    duct->max_length = duct->width * duct->height - ignore_count;
    duct->delta = duct->max_length;
}

/**
 * Pushes a step into the path.
 */
void duct_push(Duct* duct, Step* step) {
    if (duct->mask[step->position] == 0) {
        step->next = duct->tip;
        duct->mask[step->position] = 1;
        duct->tip = step;
        duct->delta -= 1;
    }
}

/**
 * Pops a step from the path.
 */
Step* duct_pop(Duct* duct) {
    Step* step = duct->tip;
    if (NULL != step) {
        duct->mask[step->position] = 0;
        duct->tip = step->next;
        duct->delta += 1;
        step->next = NULL;
        return step;
    } else {
        return NULL;
    }
}

Duct* duct_init() {
    Duct* duct = malloc(sizeof(Duct));
    if (NULL != duct) {
        duct->width = 0;
        duct->height = 0;
        duct->tip = NULL;
        duct->start = UNDEFINED;
        duct->end = UNDEFINED;
        duct->mask = NULL;
        duct->rooms = NULL;

        duct_read(duct);

        Step* step = step_init(duct->start);
        if (NULL != step) {
            duct_push(duct, step);
        }
    }
    return duct;
}

void duct_destroy(Duct* duct) {
    if (NULL != duct) {
        Step* step = NULL;
        while(NULL != (step = duct_pop(duct))) {
            step_destroy(step);
        }
        free(duct->mask);
        free(duct->rooms);
        free(duct);
        duct = NULL;
    }
}

void duct_show(Duct* duct) {
    uchar area = duct->width * duct->height;
    char p[area];
    uchar i = 0;
    while (i < area) {
        p[i++] = ' ';
    }

    Step* step = duct->tip;
    Step* prev = NULL;

    while (NULL != step) {
        p[step->position] = step_dir(step, prev, duct->width);
        prev = step;
        step = step->next;
    }
    printf("---- \n");
    i = 0;
    RoomType* rooms = duct->rooms;
    while (i < area) {
        char curr = p[i];
        if (curr == ' ') {
            char c = '.';
            switch (rooms[i]) {
                case IGNORE:
                    c = '#';
                    break;
                default:
                    break;
            };
            printf("%c ", c);
        } else {
            printf("%c ", p[i]);
        }
        if (++i % duct->width == 0) {
            printf("\n");
        }
    }
}

int duct_search(Duct* duct);

int duct_next(Duct* duct, uchar i) {
    uchar delta = duct->delta;
    uchar end = duct->end;

    if (duct->mask[i] || (delta >> 1 && i == end)) {
        // room[i] is visited or we do not own the room or
        // room[i] is the end_room but we still need to cover more rooms.
        // (delta >> 1) means we are 2 or more steps away.
        return 0;
    } else if (delta == 1 && i == end) {
        // we have a solution
        return 1;
    } else {
        // we need to recursively drill down
        int result = 0;
        Step* step = step_init(i);
        if (NULL != step) {
            duct_push(duct, step);
            result = duct_search(duct);
            step = duct_pop(duct);
            step_destroy(step);
        }
        return result;
    }
}

/**
 * Checks to see if the end position is completely covered/blocked.  
 */
bool duct_check_end(Duct* duct) {

    if (!(duct->delta >> 1)) {
        return VALID;
    }

    uchar* mask = duct->mask;
    uchar width = duct->width;
    uchar end_room = duct->end;

    RoomType room_type = duct->rooms[end_room];

    switch(room_type) {

        case BASIC:
            if (mask[end_room - 1] &&
                mask[end_room - width] &&
                mask[end_room + 1] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case TOP_EDGE:
            if (mask[end_room - 1] &&
                mask[end_room + 1] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case BOT_EDGE:
            if (mask[end_room - 1] &&
                mask[end_room - width] &&
                mask[end_room + 1]) {

                return INVALID;
            }
            break;
        case LEF_EDGE:
            if (mask[end_room - width] &&
                mask[end_room + 1] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case RIG_EDGE:
            if (mask[end_room - 1] &&
                mask[end_room - width] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case TOP_LEFT:
            if (mask[end_room + 1] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case TOP_RIGHT:
            if (mask[end_room - 1] &&
                mask[end_room + width]) {

                return INVALID;
            }
            break;
        case BOT_LEFT:
            if (mask[end_room - width] &&
                mask[end_room + 1]) {

                return INVALID;
            }
            break;
        case BOT_RIGHT:
            if (mask[end_room - 1] &&
                mask[end_room - width]) {

                return INVALID;
            }
            break;
        default:
            break;
    }
    return VALID;
}

/**
 * Checks if the specified room is a dead end.
 */
bool duct_check_dead_end(Duct* duct, uchar position) {

    uchar* mask = duct->mask;

    if (mask[position] || position == duct->end) {
        // already visited or the room is the last room,
        // where the mask count doesn't apply.
        return VALID;
    } else {
        uchar exits = 0;
        uchar top, bot, pre, nex;
        uchar width = duct->width;

        RoomType room_type = duct->rooms[position];

        switch (room_type) {

            case BASIC:
                top = !mask[position - width];
                bot = !mask[position + width];
                pre = !mask[position - 1];
                nex = !mask[position + 1];
                exits = top + bot + pre + nex;
                break;
            case TOP_EDGE:
                bot = !mask[position + width];
                pre = !mask[position - 1];
                nex = !mask[position + 1];
                exits = bot + pre + nex;
                break;
            case LEF_EDGE:
                top = !mask[position - width];
                bot = !mask[position + width];
                nex = !mask[position + 1];
                exits = top + bot + nex;
                break;
            case RIG_EDGE:
                top = !mask[position - width];
                bot = !mask[position + width];
                pre = !mask[position - 1];
                exits = top + bot + pre;
                break;
            case BOT_EDGE:
                top = !mask[position - width];
                pre = !mask[position - 1];
                nex = !mask[position + 1];
                exits = top + pre + nex;
                break;
            case TOP_LEFT:
                bot = !mask[position + width];
                nex = !mask[position + 1];
                exits = bot + nex;
                break;
            case BOT_LEFT:
                top = !mask[position - width];
                nex = !mask[position + 1];
                exits = top + nex;
                break;
            case TOP_RIGHT:
                bot = !mask[position + width];
                pre = !mask[position - 1];
                exits = bot + pre;
                break;
            case BOT_RIGHT:
                top = !mask[position - width];
                pre = !mask[position - 1];
                exits = top + pre;
                break;
            default:
                break;
        }
        if (exits >> 1) {
            // exits >= 2
            // There are 2 or more entry/exits for this room, OK.
            return VALID;
        } else {
            // A neighbour cell has only one exit, 
            // we just created a dead end.
            return INVALID;
        }
    }
}

/**
 * Checks if there are dead ends for all of the previous room's neighbour.
 */
bool duct_check_previous_neighbor(Duct* duct) {

    Step* prev = duct->tip->next;
    if (NULL == prev) {
        return VALID;
    }
    uchar position = prev->position;

    bool result = 0;
    uchar width = duct->width;

    // Consider this scenario where we just reached a room.
    //
    // * * * ? ?
    // * . * x ?   <- x marks the current position, . marks an unvisited room.
    // * . * ? ?   <- * marks visited rooms.
    // * * * ? ?
    // We just created a dead end for the room on the left hand side of the previous neighbour.

    RoomType room_type = duct->rooms[position];
    switch (room_type) {

        case BASIC:
            result = 
                duct_check_dead_end(duct, position - width) && 
                duct_check_dead_end(duct, position + width) && 
                duct_check_dead_end(duct, position - 1) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case TOP_EDGE:
            result = 
                duct_check_dead_end(duct, position + width) &&
                duct_check_dead_end(duct, position - 1) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case LEF_EDGE:
            result = 
                duct_check_dead_end(duct, position - width) &&
                duct_check_dead_end(duct, position + width) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case RIG_EDGE:
            result = 
                duct_check_dead_end(duct, position - width) &&
                duct_check_dead_end(duct, position + width) &&
                duct_check_dead_end(duct, position - 1); 
            break;
        case BOT_EDGE:
            result = 
                duct_check_dead_end(duct, position - width) &&
                duct_check_dead_end(duct, position - 1) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case TOP_LEFT:
            result = 
                duct_check_dead_end(duct, position + width) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case BOT_LEFT:
            result = 
                duct_check_dead_end(duct, position - width) &&
                duct_check_dead_end(duct, position + 1); 
            break;
        case TOP_RIGHT:
            result = 
                duct_check_dead_end(duct, position + width) &&
                duct_check_dead_end(duct, position - 1);
            break;
        case BOT_RIGHT:
            result = 
                duct_check_dead_end(duct, position - width) &&
                duct_check_dead_end(duct, position - 1);
            break;
        default:
            break;
    }
    return result;
}

bool duct_check_edge(Duct* duct) {

    // special flag is true means the starting room is on an edge.
    if (!duct->special) {
        return VALID;
    }
    RoomType* rooms = duct->rooms;
    uchar position = duct->tip->position;
    uchar* mask = duct->mask;
    uchar width = duct->width;

    Step* prev = duct->tip->next;
    if (NULL == prev) {
        return VALID;
    }

    bool result = VALID;

    // we reached an edge.
    //
    // Consider this scenario where we just arrived at a top-edge cell.
    // 1. the previous cell is not a top-edge cell.
    // 2. the cell to the left is unvisited and ours.
    // 3. the cell to the right is unvisited and ours.
    //
    // ? . x . ?   <- x marks the current position, . marks an unvisited room.
    // ? ? x ? ?   <- x marks the previous position.
    // 
    // If the starting position is on an edge, clearly
    // this divides the area into two regions, hence an invalid scenario.
    //
    // Apply de-morgan law yields the following:
    // result = 
    // !1 (previous cell is a top-edge cell) || 
    // !2 (the cell to the left is visited or not ours (masked)) ||
    // !3 (the cell to the right is visited or not ours (masked));
    switch (rooms[position]) {

        case TOP_EDGE:
            result = (rooms[prev->position] == TOP_EDGE) ||
                mask[position - 1] ||  
                mask[position + 1];
            break;
        case BOT_EDGE:
            result = (rooms[prev->position] == BOT_EDGE) ||
                mask[position - 1] || 
                mask[position + 1];
            break;
        case LEF_EDGE:
            result = (rooms[prev->position] == LEF_EDGE) ||
                mask[position - width] || 
                mask[position + width];
            break;
        case RIG_EDGE:
            result = (rooms[prev->position] == RIG_EDGE) ||
                mask[position - width] || 
                mask[position + width];
            break;
        default:
            break;
    }

    return result;
}

/**
 * The main search algorithm starts here.
 */
int duct_search(Duct* duct) {

    if (!duct_check_previous_neighbor(duct)) {
        return 0;
    } else if (!duct_check_end(duct)) {
        return 0;
    }

    int result = 0;
    uchar position = duct->tip->position;
    uchar width = duct->width;

    RoomType room_type = duct->rooms[position];
    switch (room_type) {

        case BASIC:
            result  = duct_next(duct, position - 1);
            result += duct_next(duct, position + 1);
            result += duct_next(duct, position - width);
            result += duct_next(duct, position + width);
            break;
        case TOP_EDGE:
            if (duct_check_edge(duct)) {
                result  = duct_next(duct, position - 1);
                result += duct_next(duct, position + 1);
                result += duct_next(duct, position + width);
            }
            break;
        case LEF_EDGE:
            if (duct_check_edge(duct)) {
                result  = duct_next(duct, position + 1);
                result += duct_next(duct, position - width);
                result += duct_next(duct, position + width);
            }
            break;
        case RIG_EDGE:
            if (duct_check_edge(duct)) {
                result  = duct_next(duct, position - 1);
                result += duct_next(duct, position - width);
                result += duct_next(duct, position + width);
            }
            break;
        case BOT_EDGE:
            if (duct_check_edge(duct)) {
                result  = duct_next(duct, position - 1);
                result += duct_next(duct, position + 1);
                result += duct_next(duct, position - width);
            }
            break;
        case TOP_LEFT:
            result  = duct_next(duct, position + 1);
            result += duct_next(duct, position + width);
            break;
        case TOP_RIGHT:
            result  = duct_next(duct, position - 1);
            result += duct_next(duct, position + width);
            break;
        case BOT_LEFT:
            result  = duct_next(duct, position + 1);
            result += duct_next(duct, position - width);
            break;
        case BOT_RIGHT:
            result  = duct_next(duct, position - 1);
            result += duct_next(duct, position - width);
            break;
        default:
            break;
    }
    return result; 
}

int main(int argc, char** argv) {
    int result = 0;

    clock_t start = clock();
    Duct* duct = duct_init();
    if (NULL != duct) {
        result = duct_search(duct);
        duct_destroy(duct);
    }
    clock_t end = clock();

    printf("%i\n", result);
    printf("time elapsed:%ld\n", (long int) ((end-start) * 1000/CLOCKS_PER_SEC) );
    return 0;
}
