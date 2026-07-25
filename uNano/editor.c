#include "editor.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_ROWS 100
#define MAX_LINE 1024

typedef struct
{
    char text[MAX_LINE];
    int length;

} Row;


static Row rows[MAX_ROWS];

static int row_count;

static int cx;
static int cy;

static int screen_rows;
static int screen_cols;


static void editor_get_window_size(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        screen_rows = 24;
        screen_cols = 80;
    }
    else
    {
        screen_rows = ws.ws_row;
        screen_cols = ws.ws_col;
    }
}


void editor_clear(void)
{
    row_count = 0;
    cx = 0;
    cy = 0;
}


void editor_append_row(const char *text)
{
    if (row_count >= MAX_ROWS)
        return;

    strncpy(
        rows[row_count].text,
        text,
        MAX_LINE - 1
    );

    rows[row_count].text[MAX_LINE - 1] = '\0';
    rows[row_count].length = strlen(rows[row_count].text);

    row_count++;
}


int editor_row_count(void)
{
    return row_count;
}


const char *editor_row_text(int row)
{
    if (row < 0 || row >= row_count)
        return NULL;

    return rows[row].text;
}


void editor_init(void)
{
    editor_get_window_size();
    editor_clear();
}


static void move_cursor(int x, int y)
{
    char buf[32];

    int len = snprintf(
        buf,
        sizeof(buf),
        "\033[%d;%dH",
        y + 1,
        x + 1
    );

    write(STDOUT_FILENO, buf, len);
}


void editor_draw(void)
{
    editor_get_window_size();

    for (int y = 0; y < screen_rows; y++)
    {
        if (y < row_count)
        {
            write(
                STDOUT_FILENO,
                rows[y].text,
                rows[y].length
            );
        }
        else
        {
            write(
                STDOUT_FILENO,
                "~",
                1
            );
        }

        write(
            STDOUT_FILENO,
            "\033[K",
            3
        );

        if (y < screen_rows - 1)
        {
            write(
                STDOUT_FILENO,
                "\r\n",
                2
            );
        }
    }

    move_cursor(cx, cy);
}


static void insert_char(int c)
{
    if (row_count == 0)
    {
        row_count = 1;
        rows[0].length = 0;
        rows[0].text[0] = '\0';
    }

    Row *row = &rows[cy];

    if (row->length >= MAX_LINE - 1)
        return;

    memmove(
        &row->text[cx + 1],
        &row->text[cx],
        row->length - cx + 1
    );

    row->text[cx] = c;
    row->length++;

    cx++;
}


static void insert_newline(void)
{
    if (row_count >= MAX_ROWS)
        return;

    if (row_count == 0)
    {
        row_count = 1;
        rows[0].length = 0;
        rows[0].text[0] = '\0';
    }

    for (int i = row_count; i > cy + 1; i--)
    {
        rows[i] = rows[i - 1];
    }

    Row *current = &rows[cy];
    Row *next = &rows[cy + 1];

    next->length = current->length - cx;

    memcpy(
        next->text,
        &current->text[cx],
        next->length
    );

    next->text[next->length] = '\0';

    current->length = cx;
    current->text[cx] = '\0';

    row_count++;

    cy++;
    cx = 0;
}


static void backspace(void)
{
    if (row_count == 0)
        return;

    Row *row = &rows[cy];

    if (cx > 0)
    {
        memmove(
            &row->text[cx - 1],
            &row->text[cx],
            row->length - cx + 1
        );

        row->length--;
        cx--;

        return;
    }

    if (cy > 0)
    {
        int old_length = rows[cy - 1].length;

        memcpy(
            &rows[cy - 1].text[old_length],
            row->text,
            row->length + 1
        );

        rows[cy - 1].length += row->length;

        for (int i = cy; i < row_count - 1; i++)
        {
            rows[i] = rows[i + 1];
        }

        row_count--;

        cy--;
        cx = old_length;
    }
}


void editor_process_key(int c)
{
    switch (c)
    {
        case 127:
            backspace();
            break;

        case '\r':
        case '\n':
            insert_newline();
            break;

        case 1000:
            if (cy > 0)
            {
                cy--;

                if (cx > rows[cy].length)
                    cx = rows[cy].length;
            }
            break;

        case 1001:
            if (cy + 1 < row_count)
            {
                cy++;

                if (cx > rows[cy].length)
                    cx = rows[cy].length;
            }
            break;

        case 1002:
            if (cx < rows[cy].length)
                cx++;

            break;

        case 1003:
            if (cx > 0)
                cx--;

            break;

        default:
            if (c >= 32 && c <= 126)
                insert_char(c);

            break;
    }
}
