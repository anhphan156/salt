#include <math.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct v2 {
  float x;
  float y;
} vec2;

char pixel(vec2);
float min(float x, float y);
float dot(vec2, vec2);
float clamp(float, float, float);
float lineSDF(vec2, vec2, vec2);
float length(vec2);
float lengthSq(vec2);

double t;

int main() {
  struct winsize sz;
  ioctl(0, TIOCGWINSZ, &sz);
  float resx = sz.ws_col;
  float resy = sz.ws_row;
  float aspr = resx / resy;

  int i = 0;
  while (1) {
    i++;
    for (int y = 0; y < sz.ws_row; y += 1) {
      for (int x = 0; x < sz.ws_col; x += 1) {
        float u = (float)x / resx;
        float v = (float)y / resy;
        u *= aspr / 2.f;
        vec2 uv = {u, v};
        printf("%c", pixel(uv));
      }
      printf("\n");
    }
    t += 0.3;
    usleep(150000);
  }

  return 0;
}

char pixel(vec2 uv) {
  uv.x += .25f;
  vec2 s1, s2;
  s1.x = uv.x - .5f;
  s1.y = uv.y - .74f;
  float d1 = lengthSq(s1);
  s2.x = uv.x - .5f;
  s2.y = uv.y - .26f;
  float d2 = lengthSq(s2);

  vec2 a = {.7f, .5f};
  vec2 b = {1.5f, .5f};
  float dline = lineSDF(uv, a, b) - .115f;

  float d = min(d1, d2);
  d = min(d, dline);

  if (d < .0615f) {
    return '@';
  }
  return ' ';
}

float lineSDF(vec2 p, vec2 a, vec2 b) {
  vec2 pa, ba;
  pa.x = p.x - a.x;
  pa.y = p.y - a.y;
  ba.x = b.x - a.x;
  ba.y = b.y - a.y;

  float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0 - cos(t) * .25f);

  vec2 x;
  x.x = pa.x - ba.x * h;
  x.y = pa.y - ba.y * h;

  return length(x);
}

float min(float x, float y) { return x < y ? x : y; }

float dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

float clamp(float x, float a, float b) {
  if (x < a)
    return a;
  if (x > b)
    return b;
  return x;
}

float length(vec2 p) { return sqrt(p.x * p.x + p.y * p.y); }
float lengthSq(vec2 p) { return p.x * p.x + p.y * p.y; };
