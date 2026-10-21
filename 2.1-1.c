#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ROWS 165
#define MAX_COLS 165
#define MAX_ROBOTS 55
#define MAX_PATH_LENGTH 30000

typedef struct{
    int row, column;
}point;
typedef struct{
    point data_array[MAX_PATH_LENGTH];
    int top_index;
}path_stack;

void initialize_stack(path_stack *stack_ptr) 
{
    stack_ptr->top_index = -1;
}
bool is_stack_empty(path_stack *stack_ptr) 
{
    return stack_ptr->top_index ==-1;
}
void stack_push(path_stack *stack_ptr, point point_item) 
{
    if(stack_ptr->top_index < MAX_PATH_LENGTH-1) 
    {
        stack_ptr->data_array[++(stack_ptr->top_index)] = point_item;
    }
}
void stack_pop(path_stack *stack_ptr) 
{
    if(!is_stack_empty(stack_ptr)) 
    {
        stack_ptr->top_index--;
    }
}

point stack_peek(path_stack *stack_ptr) 
{
    return stack_ptr->data_array[stack_ptr->top_index];
}
int get_stack_size(path_stack *stack_ptr) 
{
    return stack_ptr->top_index + 1;
}

typedef struct{
    int robot_id;
    int current_row, current_column;
    int robot_health;
    int win_counter;
    char robot_status[20];
    path_stack movement_stack;
}robot_info;

int row_directions[] = {-1, 0, 1, 0};
int column_directions[] = {0, -1, 0, 1};

char exploration_grid[MAX_ROWS][MAX_COLS];
bool is_cell_explored[MAX_ROBOTS][MAX_ROWS][MAX_COLS];
int cell_occupant[MAX_ROWS][MAX_COLS];
robot_info robot_array[MAX_ROBOTS];

bool simulate_round(int total_rows, int total_cols, int total_robots, int evacuation_row, int evacuation_column, int *active_robot_count) 
{
    bool any_robot_moved = false;

    for(int i=0 ; i<total_robots ; i++) 
    {
        if(strcmp(robot_array[i].robot_status, "ACTIVE") != 0) 
            continue;

        int current_row = robot_array[i].current_row;
        int current_column = robot_array[i].current_column;
        // clean occupant
        cell_occupant[current_row][current_column] = 0;

        // find next(up -> left -> down -> right)
        int next_row = -1, next_column = -1;
        bool found_new_cell = false;

        for(int d=0 ; d<4 ; d++) 
        {
            int neighbor_row = current_row+row_directions[d];
            int neighbor_column = current_column+column_directions[d];

            if(neighbor_row>=1 && neighbor_row<=total_rows && neighbor_column>=1 && neighbor_column<=total_cols) 
            {
                if(exploration_grid[neighbor_row-1][neighbor_column-1] != '#') 
                {
                    if(!is_cell_explored[i][neighbor_row][neighbor_column]) 
                    {
                        next_row = neighbor_row;
                        next_column = neighbor_column;
                        found_new_cell = true;
                        break;
                    }
                }
            }
        }

        if(found_new_cell) // push
        {
            stack_push(&robot_array[i].movement_stack, (point){next_row, next_column});
            is_cell_explored[i][next_row][next_column] = true;
            robot_array[i].current_row = next_row;
            robot_array[i].current_column = next_column;
            any_robot_moved = true;
        } 
        else // pop
        {
            if(get_stack_size(&robot_array[i].movement_stack) > 1) 
            {
                stack_pop(&robot_array[i].movement_stack);
                point previous_point = stack_peek(&robot_array[i].movement_stack);
                robot_array[i].current_row = previous_point.row;
                robot_array[i].current_column = previous_point.column;
                any_robot_moved = true;
            } 
            else // back to start -> NO PATH
            {
                strcpy(robot_array[i].robot_status, "NO PATH");
                (*active_robot_count)--;
                continue;
            }
        }

        // Check the point reached
        if(robot_array[i].current_row == evacuation_row && robot_array[i].current_column == evacuation_column) 
        {
            strcpy(robot_array[i].robot_status, "WIN");
            (*active_robot_count)--;
            continue;
        }

        // crush
        int target_row = robot_array[i].current_row;
        int target_column = robot_array[i].current_column;
        int other_robot_id = cell_occupant[target_row][target_column];

        if(other_robot_id!=0 && other_robot_id!=robot_array[i].robot_id) 
        {
            int j = other_robot_id-1;
            if(strcmp(robot_array[j].robot_status, "ACTIVE") == 0) 
            {
                if(robot_array[i].robot_health > robot_array[j].robot_health) 
                {
                    strcpy(robot_array[j].robot_status, "DIE");
                    (*active_robot_count)--;

                    robot_array[i].win_counter++;
                    robot_array[i].robot_health -= robot_array[i].win_counter;
                    if(robot_array[i].robot_health <= 0) 
                    {
                        strcpy(robot_array[i].robot_status, "DIE");
                        (*active_robot_count)--;
                    }
                } 
                else if(robot_array[i].robot_health < robot_array[j].robot_health) 
                {
                    strcpy(robot_array[i].robot_status, "DIE");
                    (*active_robot_count)--;

                    robot_array[j].win_counter++;
                    robot_array[j].robot_health -= robot_array[j].win_counter;
                    if(robot_array[j].robot_health <= 0) 
                    {
                        strcpy(robot_array[j].robot_status, "DIE");
                        (*active_robot_count)--;
                    }
                } 
                else 
                {
                    strcpy(robot_array[i].robot_status, "DIE");
                    strcpy(robot_array[j].robot_status, "DIE");
                    (*active_robot_count) -= 2;
                }
            }
        }

        if(strcmp(robot_array[i].robot_status, "ACTIVE") == 0) 
        {
            cell_occupant[robot_array[i].current_row][robot_array[i].current_column] = robot_array[i].robot_id;
        }
    }

    return any_robot_moved;
}

