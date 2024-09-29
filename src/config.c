#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "debug.h"

#define MAX_CONFIG_ITEMS ((int)sizeof(CONFIG_KEYS) / (int)sizeof(CONFIG_KEYS[0]))

const char *CONFIG_KEYS[] = {
	"ISSMOOTH",
	"SMOOTHINCREMENT",
	"SLEEPTIME",
	"WARPOUTSIDE",
	"ALWAYSWARP",
	"DISTANCETOLERANCE",
	"WARPXPLACE",
	"WARPYPLACE",
	"KEEP_RELATIVE_POSITION"
};

const char *CONFIG_VALUES[] = {
	"1",
	"5",
	"200",
	"0",
	"0",
	"50",
	"90.0",
	"90.0",
	"0"
};

ConfigItem config_items[MAX_CONFIG_ITEMS];

static inline void load_config_internal(const char *filename);
static inline void set_default_values_internal();
static inline int get_config_int_internal(const char *key);
static inline bool get_config_bool_internal(const char *key);
static inline double get_config_double_internal(const char *key);

static inline int set_config_double_internal(const char *key, double value);

void load_config(const char *filename) { TIME_FUNCTION_VOID(load_config_internal, filename); }
void set_default_values() { TIME_FUNCTION_VOID(set_default_values_internal); }
int get_config_int(const char *key) { return TIME_FUNCTION_RETURN_INT(get_config_int_internal, "%s", key); }
bool get_config_bool(const char *key) { return TIME_FUNCTION_RETURN_BOOL(get_config_bool_internal, "%s", key); }
double get_config_double(const char *key) { return TIME_FUNCTION_RETURN_DOUBLE(get_config_double_internal, "%s", key); }

int set_config_double(const char *key, double value) { return TIME_FUNCTION_RETURN_INT(set_config_double_internal, "Key: %s, Value: %f", key, value); }

// Your existing functions from main.c related to config
void load_config_internal(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "Error opening config file: %s\n", strerror(errno));
		return;
	}

	char buffer[MAX_LINE_LENGTH] = {0};

	while (fgets(buffer, sizeof(buffer), fp))
	{
		// Ignore comments and empty lines
		if (buffer[0] == ';' || buffer[0] == '#' || buffer[0] == '\n')
		{
			continue;
		}

		// Split the line into key and value
		char *key = strtok(buffer, "=");
		if (key)
		{
			char *value = strtok(NULL, "=");
			if (value)
			{
				// Trim any trailing newline or spaces from value
				size_t value_len = strcspn(value, "\r\n");
				value[value_len] = '\0';

				// Find the matching config item
				int i;
				for (i = 0; i < MAX_CONFIG_ITEMS; i++)
				{
					if (strcmp(config_items[i].key, key) == 0)
					{
						break;
					}
				}

				// If the key is not found, skip it (don't add new keys)
				if (i == MAX_CONFIG_ITEMS)
				{
					fprintf(stderr, "Unrecognized key in config file: %s\n", key);
				}
				else
				{
					// Copy the value in one go
					strncpy(config_items[i].value, value, sizeof(config_items[i].value));
				}
			}
		}
	}

	fclose(fp);
}

void set_default_values_internal()
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		strcpy(config_items[i].key, CONFIG_KEYS[i]);
		strcpy(config_items[i].value, CONFIG_VALUES[i]);
	}
}

void print_config()
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		printf("%s: %s\n", config_items[i].key, config_items[i].value);
	}
}

static inline ConfigItem* find_config_item(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return &config_items[i];
		}
	}
	return NULL;
}

bool get_config_bool_internal(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
			return config_items[i].value[0] == '1';
	}
	DEBUG_PRINT_ERR("Key not found: %s\n", key);
	return false; // default to false if not found
}

int get_config_int_internal(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return atoi(config_items[i].value);
		}
	}
	DEBUG_PRINT_ERR("Key not found: %s\n", key);
	return 0; // default to 0 if not found
}

double get_config_double_internal(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return atof(config_items[i].value);
		}
	}
	DEBUG_PRINT_ERR("Key not found: %s\n", key);
	return 0.0; // default to 0.0 if not found
}

int set_config_double_internal(const char *key, double value)
{
	ConfigItem *item = find_config_item(key);
	if (item) {
		snprintf(item->value, sizeof(item->value), "%f", value);
		return 0;
	}
	return -1;
}
