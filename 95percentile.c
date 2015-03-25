/*
95percentile calculates the 95th percentile from a stream of numerical data

http://github.com/jehiah/95percentile

copyright 2011 Jehiah Czebotar <jehiah@gmail.com> 

Version: 1.0

uses uthash from http://uthash.sourceforge.net
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "uthash.h"

#define SUCCESS 0
#define FAILURE 1
#define VERSION "1.0"

#define BUFF_SIZE 10240

void run(char *input_filename);
struct NumericEntry *new_numeric_entry(char *key);
int sort_numeric_entry(struct NumericEntry *a, struct NumericEntry *b);
void usage();
double perc = .95;

int verbose=0;

struct NumericEntry {
    UT_hash_handle hh;
    char *key;
    double value;
    uint64_t occurances;
};

struct NumericEntry *new_numeric_entry(char *key)
{
    struct NumericEntry *entry;
    entry = malloc(sizeof(struct NumericEntry));
    entry->key = key;
    entry->value = atof(key);
    entry->occurances = 0;
    return entry;
}

int sort_numeric_entry(struct NumericEntry *a, struct NumericEntry *b)
{
    if (b->value > a->value) {
        return -1;
    } else if (b->value < a->value) {
        return 1;
    }
    return 0;
}

void run(char *input_filename) {
    FILE *in_file = stdin;
    FILE *out_file = stdout;
    struct NumericEntry *entries = NULL;
    struct NumericEntry *entry, *tmp_entry;
    uint64_t total_entries = 0, current = 0, nintey_five_position;
    char buffer[BUFF_SIZE];

    if (input_filename) {
        if (verbose) fprintf(stderr, "input file is %s\n", input_filename);
        in_file = fopen(input_filename, "r" );
    } else {
        if (verbose) fprintf(stderr, "input is stdin\n");
    }
    
    if (!in_file){
        perror(input_filename);
        return;
    }
    
    while (fgets(buffer, BUFF_SIZE, in_file)) {
        HASH_FIND_STR(entries, buffer, entry);
        if (!entry) {
            entry = new_numeric_entry(strdup(buffer));
            HASH_ADD_KEYPTR(hh, entries, entry->key, strlen(entry->key), entry);
        }
        entry->occurances++;
        total_entries++;
    }
    
    HASH_SORT(entries, sort_numeric_entry);
    
    nintey_five_position = total_entries * perc;
    HASH_ITER(hh, entries, entry, tmp_entry) {
        current += entry->occurances;
        if (current >= nintey_five_position) {
            // key already has \n on it
            fprintf(stdout, "%s", entry->key);
            break;
        }
    }
    
    HASH_ITER(hh, entries, entry, tmp_entry) {
        HASH_DELETE(hh, entries, entry);
        free(entry->key);
        free(entry);
    }
    
    if (verbose) fprintf(stderr, "processed %"PRIu64" records\n", total_entries);
    fclose(in_file);
    fclose(out_file);
}


void usage(){
    fprintf(stderr, "usage: 95percentile (Version %s)\n", VERSION);
    fprintf(stderr, "\t-p 0.95 (your percentile value)\n");
    fprintf(stderr, "\t-i /path/to/input.json (optional; default is stdin)\n");
    fprintf(stderr, "\t-d debug output (to stderr)\n");
    fprintf(stderr, "\t-v version\n");
    fprintf(stderr, "\t-h this help\n");
}

int main(int argc, char **argv)
{
    int ch;
    char *input_filename = NULL;
    
    while ((ch = getopt(argc, argv, "i:vdhp:")) != -1) {
        switch (ch) {
        case 'i':
            input_filename = optarg;
            break;
        case 'v':
            fprintf(stdout, "Version %s\n", VERSION);
            exit(0);
            break;
        case 'd':
            verbose++;
            break;
        case 'h':
            usage();
            exit(0);
            break;
        case 'p':
            perc = atof(optarg);
            break;
        default:
            usage();
            exit(1);
            break;
        }
    }
    
    run(input_filename);
    return 0;
}
