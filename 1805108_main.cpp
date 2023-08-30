#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <bits/stdc++.h>
#include "1805108_classes.hpp"

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;
float CHECKER_SIZE = 2.0f; // Size of each checker square

double near_distance;
double far_distance;
double fovy;
double aspectRatio;

int level_recursion;
int image_height; // num_pixels_x_y
int image_width;  // image_width = image_height

double ambient_coefficient_checkboard;
double diffuse_coefficient_checkboard;
double reflection_coefficient_checkboard;

int object_count;

vector<Object *> objects;
// Vector<PointLight> point_lights;
// Vector<SpotLight> spot_lights;

using namespace std;

struct point
{
    GLfloat x, y, z;
};

float a = 1.0f;
float b = 0.0f;
float c = 0.0f;
float rotateX = 0.0f;
float scaleFactor = 1.0f;
float scaleFactorSphere = 1 / sqrt(3);
float centroid = 1 / 3.0f;

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

// Global variables
// float a2 = 0;
// GLfloat eyex = 0, eyey = 0, eyez = 20;
// GLfloat centerx = 0, centery = 0, centerz = 0;
// GLfloat upx = 0, upy = 1, upz = 0;
// GLfloat rightx = 1, righty = 0, rightz = 0;
// Global variables
struct point pos; // position of the eye
struct point l;   // look/forward direction
struct point r;   // right direction
struct point u;   // up direction
bool isAxes = true;

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); // Red
    // X axis
    glVertex3f(500, 0, 0);
    glVertex3f(-500, 0, 0);

    glColor3f(0, 1, 0); // Green
    // Y axis
    glVertex3f(0, 500, 0);
    glVertex3f(0, -500, 0);

    glColor3f(0, 0, 1); // Blue
    // Z axis
    glVertex3f(0, 0, 500);
    glVertex3f(0, 0, -500);
    glEnd();
}

void drawCheckerBoard()
{
    glClear(GL_COLOR_BUFFER_BIT);

    for (int x = 0; x < WINDOW_WIDTH; x += CHECKER_SIZE)
    {
        for (int y = 0; y < WINDOW_HEIGHT; y += CHECKER_SIZE)
        {
            if (((int)(x / CHECKER_SIZE) + (int)(y / CHECKER_SIZE)) % 2 == 0)
            {
                glColor3f(1.0f, 1.0f, 1.0f); // White
            }
            else
            {
                glColor3f(0.0f, 0.0f, 0.0f); // Black
            }

            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + CHECKER_SIZE, y);
            glVertex2f(x + CHECKER_SIZE, y + CHECKER_SIZE);
            glVertex2f(x, y + CHECKER_SIZE);
            glEnd();
        }
    }
}

void display()
{
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    // gluLookAt(eyex,eyey,eyez,
    //           centerx,centery,centerz,
    //           upx,upy,upz);
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);
    // draw
    // glRotatef(rotateX, 0, 1, 0);

    if (isAxes)
        drawAxes();
    // drawCheckerBoard();

    // draw the objects
    for (int i = 0; i < objects.size(); i++)
    {
        // drawing objects
        objects[i]->draw();
    }

    glutSwapBuffers(); // Render now
}

void animate()
{
    glutPostRedisplay();
}

