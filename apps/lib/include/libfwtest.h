/* libfwtest.h */

void dumpargs(int argc, char **argv);

/* fwtools */
int debugfs_get_attr(char *class_path, const char *attr, char *value, int len);
int debugfs_set_attr(char *class_path, const char *attr, char *value, int len);
