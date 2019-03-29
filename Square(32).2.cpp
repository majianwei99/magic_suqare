#pragma warning(disable : 4996)
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <thread>
#define MAXNN 100
#define MAXN 10
//#include <thread>
int stdcr;
typedef struct square
{
    int i[MAXNN];
    int remain[MAXNN];
    int totalCount, resultCount, partResultCount;
    int t_start, t_current;
    int stdsum;
    int rsum[MAXN], csum[MAXN];
    int working;
    int threshold_max[MAXN], threshold_min[MAXN];
    int threshold_max_my[4];
    int threshold_min_my[MAXN];
} SQUARE;

typedef struct squarelist
{
    int i[MAXNN];
    struct squarelist *next;
} SQUARENODE;
int exam(SQUARE *ps);
void print(SQUARE s, int direct);
void msquare(SQUARE *ps, int no);
//void    fileWrite( char *filename, SQUARENODE head) ;
void initSquare(SQUARE *ps);
void beginWorker(SQUARE *ps);

SQUARENODE *sl_append(SQUARE *ps);
void sl_free(SQUARENODE head);

int NN;
int N, interval;
SQUARENODE head, *tail;

int main(int argc, char *argv[])
{
    SQUARE s;

    printf("input a number(3-5):or input a interval");
    scanf("%d,%d", &N, &interval);
    if (interval <= 0)
        interval = 1000;
    NN = N * N;

    head.next = NULL;
    tail = &head;

    initSquare(&s);

    //std::thread t1(beginWorker, &s);
    msquare(&s, 0);
    memcpy(s.i, tail->i, NN * sizeof(int));
    print(s, true);

    char filename[256];
    sprintf(filename, "%dx%d result.txt", N, N);
    //fileWrite( filename, head );
    sl_free(head);
    _getch();
}

void beginWorker(SQUARE *ps)
{
    ps->working = true;
    msquare(ps, 0);
}

void initSquare(SQUARE *ps)
{

    ps->t_start = clock();
    ps->resultCount = ps->totalCount = ps->partResultCount = 0;

    int i, temp;

    ps->working = false;
    temp = 0;
    for (i = 1; i <= NN; i++)
    {
        ps->i[i - 1] = 0;
        ps->remain[i] = 1;
        temp += i;
    }
    ps->stdsum = temp / N;
    ps->t_current = 0;

    ps->threshold_max[N - 1] = 0;
    temp = NN;
    for (i = 1; i < N; i++)
    {
        ps->threshold_max[N - 1 - i] = temp + ps->threshold_max[N - 1 - i + 1];
        temp--;
    }

    ps->threshold_min[N - 1] = 0;
    temp = 1;
    for (i = 1; i < N; i++)
    {
        ps->threshold_min[N - 1 - i] = temp + ps->threshold_min[N - 1 - i + 1];
        temp++;
    }
    for (i = 1; i <= N - 1; i++)
    {
        stdcr += i;
    }
    if (N == 4)
    {
        ps->threshold_max_my[0] = 0;
        ps->threshold_max_my[1] = 16;
        ps->threshold_max_my[2] = 31;
        ps->threshold_max_my[3] = 45;
        ps->threshold_min_my[0] = 0;
        ps->threshold_min_my[1] = 1;
        ps->threshold_min_my[2] = 3;
        ps->threshold_min_my[3] = 6;
    }
    if (N == 3)
    {
        ps->threshold_max_my[0] = 0;
        ps->threshold_max_my[1] = 9;
        ps->threshold_max_my[2] = 17;
        ps->threshold_min_my[0] = 0;
        ps->threshold_min_my[1] = 1;
        ps->threshold_min_my[2] = 3;
    }
    ps->t_start = clock();
}

SQUARENODE *sl_append(SQUARE *ps)
{
    SQUARENODE *newnode;

    newnode = new SQUARENODE;

    memcpy(newnode->i, ps->i, NN * sizeof(int));
    newnode->next = tail->next;
    tail->next = newnode;
    tail = newnode;

    return newnode;
}

void sl_free(SQUARENODE head)
{
    SQUARENODE *node;

    while (head.next != NULL)
    {
        node = head.next;
        head.next = node->next;
        delete node;
    }
}

