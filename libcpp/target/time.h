#ifndef TIME_H
#define TIME_H

typedef unsigned long long time_t;
typedef unsigned long long clock_t;

struct timespec {};
struct tm {};

clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm *timeptr);
time_t time(time_t *timer);
int timespec_get(timespec *ts, int base);
char *asctime(const struct tm *timeptr);
char *ctime(const time_t *timer);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
size_t strftime(char * s,
                size_t maxsize,
                const char * format,
                const struct tm * timeptr);
#endif