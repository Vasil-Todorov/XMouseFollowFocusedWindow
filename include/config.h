#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// Declare config constants
#define MAX_LINE_LENGTH 1024
#define CONFIG_FILE_PATH "/etc/XMouseFollowFocusedWindow/config.ini"

extern const char *CONFIG_KEYS[];

typedef struct ConfigItem {
    char key[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} ConfigItem;

// Declare functions related to config
void load_config(const char *filename);
void set_default_values();
void print_config();

bool get_config_bool(const char *key);
int get_config_int(const char *key);
double get_config_double(const char *key);

int set_config_double(const char *key, double value);

#endif // CONFIG_H