void initialize_robots_data(int total_robots) 
{
    for(int i=0 ; i<total_robots ; i++) 
    {
        robot_array[i].robot_id = i+1;
        scanf("%d %d %d", &robot_array[i].current_row, &robot_array[i].current_column, &robot_array[i].robot_health);
        robot_array[i].win_counter = 0;
        strcpy(robot_array[i].robot_status, "ACTIVE");
        
        // Start to push
        initialize_stack(&robot_array[i].movement_stack);
        stack_push(&robot_array[i].movement_stack, (point){robot_array[i].current_row, robot_array[i].current_column});
        
        is_cell_explored[i][robot_array[i].current_row][robot_array[i].current_column] = true;
        cell_occupant[robot_array[i].current_row][robot_array[i].current_column] = robot_array[i].robot_id;
    }
}

void print_result(int total_robots) 
{
    for(int i=0 ; i<total_robots ; i++) 
    {
        printf("Robot %d: %s", robot_array[i].robot_id, robot_array[i].robot_status);
        if(strcmp(robot_array[i].robot_status, "WIN")==0 || strcmp(robot_array[i].robot_status, "DIE")==0) 
        {
            printf("\nPath: ");
            int stack_length = get_stack_size(&robot_array[i].movement_stack);
            for(int k=0 ; k<stack_length ; k++) 
            {
                printf("(%d,%d)", robot_array[i].movement_stack.data_array[k].row, robot_array[i].movement_stack.data_array[k].column);
                if(k+1 < stack_length) 
                {
                    printf(" -> ");
                }
            }
        }
        if(i+1 < total_robots) 
        {
            printf("\n");
        }
    }
}

int main() 
{
    int total_rows, total_cols, total_robots;
    if(scanf("%d %d %d", &total_rows, &total_cols, &total_robots) != 3) 
        return 0;

    int evacuation_row = -1, evacuation_column = -1;
    for(int i=0 ; i<total_rows ; i++) 
    {
        scanf("%s", exploration_grid[i]);
        for(int j=0 ; j<total_cols ; j++) 
        {
            if(exploration_grid[i][j] == 'E') 
            {
                evacuation_row = i+1;
                evacuation_column = j+1;
            }
        }
    }

    memset(cell_occupant, 0, sizeof(cell_occupant));
    memset(is_cell_explored, 0, sizeof(is_cell_explored));
    initialize_robots_data(total_robots);

    int active_robot_count = total_robots;

    while(active_robot_count > 0) 
    {
        bool any_robot_moved = simulate_round(total_rows, total_cols, total_robots, evacuation_row, evacuation_column, &active_robot_count);
        if(!any_robot_moved) 
            break;
    }
    
    print_result(total_robots);

    return 0;
}