void calcThreshold(SQUARE *ps)
{
    int i, count = 0;

    ps->threshold_max[N - 1] = 0;
    for (i = NN - 1; i >= 0; i--)
    {
        if (ps->remain[i])
        {
            count++;
            ps->threshold_max[N - 1 - count] = ps->threshold_max[N - 1 - count - 1];
            if (count == N - 1)
                break;
        }
    }
}

void msquare(SQUARE *ps, int no)
{
    int i;
    int r, c, rsum, csum, temprsum, tempcsum;
    r = no / N;
    c = no % N;

    if (r == 0)
    {
        ps->csum[c] = 0;
        if (c == 0)
        {
            for (i = 0; i < N; i++)
                ps->rsum[i] = 0;
        }
    }
    rsum = ps->rsum[r];
    csum = ps->csum[c];

    for (i = 1; i <= NN; i++)
    {
        if (ps->remain[i] == 0)
            continue;
        ps->partResultCount++; //all
        tempcsum = csum + i;
        temprsum = rsum + i;
        if (c >= N - 2)
        {
            if ((temprsum + ps->threshold_max_my[N - c] < ps->stdsum))
                continue;
           if(temprsum + ps->threshold_min_my[N - c] > ps->stdsum)
                continue;       }
        if (r >= N - 2)
        {
            if ((tempcsum + ps->threshold_max_my[N - r] < ps->stdsum) )
                continue;
            if (tempcsum + ps->threshold_min_my[N - r] > ps->stdsum)
                continue;
        }
        if (temprsum > ps->stdsum || tempcsum > ps->stdsum)
            break;

        if (c == N - 1 && temprsum != ps->stdsum)
            continue;
        if (r == N - 1 && tempcsum != ps->stdsum)
            continue;

        ps->i[no] = i;
        ps->remain[i] = 0;
        ps->rsum[r] = temprsum;
        ps->csum[c] = tempcsum;
        if (no == NN - 1)
        {
            ps->totalCount++; //part
            if (exam(ps))
            {
                ps->resultCount++; //real
                ps->t_current = clock() - ps->t_start;
                sl_append(ps);
                print(*ps, false);
            }
        }
        else
        {
            msquare(ps, no + 1);
        }
        ps->remain[i] = 1;
        ps->i[no] = 0;
    }
    ps->rsum[r] = rsum;
    ps->csum[c] = csum;
}

/*void fileWrite( char *filename, SQUARENODE head) 
{
    FILE    *fp;
    SQUARENODE *n;
    char    line[4096], temp[1024];
    int     i, serialnum = 1;

    fopen_s( &fp, filename, "w");
    n = head.next;

    while(n) {
        sprintf( line, "%6d [ ", serialnum++);
        for(i=0;i<NN;i++) {
            sprintf(temp, "%2d ", n->i[i]);
            strcat_s( line, sizeof(line), temp);
        } 
        strcat_s( line, sizeof(line), "]\n");
        fwrite( line, strlen(line), 1, fp);

        n = n->next;
    }

    fclose(fp);
}
*/
void print(SQUARE s, int direct)
{
    int j;

    if (s.resultCount % interval == 0 || (direct))
    {
        printf("%-6d[ ", s.resultCount);
        for (j = 0; j < NN; j++)
            printf("%2d ", s.i[j]);
        printf("] %ld\r\n", clock() - s.t_start);
    }
}

// 妫€鏌ユ暟缁刬鍚勫厓绱犳槸鍚︽弧瓒充節瀹牸瑕佹眰锛屽鏋滄弧瓒宠繑鍥為潪0銆?
int exam(SQUARE *ps)
{
    int j;
    int sum1, sum2;
    // 鍒濆鍖栧悇姹傚拰鍙橀噺锛屽噯澶囨眰鍜屾鏌?
    sum1 = 0;
    sum2 = 0;

    // 鎸夎/鍒椼€佸瑙掔嚎姹傚拰
    for (j = 0; j < N; j++)
    {
        sum1 += ps->i[j * N + j];         //M[j][j];
        sum2 += ps->i[j * N + N - 1 - j]; //M[j][2 - j];
        if ((j >= N / 2) & (sum1 < ps->stdsum / 2 || sum2 < ps->stdsum / 2))
            return 0;
    }

    // 妫€鏌ュ瑙掔嚎鏄惁鐩哥瓑
    if (sum1 != ps->stdsum || ps->stdsum != sum2)
        return 0;
    return 1;
}
