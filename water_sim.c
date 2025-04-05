#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define GRID_SIZE 50      // Size of the water grid
#define GRID_SCALE 0.1    // Scale factor for grid spacing
#define WAVE_SPEED 0.05   // Speed of background wave
#define RIPPLE_DECAY 0.95 // Damping factor for ripple
#define RIPPLE_SPEED 0.2  // Speed of ripple propagation

float angle = 0.0; // Camera rotation angle
float time = 0.0;  // Time for animation

// Ripple structure
typedef struct {
  float x, z;      // Center of ripple
  float amplitude; // Strength of ripple
  float radius;    // Current radius of ripple
  int active;      // Whether ripple is active
} Ripple;

Ripple ripples[10]; // Support up to 10 ripples
int rippleCount = 0;

// Base water height (background waves)
float baseWaterHeight(float x, float z) {
  return 0.15 * sin(x * 2.0 + time) + 0.1 * cos(z * 1.5 + time);
}

// Ripple height
float rippleHeight(float x, float z) {
  float totalHeight = 0.0;
  for (int i = 0; i < rippleCount; i++) {
    if (ripples[i].active) {
      float dx = x - ripples[i].x;
      float dz = z - ripples[i].z;
      float distance = sqrt(dx * dx + dz * dz);
      float wavePhase = distance - ripples[i].radius;
      if (wavePhase < 0 && wavePhase > -1.5) { // Wider ripple
        totalHeight +=
            ripples[i].amplitude * sin(wavePhase * 15.0) * exp(-distance * 0.3);
      }
    }
  }
  return totalHeight;
}

// Combined water height
float waterHeight(float x, float z) {
  return baseWaterHeight(x, z) + rippleHeight(x, z);
}

// Compute normal vector at a point for lighting
void computeNormal(float x, float z, float *normal) {
  float h = waterHeight(x, z);
  float hx = waterHeight(x + 0.01, z) - h; // Approximate derivative in x
  float hz = waterHeight(x, z + 0.01) - h; // Approximate derivative in z
  normal[0] = -hx;                         // Tangent slope in x
  normal[1] = 1.0;                         // Up direction
  normal[2] = -hz;                         // Tangent slope in z
  float mag = sqrt(normal[0] * normal[0] + normal[1] * normal[1] +
                   normal[2] * normal[2]);
  normal[0] /= mag;
  normal[1] /= mag;
  normal[2] /= mag;
}

// Initialize OpenGL settings
void init() {
  glClearColor(0.0, 0.1, 0.3, 1.0); // Deep blue background
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);

  // Light properties
  GLfloat light_pos[] = {2.0, 5.0, 2.0, 0.0};      // Higher light source
  GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1.0};  // Soft ambient light
  GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};  // Bright diffuse light
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0}; // White specular light
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  // Material properties for water
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0}; // Shiny highlights
  GLfloat mat_shininess[] = {50.0}; // Specular exponent (shininess)
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  // Initialize ripples
  for (int i = 0; i < 10; i++) {
    ripples[i].active = 0;
  }
}

// Display callback
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  gluLookAt(3.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glRotatef(angle, 0.0, 1.0, 0.0);

  glBegin(GL_QUADS);
  for (int x = 0; x < GRID_SIZE - 1; x++) {
    for (int z = 0; z < GRID_SIZE - 1; z++) {
      float x0 = (x - GRID_SIZE / 2) * GRID_SCALE;
      float z0 = (z - GRID_SIZE / 2) * GRID_SCALE;
      float x1 = (x + 1 - GRID_SIZE / 2) * GRID_SCALE;
      float z1 = (z + 1 - GRID_SIZE / 2) * GRID_SCALE;

      float h00 = waterHeight(x0, z0);
      float h10 = waterHeight(x1, z0);
      float h01 = waterHeight(x0, z1);
      float h11 = waterHeight(x1, z1);

      // Compute normals for each vertex
      float normal[3];
      computeNormal(x0, z0, normal);
      glNormal3fv(normal);
      glColor3f(0.0, 0.5 + h00 * 0.5, 0.8); // Color varies with height
      glVertex3f(x0, h00, z0);

      computeNormal(x1, z0, normal);
      glNormal3fv(normal);
      glColor3f(0.0, 0.5 + h10 * 0.5, 0.8);
      glVertex3f(x1, h10, z0);

      computeNormal(x1, z1, normal);
      glNormal3fv(normal);
      glColor3f(0.0, 0.5 + h11 * 0.5, 0.8);
      glVertex3f(x1, h11, z1);

      computeNormal(x0, z1, normal);
      glNormal3fv(normal);
      glColor3f(0.0, 0.5 + h01 * 0.5, 0.8);
      glVertex3f(x0, h01, z1);
    }
  }
  glEnd();

  glutSwapBuffers();
}

// Reshape callback
void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (float)width / height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

// Idle callback
void idle() {
  time += WAVE_SPEED;
  for (int i = 0; i < rippleCount; i++) {
    if (ripples[i].active) {
      ripples[i].radius += RIPPLE_SPEED;
      ripples[i].amplitude *= RIPPLE_DECAY;
      if (ripples[i].amplitude < 0.01) {
        ripples[i].active = 0;
      }
    }
  }
  glutPostRedisplay();
}

// Mouse callback
void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && rippleCount < 10) {
    float wx = (float)(x - glutGet(GLUT_WINDOW_WIDTH) / 2) /
               (glutGet(GLUT_WINDOW_WIDTH) / 2) * 2.5;
    float wz = (float)(y - glutGet(GLUT_WINDOW_HEIGHT) / 2) /
               (glutGet(GLUT_WINDOW_HEIGHT) / 2) * 2.5;
    ripples[rippleCount].x = wx;
    ripples[rippleCount].z = wz;
    ripples[rippleCount].amplitude = 0.3; // Stronger initial ripple
    ripples[rippleCount].radius = 0.0;
    ripples[rippleCount].active = 1;
    rippleCount++;
  }
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'q':
  case 27:
    exit(0);
    break;
  case 'r':
    angle += 5.0;
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Realistic 3D Water Simulator");

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);

  glutMainLoop();
  return 0;
}
