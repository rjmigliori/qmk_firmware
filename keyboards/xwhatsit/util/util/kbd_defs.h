#pragma once

struct key_def
{
    unsigned int row;
    unsigned int col;
    double x;
    double y;
    double w;
    double h;
};

struct lay_def
{
    const char *lay_name;
    int n_keys;
    const struct key_def *keys;
};

struct kbd_def
{
    const char *kbd_name;
    int n_layouts;
    const struct lay_def *layouts;
    unsigned int cols;
    unsigned int rows;
};

extern struct kbd_def keyboards[];
extern int n_keyboards;
