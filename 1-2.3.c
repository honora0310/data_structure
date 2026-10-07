// different from 1-2.2: mistake prevention
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// ---------------------
int N, K, R, C, G;
/*
➢ N: number of cells
➢ K: max_move-time each move
➢ R: max_move
➢ C: number of special cells
➢ G: min_score required to complete the mission
*/
// ---------------------
typedef struct{
    char type; // 'L', 'S', 'H', 'P', 'B', 'T' (no effect: '\0')
    int value; // +-score; aim place
}Cell;
Cell *board = NULL;

// Store success route for printing
typedef struct{
    int *dice;
    int turns;
    int score;
}RouteRecord;
RouteRecord *route = NULL;
int total_successful_routes = 0;
int route_capacity = 10;

// Record cell board information
void record_cellboard(int N)
{
    for(int i=1 ; i<=N ; i++)
    {
        board[i].type = '\0';
        board[i].value = 0;
    }
    for(int i=0 ; i<C ; i++)
    {
        int id, val;
        char typee;
        if(scanf("%d %c %d", &id, &typee, &val) != 3) 
        {
            break;
        }
        if(id>=1 && id<=N) 
        {
            board[id].type = typee;
            board[id].value = val;
        }
    }
}

// ;
void effect_string(char *effect_original_string, const char *new_effect)
{
    if(strlen(effect_original_string) > 0) 
    {
        strcat(effect_original_string, ";");
    }
    strcat(effect_original_string, new_effect);   
}

void special_effect(int *final_cell,   
                    int *next_score,  int *next_bonus, int *next_penalty, int *next_shield, int *treasures,    
                    char *effect_str) 
{
    while(1) 
    {
        // player reache or pass goal: N
        if(*final_cell >= N) 
        {
            effect_string(effect_str, "Reach goal");
            *final_cell = N;
            break;
        }

        char type = board[*final_cell].type;
        int val = board[*final_cell].value;
        char temp[64];
        switch(type) 
        {
            case 'L': // Ladder: move to aim
                sprintf(temp, "Ladder %d->%d", *final_cell, val);
                effect_string(effect_str, temp);
                *final_cell = val;
                break;

            case 'S': // Snake: blocked by shield | move down
                if(*next_shield == 1) 
                {
                    effect_string(effect_str, "Snake blocked");
                    *next_shield = 0;
                    return; 
                } 
                else 
                {
                    sprintf(temp, "Snake %d->%d", *final_cell, val);
                    effect_string(effect_str, temp);
                    *final_cell = val;
                }
                break;

            case 'H': // Shield: give a shield (max: 1)
                effect_string(effect_str, "Shield");
                *next_shield = 1;
                return; 

            case 'P': // Penalty: decrease next move
                sprintf(temp, "Penalty -%d", val);
                effect_string(effect_str, temp);
                *next_penalty = val;
                return; 

            case 'B': // Bonus: increase next move
                sprintf(temp, "Bonus +%d", val);
                effect_string(effect_str, temp);
                *next_bonus = val;
                return; 

            case 'T': // Treasure: gain score on first visit
                if(!treasures[*final_cell]) 
                {
                    sprintf(temp, "Treasure +%d", val);
                    effect_string(effect_str, temp);
                    *next_score += val;
                    treasures[*final_cell] = 1;
                } 
                else 
                {
                    if(strlen(effect_str) == 0) 
                    {
                        strcpy(effect_str, "None");
                    }
                }
                return; 

            case '\0': // normal cell

            default:
                if(strlen(effect_str) == 0) 
                {
                    strcpy(effect_str, "None");
                }
                return; 
        }
    }
}

// tracking
typedef struct{
    int roll;
    int from;
    int move_to;
    char effect[256];
    int final_pos;
    int score;
    int bonus;
    int penalty;
    int shield;
}TurnState;

// clean helper
int current_penalty_next_placeholder(int p) 
{
    return p;
}

