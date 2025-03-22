#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*                                     * - Study planner - *
    Write program to print your daily schedule. The program reads your schedule from a CSV file. The
    header row has four headings: Day, Time, Course, Room. Data rows don’t need to appear in any
    specific order, but program must print times in ascending order. Program asks user to choose a day
    and prints the day’s schedule starting with the earliest time in ascending order. If there are no classes for
    the day program prints “No classes today”. If user enters “stop” as a day the program stops.

    For example:

    Enter day: Tuesday
    On Tuesday you have:
    9:00 Java, KNE556
    13:00 Network, KMD666
    Enter day: Thursday
    No classes today        */

#define MAX_LENGTH 100
/* the project description didn't say anything about asking a file name from the user,
   so I just went with the "define route" that can be manually changed depending on the file name vvv */
#define FILE_NAME "timetable"

// struct for the entries on the timetable file vvv
typedef struct {
    char day[MAX_LENGTH];
    char time[MAX_LENGTH];
    char course[MAX_LENGTH];
    char room[MAX_LENGTH];
} TimetableDay;

int read_file(const char *filename, TimetableDay timetable[], int *count);
void print_day(const TimetableDay schedule[], int count, const char *day);
int compare_times(const void *a, const void *b);

// function for the funny banner when the program is run vvv
void banner(void)
{
    printf("-------------------------------------------------------------------------------\n");
    printf(" _____ _____ _   _________   __ ______ _       ___   _   _  _   _  ___________  \n");
    printf("/  ___|_   _| | | |  _  \\ \\ / / | ___ \\ |     / _ \\ | \\ | || \\ | ||  ___| ___ \\ \n");
    printf("\\ `--.  | | | | | | | | |\\ V /  | |_/ / |    / /_\\ \\|  \\| ||  \\| || |__ | |_/ / \n");
    printf(" `--. \\ | | | | | | | | | \\ /   |  __/| |    |  _  || . ` || . ` ||  __||    /  \n");
    printf("/\\__/ / | | | |_| | |/ /  | |   | |   | |____| | | || |\\  || |\\  || |___| |\\ \\  \n");
    printf("\\____/  \\_/  \\___/|___/   \\_/   \\_|   \\_____/\\_| |_\\_| \\_/\\_| \\_/\\____/\\_| \\_| \n");
    printf("                                                                        V.1.0\n");
    printf("-------------------------------------------------------------------------------\n");
}

//this function reads the csv file into an array vvv
int read_file(const char *filename, TimetableDay timetable[], int *count)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    // skips the header (in the file: "Day, Time, Course, Room") vvv
    char line[MAX_LENGTH];
    fgets(line, sizeof(line), file);

    // reads the data after the header vvv
    while (fgets(line, sizeof(line), file)) {
        if (*count >= MAX_LENGTH) {
            break;
        }

        // parses the lines into fields vvv
        if (sscanf(line, " %9[^,], %5[^,], %49[^,], %9[^\n]",
                   timetable[*count].day,
                   timetable[*count].time,
                   timetable[*count].course,
                   timetable[*count].room) == 4) {
            (*count)++;
                   }
    }

    fclose(file);
    return 0;
}

// this function handles filtering and printing for the inputted day vvv
void print_day(const TimetableDay schedule[], int count, const char *day)
{
    // creates a temporary array for storing the course name for the inputted day vvv
    TimetableDay day_schedule[MAX_LENGTH];
    int day_count = 0;

    // and then filters the entries vvv
    for (int i = 0; i < count; i++) {
        if (strcmp(schedule[i].day, day) == 0) {
            day_schedule[day_count++] = schedule[i];
        }
    }

    // checks if no course names are found vvv
    if (day_count == 0) {
        printf("You have no classes today!\n");
        return;
    }

    // this sorts the course names by time vvv
    qsort(day_schedule, day_count, sizeof(TimetableDay), compare_times);

    // and prints the schedule in aligned columns vvv
    printf("--------- %s ---------\n", day);
    for (int i = 0; i < day_count; i++) {
        printf("%-6s %s, %s\n", day_schedule[i].time, day_schedule[i].course, day_schedule[i].room);
    }
}

// this function handles the sorting in ascending order vvv
int compare_times(const void *a, const void *b)
{
    TimetableDay *entryA = (TimetableDay *)a;
    TimetableDay *entryB = (TimetableDay *)b;

    // extracts hours and minutes vvv
    int hourA, minuteA, hourB, minuteB;
    sscanf(entryA->time, "%d:%d", &hourA, &minuteA);
    sscanf(entryB->time, "%d:%d", &hourB, &minuteB);

    // and then compares the time (first hours and then minutes) vvv
    if (hourA != hourB) {
        return hourA - hourB;
    }
    return minuteA - minuteB;
}

// made this function to convert inputs like "monday" or "MONDAY" to "Monday" before comparison vvv
void fix_user_input(char *str)
{

    // converts first letter into uppercase vvv
    if (str[0] >= 'a' && str[0] <= 'z') {
        str[0] = str[0] - 'a' + 'A';
    }

    // and makes the rest lowercase vvv
    for (int i = 1; str[i] != '\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
    }
}

// and finally the main function vvv
int main(void)
{
    TimetableDay schedule[MAX_LENGTH];
    int count = 0;
    char input_day[MAX_LENGTH];

    // for checking if the user input is a valid day vvv
    const char *valid_days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    if (read_file(FILE_NAME, schedule, &count) != 0) {
        fprintf(stderr, "There was an error reading schedule file. :( Maybe it's missing.\n");
        return 1;
    }

    banner();

    while (1) {
        printf("Please enter day (or 'stop' to QUIT):\n");
        scanf("%9s", input_day);

        if (strcmp(input_day, "stop") == 0) {
            break;
        }

        fix_user_input(input_day);

        // checks if the day is valid vvv
        int valid_day = 0;
        for (int i = 0; i < 7; i++) {
            if (strcmp(input_day, valid_days[i]) == 0) {
                valid_day = 1;
                break;
            }
        }

        if (!valid_day) {
            printf("That's not a valid day. :( Please enter a valid day of the week!\n");
            continue;
        }
        print_day(schedule, count, input_day);
    }
    printf("Exiting program... See you next time! :)");
    return 0;
}




