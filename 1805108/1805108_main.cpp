#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <bits/stdc++.h>
#include "1805108_util.hpp"
#include "bitmap_image.hpp"

int textureMode = 0;
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
vector<SpotLight> spotLights;
vector<NormalLight> normalLights;

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
// struct point pos; // position of the eye
// struct point l;   // look/forward direction
// struct point r;   // right direction
// struct point u;   // up direction
Vector pos, l, r, u;
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

void capture()
{
    cout << "Hello capture" << endl;
    cout << "image_hieght" << image_height << endl;
    image_width = image_height;
    cout << "image_width" << image_width << endl;
    bitmap_image image(image_height, image_width);
    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }
    double plane_distance = near_distance;
    // double plane_distance = (WINDOW_HEIGHT / 2.0) / tan((fovy / 2.0) * (M_PI / 180.0));
    double window_height = 2 * plane_distance * tan((fovy / 2.0) * (M_PI / 180.0));
    double fovx = fovy * aspectRatio;
    double window_width = 2 * plane_distance * tan((fovx / 2.0) * (M_PI / 180.0));

    Vector top_left = pos + (l * plane_distance) - (r * (window_width / 2.0)) + (u * (window_height / 2.0));

    double du = window_width / (image_width * 1.0);
    double dv = window_height / (image_height * 1.0);

    top_left = top_left + (r * (du / 2.0)) - (u * (dv / 2.0));

    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            int nearest = -1;
            double t_min = 1000000000000000000.0;
            Vector corner = top_left + (r * (j * du)) - (u * (i * dv));
            Vector direction = corner - pos;
            direction.normalize();

            Ray ray(pos, direction);

            Color color;
            for (int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->intersect(ray, color, 0);
                if (t > 0 && t < t_min)
                {
                    t_min = t;
                    nearest = k;
                }
            }

            if (nearest != -1)
            {
                t_min = objects[nearest]->intersect(ray, color, 1);
            }
            color.normalize();
            image.set_pixel(j, i, color.r * 255, color.g * 255, color.b * 255);
        }
    }

    string file_name = "output.bmp";
    image.save_image(file_name);
    cout << "Hello capture ended" << endl;
    image.clear();
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

    pos.x = 0;
    pos.y = -100;
    pos.z = 50;

    // l.x = -1 / sqrt(2);
    l.x = 0;
    // l.y = -1 / sqrt(2);
    l.y = 1;
    l.z = 0;
    u.x = 0;
    u.y = 0;
    u.z = 1;
    // r.x = -1 / sqrt(2);
    // r.y = 1 / sqrt(2);
    // r.z = 0;
    r.x = 1;
    r.y = 0;
    r.z = 0;

    glClearColor(0, 0, 0, 0);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    // near_distance;
    // double far_distance;
    // double fovy;
    // double aspectRatio;
    gluPerspective(fovy, aspectRatio, near_distance, far_distance);
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
    gluPerspective(fovy, aspectRatio, near_distance, far_distance);
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
    case '0':
        capture();
        break;
    case ' ':
        textureMode = 1;
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

    object = new Floor(800, CHECKER_SIZE);
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
            Vector bottom_lower_left_corner;
            inputStream >> bottom_lower_left_corner;
            double side;
            inputStream >> side;
            Color color;
            inputStream >> color;
            double ambient, diffuse, specular, reflection;
            inputStream >> ambient >> diffuse >> specular >> reflection;
            int shininess;
            inputStream >> shininess;
            // contruct 12 triangles to construct cube
            // find the 8 vertices
            Vector bottom_lower_right_corner = bottom_lower_left_corner + Vector(side, 0, 0);
            Vector bottom_upper_left_corner = bottom_lower_left_corner + Vector(0, side, 0);
            Vector bottom_upper_right_corner = bottom_lower_left_corner + Vector(side, side, 0);
            Vector top_lower_left_corner = bottom_lower_left_corner + Vector(0, 0, side);
            Vector top_lower_right_corner = bottom_lower_left_corner + Vector(side, 0, side);
            Vector top_upper_left_corner = bottom_lower_left_corner + Vector(0, side, side);
            Vector top_upper_right_corner = bottom_lower_left_corner + Vector(side, side, side);

            // now we create 12 triangles
            object = new Triangle(bottom_lower_left_corner, bottom_lower_right_corner, bottom_upper_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_lower_left_corner, bottom_upper_left_corner, bottom_upper_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_lower_left_corner, bottom_lower_right_corner, top_lower_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_lower_left_corner, top_lower_left_corner, top_lower_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_lower_left_corner, bottom_upper_left_corner, top_upper_left_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_lower_left_corner, top_lower_left_corner, top_upper_left_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_upper_right_corner, bottom_upper_left_corner, top_upper_left_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_upper_right_corner, top_upper_right_corner, top_upper_left_corner );
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_upper_right_corner, bottom_lower_right_corner, top_lower_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_upper_right_corner, top_upper_right_corner,top_lower_right_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(top_lower_left_corner, top_lower_right_corner, top_upper_left_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(top_lower_right_corner,  top_upper_right_corner, top_upper_left_corner);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);

            objects.push_back(object);


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
            Vector lowest_point_coordinate;
            inputStream >> lowest_point_coordinate;
            double width, height;
            inputStream >> width >> height;
            Color color;
            inputStream >> color;
            double ambient, diffuse, specular, reflection;
            inputStream >> ambient >> diffuse >> specular >> reflection;
            int shininess;
            inputStream >> shininess;
            // calculate the 6 triangles needed to draw the pyramid
            // 1. 2 bottom triangles
            Vector bottom_left_near = lowest_point_coordinate;
            Vector bottom_right_near = lowest_point_coordinate + Vector(width, 0, 0);
            Vector bottom_left_far = lowest_point_coordinate + Vector(0, width, 0);
            Vector bottom_right_far = lowest_point_coordinate + Vector(width, width, 0);
            Vector top = lowest_point_coordinate + Vector(width / 2.0, width / 2.0, height);
            // now we create 6 triangles
            object = new Triangle(bottom_left_near, bottom_right_near, top);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_right_near, bottom_right_far, top);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_right_far, bottom_left_far, top);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_left_far, bottom_left_near, top);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_left_near, bottom_right_near, bottom_left_far);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);

            object = new Triangle(bottom_right_near, bottom_right_far, bottom_left_far);
            object->setColor(color);
            object->setCoefficients(ambient, diffuse, specular, reflection);
            object->setShininess(shininess);
            objects.push_back(object);
        }
    }

    int num_of_normal_light_sources;
    inputStream >> num_of_normal_light_sources;

    for (int i = 0; i < num_of_normal_light_sources; i++)
    {
        Vector light_source_coordinate;
        inputStream >> light_source_coordinate;

        Color color;
        color = Color(1, 1, 1);

        double fallOffRate;
        inputStream >> fallOffRate;

        NormalLight normal_light(light_source_coordinate, color, fallOffRate);
        normalLights.push_back(normal_light);
    }

    int num_of_spot_light_sources;
    inputStream >> num_of_spot_light_sources;
    for (int i = 0; i < num_of_spot_light_sources; i++)
    {
        Vector light_source_coordinate;
        inputStream >> light_source_coordinate;

        Color color;
        color = Color(1, 1, 1);

        double fallOffRate;
        inputStream >> fallOffRate;

        Vector direction;
        inputStream >> direction;

        double cutOffAngle;
        inputStream >> cutOffAngle;

        SpotLight spot_light(light_source_coordinate, direction, cutOffAngle, color, fallOffRate);

        spotLights.push_back(spot_light);
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
    // memory clean up
    for (int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }

    vector<Object *>().swap(objects);
    vector<SpotLight>().swap(spotLights);
    vector<NormalLight>().swap(normalLights);
    
    return 0;
}
