#include <stdio.h>
#include <stdlib.h>
typedef long long ll;

// queue
typedef struct{
    int head, tail;   // job IDs, 0 = empty 
    ll pages;         // total remaining pages 
}Queue;
static int *nextJob;  
static ll  *jobRem;   
static void queue_push(Queue *q, int j) 
{
    nextJob[j] = 0;
    if(q->tail) 
        nextJob[q->tail] = j; 
    else 
        q->head = j;
    q->tail = j;
    q->pages += jobRem[j];
}
static int queue_pop(Queue *q) 
{
    int j = q->head;
    q->head = nextJob[j];
    if(!q->head) 
        q->tail = 0;
    q->pages -= jobRem[j];
    return j;
}
static int queue_empty(const Queue *q) 
{ 
    return q->head == 0; 
}

typedef struct{
    ll speed, heatLimit, restTime;
    Queue waitQ;
    int curJob;      // job being printed, 0 = none 
    ll segStart;     // time the current printing segment began 
    ll count;        // consecutive pages since last rest / idle 
    int resting;
    ll restEnd;
}Printer;
static int M, N;
static Printer *pr;
static ll min_ll(ll a, ll b) 
{ 
    return a < b ? a : b; 
}
static ll max_ll(ll a, ll b) 
{ 
    return a > b ? a : b; 
}

// next event (completes or overheats)
static ll event_time(const Printer *p) 
{
    ll k = min_ll(jobRem[p->curJob], p->heatLimit - p->count);
    return p->segStart + k * p->speed;
}

static ll estimate(int i, ll t, ll pages) 
{
    const Printer *p = &pr[i];
    ll start = t;
    if(p->resting) 
        start = max_ll(start, p->restEnd);
    if(p->curJob)  
        start = max_ll(start, p->segStart + jobRem[p->curJob] * p->speed);
    return start + (p->waitQ.pages + pages) * p->speed;
}

static int choose(ll t, ll pages, int skip) 
{
    int best = -1, i;
    ll bestT = 0;
    for(i=1 ; i<=M ; i++) 
    {
        ll e;
        if(i == skip) 
            continue;
        e = estimate(i, t, pages);
        if(best==-1 || e<bestT)
        { 
            best = i; 
            bestT = e; 
        }
    }
    return best;
}

// Jobs sorted by arrival time
static ll *arrival;
static int cmp_job(const void *a, const void *b) 
{
    int x = *(const int *)a, y = *(const int *)b;
    if(arrival[x] != arrival[y]) 
        return arrival[x] < arrival[y] ? -1 : 1;
    return x - y;
}

