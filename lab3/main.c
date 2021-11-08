#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

char* ReadLine (FILE* fin, int scount, int* last){
    int symbol = getc(fin);
    char* line;
    if (symbol != EOF && symbol != '\n'){
        line = ReadLine(fin, scount + 1, last);
        line[scount] = symbol;
    }
    else{
        line = (char*)malloc(scount + 1);
        line[scount] = '\0';
        if (symbol == EOF)
            *last = 1;
        else
            *last = 0;
    }
    return line;
}

int ParseLine (FILE* fin, char** timestr, char** requeststr, char** statusstr, char** fr){
    int last;
    char* l;
    char* line = ReadLine(fin, 0, &last);
    *fr = line;
    strtok_r(line, "[", &l);
    *timestr = strtok_r(NULL, "]", &l);
    strtok_r(NULL, "\"", &l);
    *requeststr = strtok_r(NULL, "\"", &l);
    *statusstr = strtok_r(NULL, " ", &l);
    return last;
}

int ParseTime (char* timestr){
    struct tm time;
    char* tempstr;
    char* l;
    tempstr = strtok_r(timestr, "/", &l);
    time.tm_mday = atoi(tempstr);
    tempstr = strtok_r(NULL, "/", &l);
    if (strcmp(tempstr, "Jan") == 0)
        time.tm_mon = 0;
    else if (strcmp(tempstr, "Feb") == 0)
        time.tm_mon = 1;
    else if (strcmp(tempstr, "Mar") == 0)
        time.tm_mon = 2;
    else if (strcmp(tempstr, "Apr") == 0)
        time.tm_mon = 3;
    else if (strcmp(tempstr, "May") == 0)
        time.tm_mon = 4;
    else if (strcmp(tempstr, "Jun") == 0)
        time.tm_mon = 5;
    else if (strcmp(tempstr, "Jul") == 0)
        time.tm_mon = 6;
    else if (strcmp(tempstr, "Aug") == 0)
        time.tm_mon = 7;
    else if (strcmp(tempstr, "Sep") == 0)
        time.tm_mon = 8;
    else if (strcmp(tempstr, "Oct") == 0)
        time.tm_mon = 9;
    else if (strcmp(tempstr, "Nov") == 0)
        time.tm_mon = 10;
    else if (strcmp(tempstr, "Dec") == 0)
        time.tm_mon = 11;
    tempstr = strtok_r(NULL, ":", &l);
    time.tm_year = atoi(tempstr) - 1900;
    tempstr = strtok_r(NULL, ":", &l);
    time.tm_hour = atoi(tempstr);
    tempstr = strtok_r(NULL, ":", &l);
    time.tm_min = atoi(tempstr);
    tempstr = strtok_r(NULL, " ", &l);
    time.tm_sec = atoi(tempstr);
    return mktime(&time);
}

int PeriodIn(){
    int period = 0;
    char* strin;
    printf("input period (123:55 ): ");
    scanf("%s", strin);
    char* tempstr;
    char* l;
    tempstr = strtok_r(strin, ":", &l);
    period += atoi(tempstr) * 60;
    tempstr = strtok_r(NULL, " ", &l);
    period += atoi(tempstr);
    return period;
}

struct Request{
    int time;
    int index;
    struct Request* prev;
    struct Request* next;
};

struct RequestList{
    struct Request* first;
    struct Request* last;
};

void Push(struct RequestList* requestlist, int time, int index){
    struct Request* q = (struct Request*) malloc(sizeof(struct Request));
    q->time = time;
    q->index = index;
    q->prev = NULL;
    q->next = NULL;
    if (requestlist->first == NULL)
        requestlist->last = q;
    else{
        q->next = requestlist->first;
        requestlist->first->prev = q;
    }
    requestlist->first = q;
}

void Pop(struct RequestList* requestlist){
    struct Request* q = requestlist->last;
    if (q != NULL){
        if (q->prev != NULL){
            requestlist->last = q->prev;
            requestlist->last->next = NULL;
        }
        else{
            requestlist->first = NULL;
            requestlist->last = NULL;
        }
        free(q);
    }
}

struct Period{
    int begintime;
    int endtime;
    int max;
};

int main() {
    int periodlen = PeriodIn();
    FILE* fin;
    fin = fopen("access_log_Jul95", "r");
    char* timestr;
    char* requeststr;
    char* statusstr;
    char* fr;
    struct RequestList requestlist = {NULL, NULL};
    int counter = 0;
    struct Period period = {0, 0};
    int last = 0;
    for (int i = 0; !last; i++){
        last = ParseLine(fin, &timestr, &requeststr, &statusstr, &fr);
        if (timestr != NULL){
            int time = ParseTime(timestr);
            Push(&requestlist, time, i);
            while (time - requestlist.last->time > periodlen)
                Pop(&requestlist);
            int inperiod = i - requestlist.last->index + 1;
            if (inperiod > period.max) {
                period.max = inperiod;
                period.begintime = requestlist.last->time;
                period.endtime = time;
            }
        }
        if (statusstr != NULL && statusstr[0] == '5'){
            printf("%s \n", requeststr);
            counter++;
        }
        free(fr);
    }
    time_t temp = period.begintime;
    struct tm* time;
    time = localtime(&temp);
    printf("busiest time:\nfrom %s", asctime(time));
    temp = period.endtime;
    time = localtime(&temp);
    printf(" to %s", asctime(time))
    printf("number of requests with error 5xx: %d", counter);
    return 0;
}
