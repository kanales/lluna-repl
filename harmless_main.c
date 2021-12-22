#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <lauxlib.h>
#include <luajit.h>
#include <lualib.h>

#include "definitions.h"
#include "handler.h"
#include "linebuffer.h"

void die(const char *msg) {
  perror(msg);
  exit(1);
}

#define DEBUG(...)                                                             \
  fputs(UP(1), stderr);                                                        \
  fputs(LEFT(999), stderr);                                                    \
  fprintf(stderr, __VA_ARGS__);                                                \
  fputs(DOWN(1), stderr);                                                      \
  fprintf(stderr, "\x1b[%dC", cursor);

///
struct termios og_termios;

void disable_raw() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios) == -1) {
    die("tcsetattr");
  }
}
void enable_raw() {
  if (tcgetattr(0, &og_termios) == -1)
    die("tcgetattr");
  atexit(disable_raw);

  struct termios raw = og_termios;
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_oflag &= ~(OPOST);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

///

char read_key() {
  int nread;
  char c;
  while ((nread = read(0, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

void display() {
  fputs(LEFT(999), stderr);
  fputs(CLEAR_ALL, stderr);
  fputs(prompt, stderr);
  const char *line_buffer = buffer_get();

  fprintf(stderr, "%s", line_buffer);
  fputs(LEFT(999), stderr);
  fprintf(stderr, "\x1b[%dC", cursor() + prompt_len);
}

void handle_newline() {
  display();
  const char *line_buffer = buffer_get();
  fprintf(stderr, "\r\n%s\r\n", line_buffer);
  cursor_reset();
  buffer_clear();
}

void process_keypress() {
  char c = read_key(), a, b;

  if (!iscntrl(c)) {
    buffer_write(c);
    display();
    return;
  }

  switch (c) {
  case 17:
  case CTRL_KEY('d'):
    exit(0);
    break;
  case 127:
  case CTRL_KEY('h'):
    buffer_delete();
    display();
    break;
  case 10:
  case 13:
    handle_newline();
    /* newline */
    break;
  case 27:
    /* escape sequence */
    a = read_key(), b = read_key();
    if (a == 91 && b == 68) {
      /* left */
      cursor_left();
    }
    if (a == 91 && b == 67) {
      /* right */
      cursor_right();
    }
  }
}

/**
 * TODO
 * define lua `repl` object with the following methods:
 * - before_print: string -> string
 * - execute: string -> string
 * - complete: string -> string
 * If the output of before_print is (visually) a different length
 * than its input it might show as broken
 */
int main(int argc, char *argv[]) {
  lua_State *L = luaL_newstate();

  luaL_openlibs(L);
  handler_init(L);
  enable_raw();
  init();
  display();
  while (1) {
    process_keypress();
    display();
    fflush(stderr);
  }

  return 0;
}