int main() 
{
    int i, j, nextArr = 0, done = 0;
    int *order, *arrived, *reSrc, *reJob, *overheated, *justOverheated;

    if(scanf("%d %d", &M, &N) != 2) 
        return 0;
    pr = (Printer *)calloc(M + 1, sizeof(Printer));
    for(i=1 ; i<=M ; i++)
        if(scanf("%lld %lld %lld", &pr[i].speed, &pr[i].heatLimit, &pr[i].restTime) != 3) 
            return 0;

    arrival = (ll *)calloc(N + 1, sizeof(ll));
    jobRem  = (ll *)calloc(N + 1, sizeof(ll));
    nextJob = (int *)calloc(N + 1, sizeof(int));
    for(j=1 ; j<=N ; j++)
        if(scanf("%lld %lld", &arrival[j], &jobRem[j]) != 2) 
            return 0;

    order = (int *)malloc(N * sizeof(int));
    for(j=0 ; j<N ; j++) 
        order[j] = j + 1;
    qsort(order, N, sizeof(int), cmp_job);

    arrived = (int *)malloc(N * sizeof(int));
    reSrc = (int *)malloc((M + 1) * sizeof(int));
    reJob = (int *)malloc((M + 1) * sizeof(int));
    overheated = (int *)malloc((M + 1) * sizeof(int));
    justOverheated = (int *)calloc(M + 1, sizeof(int));

    while(1)
    {
        ll t = -1;
        int nRe = 0, nOver = 0, nArr = 0, k;

        //next time 
        if(nextArr < N) 
            t = arrival[order[nextArr]];
        for(i=1 ; i<=M ; i++) 
        {
            ll x;
            if(pr[i].curJob)       
                x = event_time(&pr[i]);
            else if(pr[i].resting) 
                x = pr[i].restEnd;
            else 
                continue;
            if(t==-1 || x<t) 
                t = x;
        }

        for(i=1 ; i<=M ; i++)
        {
            Printer *p = &pr[i];
            ll pages;
            justOverheated[i] = 0;
            if(!p->curJob || event_time(p)!=t) 
                continue;
            j = p->curJob;
            pages = (t - p->segStart) / p->speed;  
            jobRem[j] -= pages;
            p->count  += pages;
            p->curJob  = 0;
            if(jobRem[j] == 0) 
            {
                printf("Time %lld: COMPLETE Job %d in Printer %d\n", t, j, i);
                done++;
            } 
            else 
            {                              
                reSrc[nRe] = i; reJob[nRe] = j; nRe++;
            }
            if(p->count == p->heatLimit) 
            { 
                overheated[nOver++] = i; 
                justOverheated[i] = 1; 
            }
        }
        if(done == N) 
        {                           
            printf("Finish time: %lld", t);
            break;
        }
        for(k=0 ; k<nOver ; k++) 
        {
            Printer *p = &pr[overheated[k]];
            p->resting = 1;
            p->restEnd = t + p->restTime;
            printf("Time %lld: OVERHEAT Printer %d, Restart time=%lld\n", t, overheated[k], p->restEnd);
        }

        // (3) REST END 
        for(i=1 ; i<=M ; i++) 
        {
            Printer *p = &pr[i];
            if(p->resting && p->restEnd == t && !justOverheated[i]) 
            {
                p->resting = 0;
                p->count = 0;
                printf("Time %lld: REST END Printer %d\n", t, i);
            }
        }

        // (4) ARRIVE (order is already sorted by job ID on ties)
        while(nextArr < N && arrival[order[nextArr]] == t) 
        {
            j = order[nextArr++];
            arrived[nArr++] = j;
            printf("Time %lld: ARRIVE Job %d, pages=%lld\n", t, j, jobRem[j]);
        }

        // (5) REASSIGN (increasing source printer ID) 
        for(k=0 ; k<nRe ; k++) 
        {
            int d = choose(t, jobRem[reJob[k]], reSrc[k]);
            queue_push(&pr[d].waitQ, reJob[k]);
            printf("Time %lld: REASSIGN Job %d, remaining pages=%lld, from Printer %d to Printer %d\n", t, reJob[k], jobRem[reJob[k]], reSrc[k], d);
        }

        // (6) ASSIGN new arrivals one at a time 
        for(k=0 ; k<nArr ; k++) 
        {
            int d = choose(t, jobRem[arrived[k]], 0);
            queue_push(&pr[d].waitQ, arrived[k]);
            printf("Time %lld: ASSIGN Job %d to Printer %d\n", t, arrived[k], d);
        }

        // (7) START, and idle handling 
        for(i=1 ; i<=M ; i++)
        {
            Printer *p = &pr[i];
            if(p->resting || p->curJob) 
                continue;
            if(queue_empty(&p->waitQ)) 
            { 
                p->count = 0; 
                continue; 
            } 
            j = queue_pop(&p->waitQ);
            p->curJob = j;
            p->segStart = t;
            printf("Time %lld: START Printer %d, Job %d, remaining pages=%lld\n", t, i, j, jobRem[j]);
        }
    }

    free(pr); 
    free(arrival); 
    free(jobRem); 
    free(nextJob); 
    free(order);
    free(arrived); 
    free(reSrc); 
    free(reJob); 
    free(overheated); 
    free(justOverheated);
    return 0;
}