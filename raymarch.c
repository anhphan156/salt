#include <math.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct v2 {
  float x;
  float y;
} vec2;

typedef struct v3 {
  float x;
  float y;
  float z;
} vec3;

char pixel(vec2);
float min(float x, float y);
float clamp(float, float, float);
float length(vec3);
float scene(vec3 p);
float raymarch(vec3 ro, vec3 rd);
void normal(vec3 p, vec3 *out);
float dot(vec3, vec3);
float light(vec3 p, vec3 normal, vec3 lightPos);

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
  uv.x -= .5f;
  uv.y -= .5f;

  vec3 ro = {0.0f, 0.0f, 3.0f};
  vec3 rd = {uv.x, uv.y, 0.0f};
  vec3 lightPos = {ro.x, ro.y, ro.z};

  float d = raymarch(ro, rd);
  vec3 p = {ro.x + rd.x * d, ro.y + rd.y * d};
  vec3 n;
  normal(p, &n);
  float l = light(p, n, lightPos);

  if (l > .8f) {
    return '@';
  }
  if (l > .5f) {
    return '#';
  }
  if (l > .1f) {
    return 'p';
  }
  return ' ';
}

float clamp(float x, float a, float b) {
  if (x < a)
    return a;
  if (x > b)
    return b;
  return x;
}

float raymarch(vec3 ro, vec3 rd) {
  float d0 = 0.0f;
  float d = 0.0f;

  for (int i = 0; i < 50; i += 1) {
    vec3 ray = {ro.x + rd.x * d0, ro.y + rd.y * d0, ro.z + rd.z * d0};
    d = scene(ray);
    d0 += d;
    if (d0 > 100.0f || d < 0.001f)
      break;
  }

  return d0;
}

void normal(vec3 p, vec3 *out) {
  float e = 0.001f;
  vec3 posA, posB, n;

  posA.x = p.x + e;
  posA.y = p.y;
  posA.z = p.z;
  posB.x = p.x - e;
  posB.y = p.y;
  posB.z = p.z;
  n.x = scene(posA) - scene(posB);

  posA.x = p.x;
  posA.y = p.y + e;
  posA.z = p.z;
  posB.x = p.x;
  posB.y = p.y - e;
  posB.z = p.z;
  n.y = scene(posA) - scene(posB);

  posA.x = p.x;
  posA.y = p.y;
  posA.z = p.z + e;
  posB.x = p.x;
  posB.y = p.y;
  posB.z = p.z - e;
  n.z = scene(posA) - scene(posB);

  float normalLength = length(n);
  if (normalLength != 0.0f) {
    out->x = n.x / normalLength;
    out->y = n.y / normalLength;
    out->z = n.z / normalLength;
  } else {
    out->x = n.x;
    out->y = n.y;
    out->z = n.z;
  }
}

float light(vec3 p, vec3 normal, vec3 lightPos) {
  float l;

  vec3 lightDir = {lightPos.x - p.x, lightPos.y - p.y, lightPos.z - p.z};
  float lightDirLen = length(lightDir);
  if (lightDirLen != 0.0f) {
    lightDir.x /= lightDirLen;
    lightDir.y /= lightDirLen;
    lightDir.z /= lightDirLen;
  }

  l = dot(normal, lightDir);

  return l * .5 + .5;
}

float dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; };
float min(float x, float y) { return x < y ? x : y; }
float length(vec3 p) { return sqrt(p.x * p.x + p.y * p.y + p.z * p.z); }
float scene(vec3 p) { return length(p) - 2.5f; }
