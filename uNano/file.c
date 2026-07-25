#include "file.h"
#include "editor.h"

#include <stdio.h>
#include <string.h>


static const char *current_filename = NULL;


void file_open(const char *filename)
{
    current_filename = filename;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
        return;

    editor_clear();

    char line[1024];

    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\r\n")] = '\0';

        editor_append_row(line);
    }

    fclose(fp);
}


void file_save(void)
{
    if (current_filename == NULL)
        return;

    FILE *fp = fopen(current_filename, "w");

    if (fp == NULL)
        return;

    int count = editor_row_count();

    for (int i = 0; i < count; i++)
    {
        fprintf(
            fp,
            "%s\n",
            editor_row_text(i)
        );
    }

    fclose(fp);
}


const char *file_name(void)
{
    return current_filename;
}