void DFS(int current_cell, int current_turn, int current_score, int current_bonus, 
         int current_penalty, int current_shield, int *treasures, int *dice_path)
{
    if(current_turn > R) 
        return;

    // dice rolls
    for(int roll=1 ; roll<=K ; roll++) 
    {
        dice_path[current_turn - 1] = roll;
        int from = current_cell;
        // value calculation
        int effective_move = roll + current_bonus - current_penalty;
        
        int next_bonus = 0;
        int next_penalty = 0;
        int next_shield = current_shield;
        int next_score = current_score;
        int *treasures_copy = malloc((N + 1) * sizeof(int));
        if(!treasures_copy) 
        {
            exit(1);
        }// stupid
        for(int i=1 ; i<=N ; i++) 
        {
            treasures_copy[i] = treasures[i];
        }
        int move_to;
        int final_pos;
        char effect_str[2048] = "";

        if(effective_move <= 0) 
        {
            move_to = from;
            final_pos = from;
            strcpy(effect_str, "None");
        } 
        else 
        {
            move_to = from + effective_move;
            final_pos = move_to;
            if(final_pos >= N) 
            {
                final_pos = N;
                effect_string(effect_str, "Reach goal");
            } 
            else 
            {
                special_effect(&final_pos, 
                               &next_score, &next_bonus, &next_penalty, &next_shield, 
                               treasures_copy, effect_str);
            }
        }

        if(final_pos>=N && current_turn<=R && next_score>=G) 
        {
            // Record route
            if(total_successful_routes >= route_capacity) 
            {
                route_capacity *= 2;
                RouteRecord *temp = realloc(route, route_capacity * sizeof(RouteRecord));
            
                // stupid
                if(!temp) 
                {
                    free(treasures_copy);
                    exit(1);
                }
                route = temp;
            }
            route[total_successful_routes].turns = current_turn;
            route[total_successful_routes].score = next_score;
            route[total_successful_routes].dice = malloc(current_turn * sizeof(int));
            if(!route[total_successful_routes].dice) 
            {
                free(treasures_copy);
                exit(1);
            } // stupid
            for(int i=0 ; i<current_turn ; i++) 
            {
                route[total_successful_routes].dice[i] = dice_path[i];
            }
            total_successful_routes++;
        } 
        else if(final_pos<N && current_turn<R) 
        {
            DFS(final_pos, current_turn + 1, next_score, next_bonus, 
                current_penalty_next_placeholder(next_penalty), next_shield, treasures_copy, dice_path); 
            // recursive 
        }
        free(treasures_copy);
    }
}

void print_successful_routes()
{
    int shortest_route_idx = 0;
    int shortest_turns = route[0].turns;
    int highest_score_idx = 0;
    int highest_score = route[0].score;

    for(int r=0 ; r<total_successful_routes ; r++)
    {
        if(route[r].turns < shortest_turns)
        {
            shortest_turns = route[r].turns;
            shortest_route_idx = r;
        }
        if(route[r].score > highest_score)
        {
            highest_score = route[r].score;
            highest_score_idx = r;
        }

        printf("Route %d:\n", r + 1);
        printf("Dice:");
        for(int i=0 ; i<route[r].turns ; i++)
        {
            printf(" %d", route[r].dice[i]);
        }
        printf("\n");

        // precise output state
        int current_cell = 1;
        int current_score = 0;
        int current_bonus = 0;
        int current_penalty = 0;
        int current_shield = 0;
        int *treasures = calloc(N + 1, sizeof(int));

        for(int i=1 ; i<=route[r].turns ; i++)
        {
            int roll = route[r].dice[i - 1];
            int from = current_cell;
            int effective_move = roll + current_bonus - current_penalty;

            int next_bonus = 0;
            int next_penalty = 0;
            int next_shield = current_shield;
            int next_score = current_score;
            
            int move_to;
            int final_pos;
            char effect_str[2048] = "";

            if(effective_move <= 0)
            {
                move_to = from;
                final_pos = from;
                strcpy(effect_str, "None");
            }
            else
            {
                move_to = from + effective_move;
                final_pos = move_to;
                if(final_pos >= N)
                {
                    final_pos = N;
                    effect_string(effect_str, "Reach goal");
                }
                else
                {
                    special_effect(&final_pos, &next_score, &next_bonus, &next_penalty, &next_shield, treasures, effect_str);
                }
            }

            printf("Turn %d: roll=%d, from=%d, move_to=%d, effect=%s, final=%d, score=%d, bonus=%d, penalty=%d, shield=%d\n",
                   i, roll, from, move_to, effect_str, final_pos, next_score, next_bonus, next_penalty, next_shield);

            current_cell = final_pos;
            current_score = next_score;
            current_bonus = next_bonus;
            current_penalty = next_penalty;
            current_shield = next_shield;
        }

        printf("Result: Win in %d turns, score=%d\n", route[r].turns, route[r].score);
        printf("\n"); 
        free(treasures);
    }

    // summary
    printf("Summary:\n");
    printf("Total successful routes: %d\n", total_successful_routes);
    printf("Shortest route: Route %d, turns=%d\n", shortest_route_idx + 1, route[shortest_route_idx].turns);
    printf("Highest score: Route %d, score=%d", highest_score_idx + 1, route[highest_score_idx].score);
}

int main()
{
    if(scanf("%d %d %d %d %d", &N, &K, &R, &C, &G) != 5) 
    {
        return 0;
    }

    board = malloc((N + 1) * sizeof(Cell));
    route = malloc(route_capacity * sizeof(RouteRecord));
    if(!board || !route) 
    {
        return 1;
    }// stupid

    record_cellboard(N);

    int *treasures = calloc(N + 1, sizeof(int));
    int *dice_path = malloc((R + 1) * sizeof(int));
    if(!treasures || !dice_path) 
    {
        return 1;
    }
    DFS(1, 1, 0, 0, 0, 0, treasures, dice_path);

    if(total_successful_routes == 0) 
    {
        printf("No successful route\n");
    } 
    else 
    {
        print_successful_routes();
    }

    free(board);
    free(treasures);
    free(dice_path);
    for(int i=0 ; i<total_successful_routes ; i++) 
    {
        free(route[i].dice);
    }
    free(route);

    return 0;
}