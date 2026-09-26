#include <stdio.h>
#include <stdlib.h>
int segment(int *Height, int Left, int Right)
{
    int left = Left;
    int right = Right;
    int left_max = 0;
    int right_max = 0;
    int total = 0;
    
    while(left <= right) 
    {
        if(Height[left] < Height[right]) 
        {
            if(Height[left] > left_max) 
                left_max = Height[left];
            else
                total += left_max - Height[left];
            left++;
        } 
        else 
        {
            if(Height[right] > right_max) 
                right_max = Height[right];
            else
                total += right_max - Height[right];
            right--;
        }
    }
    return total;    
}

int water(int *height, int N)
{
    int total_water = 0;
    
    for(int start=1 ; start<=N ; start++)
    {
        // find boundary
        int end = start;
        while(end <= N && height[end] != -1) 
        {
            end++;
        }
        
        total_water += segment(height, start, end - 1);
        start = end;
    }
    return total_water;
}

void find_best_removal(int *height, int N, int *best_remove_place, int *max_water)
{
    *max_water = -1;
    *best_remove_place = -1;
    
    for(int i=1 ; i<=N ; i++)
    {
        // dams' height > 0
        if(height[i] > 0)
        {
            int original_height = height[i];
            height[i] = 0;
            
            int current_water = water(height, N);
            /* Update: only if strictly greater, 
                       same: keeping the smallest index.
            */
            if(current_water > *max_water)
            {
                *max_water = current_water;
                *best_remove_place = i;
            }
            
            height[i] = original_height;
        }
    }
}

int main()
{
    int T; // the number of test cases; 1 <= T <= 20
    if (scanf("%d", &T) != 1) 
    {
        return 0;
    }
    
    for(int test=1 ; test<=T ; test++)
    {
        int N; // line_01: the number of dams; 1 <= N <= 5000
        scanf("%d", &N);
        
        // line_02: store height
        int *height = (int*)malloc((N+1) * sizeof(int));
        for(int i=1 ; i<=N ; i++) 
        {
            scanf("%d", &height[i]);
        }
        
        // result
        int max_water = -1; 
        int best_remove_place = -1;
        
        find_best_removal(height, N, &best_remove_place, &max_water);
        
        printf("Case %d:\n", test);
        printf("Remove: %d\n", best_remove_place);
        printf("Maximum water: %d\n", max_water);
        
        free(height);
    }
    return 0;
}
        
