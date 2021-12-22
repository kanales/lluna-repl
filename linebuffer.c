#include "linebuffer.h"

#include "definitions.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 4096

static char line_buffer[BUFSIZE];
static int _cursor = 0;

void init() {
  memset(line_buffer, '\0', BUFSIZE);
  fputs("Press Ctrl+D to exit\n", stderr);
}

int cursor() { return _cursor; }

void cursor_left() {
  if (_cursor > 0) {
    _cursor -= 1;
    fputs(LEFT(1), stderr);
  }
}
void cursor_right() {
  int max = strlen(line_buffer);
  if (_cursor < max) {
    _cursor += 1;
    fputs(RIGHT(1), stderr);
  }
}

void cursor_reset() { _cursor = 0; }

void buffer_write(char c) {
  char *ptr = &line_buffer[_cursor];
  char tmp;
  do {
    tmp = *ptr;
    *ptr = c;
    c = tmp;
  } while (*(++ptr) != '\0');
  *ptr = tmp;

  cursor_right();
}

const char *buffer_get() { return (const char *)line_buffer; }
void buffer_delete() {
  for (char *ptr = &line_buffer[_cursor - 1]; *ptr != '\0'; ptr++) {
    *ptr = *(ptr + 1);
  }
  cursor_left();
}

void buffer_clear() { memset(line_buffer, '\0', strlen(line_buffer)); }
