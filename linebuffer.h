#ifndef _LINEBUFFER_H_
#define _LINEBUFFER_H_
#include "definitions.h"

static const char prompt[] = ">>> ";
static const int prompt_len = sizeof(prompt) - 1;

void init();
int cursor();
void cursor_left();
void cursor_right();
void cursor_reset();

void buffer_write(char c);
const char *buffer_get();
void buffer_delete();
void buffer_clear();

#endif
