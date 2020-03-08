#include "cfunction.h"
// https://github.com/raysan5/raylib/blob/master/src/core.c

/*
// Get pointer to extension for a filename string
char *GetExtension(char *fileName)
{
    char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return (dot + 1);
}
*/


// https://github.com/shadowsocks/simple-obfs/blob/master/src/netutils.c
static const char valid_label_bytes[] =
    "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

int validate_hostname(char *hostname,  int hostname_len)
{
    if (hostname == NULL)
        return 0;

    if (hostname_len < 1 || hostname_len > 255)
        return 0;

    if (hostname[0] == '.')
        return 0;

    const char *label = hostname;
    while (label < hostname + hostname_len) {
        size_t label_len = hostname_len - (label - hostname);
        char *next_dot   = strchr(label, '.');
        if (next_dot != NULL)
            label_len = next_dot - label;

        if (label + label_len > hostname + hostname_len)
            return 0;

        if (label_len > 63 || label_len < 1)
            return 0;

        if (label[0] == '-' || label[label_len - 1] == '-')
            return 0;

        if (strspn(label, valid_label_bytes) < label_len)
            return 0;

        label += label_len + 1;
    }

    return 1;
}