void init()
{
    glClearColor(0, 0, 0, 0);

    pos.x = 200;
    pos.y = 200;
    pos.z = 5;

    l.x = -1 / sqrt(2);
    l.y = -1 / sqrt(2);
    l.z = 0;
    u.x = 0;
    u.y = 0;
    u.z = 1;
    r.x = -1 / sqrt(2);
    r.y = 1 / sqrt(2);
    r.z = 0;

    glClearColor(0, 0, 0, 0);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(80, 1, 1, 1000.0);
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    /*if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }*/
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
    float v = 0.1;
    double rate = 0.1;
    double s;

    switch (key)
    {
    // Control eye (location of the eye)
    // control eyex
    case ',':
        if (scaleFactor > 0)
        {
            // a2 += 0.1;
            scaleFactor -= 0.05;
            // scaleFactorSphere += 0.1;
        }

        break;
    case '.':
        if (scaleFactor < 1)
        {
            scaleFactor += 0.05;
            // scaleFactorSphere -= 0.1;
            // a2 -= 0.1;
        }

        break;

    case '1':
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;

    case '2':
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;

    case '3':
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;

    case '4':
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;

    case '5':
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;

    case '6':
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;
    case 'a':
        // rotate the object in the clockwise direction about its own axis

        break;
    // Control exit
    case 27:     // ESC key
        exit(0); // Exit window
        break;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP: // down arrow key
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;
        break;
    case GLUT_KEY_DOWN: // up arrow key
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;
        break;

    case GLUT_KEY_RIGHT:
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;
    case GLUT_KEY_LEFT:
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

void takeInput()
{
    ifstream inputStream;
    inputStream.open("description.txt");

    if (!inputStream.is_open())
    {
        cout << "File not found" << endl;
        exit(0);
    }

    inputStream >> near_distance >> far_distance >> fovy >> aspectRatio;
    inputStream >> level_recursion;
    inputStream >> image_height;
    WINDOW_WIDTH = image_height;
    WINDOW_HEIGHT = image_height;

    inputStream >> CHECKER_SIZE;

    inputStream >> ambient_coefficient_checkboard >> diffuse_coefficient_checkboard >> reflection_coefficient_checkboard;

    inputStream >> object_count;

    Object *object;

    object = new Floor(800, 20);
    object->setCoefficients(ambient_coefficient_checkboard, diffuse_coefficient_checkboard, 0, reflection_coefficient_checkboard);
    objects.push_back(object);

    // declare an array of 3 strings
    string object_type;

    // a loop of object_count

    // cout << near_distance << " " << far_distance << " " << fovy << " " << aspectRatio << endl;
    // cout << level_recursion << endl;
    // cout << image_height << endl;
    // cout << CHECKER_SIZE << endl;

    // cout << "object_count" << endl;
    // cout << object_count << endl;

    for (int i = 0; i < object_count; i++)
    {
        inputStream >> object_type;
        cout << object_type << endl;
        if (object_type == "cube")
        {
            Point bottom_lower_left_corner;
            inputStream >> bottom_lower_left_corner.x >> bottom_lower_left_corner.y >> bottom_lower_left_corner.z;
            double side;
            inputStream >> side;
            Color color;
            inputStream >> color;
            double ambient, diffuse, specular, reflection;
            inputStream >> ambient >> diffuse >> specular >> reflection;
            int shininess;
            inputStream >> shininess;
        }
        else if (object_type == "sphere")
        {
            cout << "inside sphere" << endl;
            Vector centre;
            inputStream >> centre;
            cout << centre.x << centre.y << centre.z << endl;

            double radius;
            inputStream >> radius;

            Color color;
            inputStream >> color;

            double ambient, diffuse, specular, reflection;
            inputStream >> ambient >> diffuse >> specular >> reflection;

            int shininess;
            inputStream >> shininess;

            object = new Sphere(centre, radius);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);

            objects.push_back(object);
        }
        else if (object_type == "pyramid")
        {
            Point lowest_point_coordinate;
            inputStream >> lowest_point_coordinate.x >> lowest_point_coordinate.y >> lowest_point_coordinate.z;
            double width, height;
            inputStream >> width >> height;
            Color color;
            inputStream >> color;
            double ambient, diffuse, specular, reflection;
            inputStream >> ambient >> diffuse >> specular >> reflection;
            int shininess;
            inputStream >> shininess;
        }
    }

    cout << "Hello input completed" << endl;
    // print the objects
    for (int i = 0; i < objects.size(); i++)
    {
        cout << objects[i]->reference_point << endl;
        cout << objects[i]->height << endl;
        cout << objects[i]->width << endl;
        cout << objects[i]->length << endl;
        cout << objects[i]->color << endl;
        cout << objects[i]->ambient << " " << objects[i]->diffuse << " " << objects[i]->specular << " " << objects[i]->reflection << endl;
        cout << objects[i]->shininess << endl;
    }

    cout << "Hello input completed" << endl;
    cout << objects.size() << endl;

    inputStream.close();

    cout << "Hello input completed" << endl;
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    cout << "Hello" << endl;
    takeInput();

    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitWindowSize(640, 640);                             // Set the window's initial width & height
    glutInitWindowPosition(0, 0);                             // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");
    // Create a window with the given title

    init();

    glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
    glutDisplayFunc(display); // Register display callback handler for window re-paint

    glutIdleFunc(animate); // Register display callback handler for window re-paint
    // glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);  // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener); // Register callback handler for special-key event
                                         // Our own OpenGL initialization
    glutMainLoop();                      // Enter the event-processing loop
    return 0;
}
