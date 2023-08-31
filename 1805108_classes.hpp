#ifndef _1805108_CLASSES_HPP_
#define _1805108_CLASSES_HPP_

#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

class Point;
class Ray;
class Color;
class Vector;
class Object;
class Floor;
class Triangle;
class Sphere;
class NormalLight;
class SpotLight;

extern int level_recursion;

extern vector<Object *> objects;
extern vector<NormalLight> normalLights;
extern vector<SpotLight> spotLights;

class Point
{
public:
    double x, y, z;
    Point()
    {
        x = y = z = 0;
    }
    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

class Color
{
public:
    double r, g, b;
    Color()
    {
        r = g = b = 0;
    }
    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    Color operator+(const Color &c)
    {
        return Color(r + c.r, g + c.g, b + c.b);
    }

    Color operator*(const Color &c)
    {
        return Color(r * c.r, g * c.g, b * c.b);
    }

    Color operator*(const double c)
    {
        return Color(r * c, g * c, b * c);
    }

    void normalize()
    {
        r = min(r, 1.0);
        g = min(g, 1.0);
        b = min(b, 1.0);
    }

    friend ostream &operator<<(ostream &os, const Color &c)
    {
        os << c.r << " " << c.g << " " << c.b;
        return os;
    }

    friend istream &operator>>(istream &is, Color &c)
    {
        is >> c.r >> c.g >> c.b;
        return is;
    }
};

class Vector
{
public:
    double x, y, z, w;
    Vector()
    {
        x = y = z = 0;
        w = 0;
    }

    Vector(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        w = 1.0;
    }

    Vector(const Vector &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }

    void normalize()
    {
        double len = sqrt(x * x + y * y + z * z);
        x /= len;
        y /= len;
        z /= len;
    }

    void scale()
    {
        this->x /= this->w;
        this->y /= this->w;
        this->z /= this->w;
        this->w = 1.0;
    }

    Vector operator+(const Vector &v)
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    Vector operator-(const Vector &v)
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    Vector operator*(const double c)
    {
        return Vector(x * c, y * c, z * c);
    }

    double operator*(const Vector &v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector operator^(const Vector &v)
    {
        return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    friend ostream &operator<<(ostream &os, const Vector &v)
    {
        os << v.x << " " << v.y << " " << v.z;
        return os;
    }

    friend istream &operator>>(istream &is, Vector &v)
    {
        is >> v.x >> v.y >> v.z;
        return is;
    }
    double distance(Vector &v)
    {
        return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
    }
};

class NormalLight
{
public:
    Vector position;
    Color color;
    double fall_Off_Rate;

    NormalLight() {}

    NormalLight(Vector position, Color color, double fall_Off_Rate)
    {
        this->position = position;
        this->color = color;
        this->fall_Off_Rate = fall_Off_Rate;
    }

    void setFallOffRate(double fall_Off_Rate)
    {
        this->fall_Off_Rate = fall_Off_Rate;
    }
};

class SpotLight : public NormalLight
{
public:
    Vector direction;
    double angle;
    SpotLight() {}

    SpotLight(Vector position, Vector direction, double angle, Color color, double fall_Off_Rate)
    {
        this->position = position;
        this->direction = direction;
        this->angle = angle;
        this->color = color;
        this->fall_Off_Rate = fall_Off_Rate;
    }

    void setFallOffRate(double fall_Off_Rate)
    {
        this->fall_Off_Rate = fall_Off_Rate;
    }

    void draw()
    {
        glColor3d(color.r, color.g, color.b);
        glPushMatrix();
        glTranslated(position.x, position.y, position.z);
        glutSolidSphere(0.5, 100, 100);
        glPopMatrix();
    }
};

class Ray
{
public:
    Vector start, dir;

    Ray() {}

    Ray(Vector start, Vector dir)
    {
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
    }

    Ray(const Ray &r)
    {
        start = r.start;
        dir = r.dir;
    }
};

class Object
{
public:
    Vector reference_point;
    Color color;
    double height, width, length;
    double ambient, diffuse, specular, reflection, shininess;
    Object() {}
    virtual void draw() {}
    void setColor(Color c)
    {
        color = c;
    }
    void setCoefficients(double a, double d, double s, double r)
    {
        ambient = a;
        diffuse = d;
        specular = s;
        reflection = r;
    }
    void setShininess(double s)
    {
        shininess = s;
    }

    virtual Color getColor(Vector &v)
    {
        return this->color;
    }

    double intersect(Ray &ray, Color &color, int level_of_recursion)
    {
        double t_min = this->getT(ray);
        if (level_of_recursion == 0)
        {
            return t_min;
        }

        Vector initial_Point;
        initial_Point = ray.start + ray.dir * t_min;

        color = this->getColor(initial_Point) * this->ambient;
        color.normalize();

        Vector normal = this->getNormal(initial_Point);
        normal.normalize();

        // illumination and casting
        double lambert = 0.0, phong = 0.0;
        for (int i = 0; i < normalLights.size(); i++)
        {
            Vector lightDir = normalLights[i].position - initial_Point;
            double lightDistance = normalLights[i].position.distance(initial_Point);
            lightDir.normalize();

            Vector lightPosition;
            lightPosition = initial_Point + lightDir * 0.00000001;

            Ray lightRay(lightPosition, lightDir);

            bool isShadow = false;
            Color tempColor;
            double t, t_min_orgininal = 1000000000;
            for (int j = 0; j < objects.size(); j++)
            {
                t = objects[j]->intersect(lightRay, tempColor, 0);
                if (t > 0 && t < t_min_orgininal)
                {
                    t_min_orgininal = t;
                }
            }

            // when we are not in shadow region
            if (t_min < t_min_orgininal)
            {
                Vector R = normal * (normal * lightDir) * 2 - lightDir;
                R.normalize();
                double scalingFactor = exp(-lightDistance * lightDistance * normalLights[i].fall_Off_Rate);
                lambert += max(normal * lightDir, 0.0) * scalingFactor;
                phong += max(pow(R * ray.dir, shininess), 0.0) * scalingFactor;

                color = color + this->getColor(initial_Point) * (normalLights[i].color * lambert * this->diffuse);
                color.normalize();

                color = color + (normalLights[i].color * phong * this->specular);
                color.normalize();
            }
        }

        // spot light
        for (int i = 0; i < spotLights.size(); i++)
        {
            Vector lightDir = spotLights[i].direction;
            double lightDistance = lightDir * lightDir;
            lightDir.normalize();

            Vector light_to_intersect = initial_Point - spotLights[i].position;
            light_to_intersect.normalize();

            double angle = acos(light_to_intersect * spotLights[i].direction) * 180 / acos(-1);

            if (angle > spotLights[i].angle)
            {
                continue;
            }

            // same as normal light
            lightDir = spotLights[i].position - initial_Point;
            lightDistance = spotLights[i].position.distance(initial_Point);
            lightDir.normalize();

            Vector lightPosition;
            lightPosition = initial_Point + lightDir * 0.001;

            Ray lightRay(lightPosition, lightDir);

            bool isShadow = false;
            Color tempColor;
            double t, t_min_orgininal = 1000000000;
            for (int j = 0; j < objects.size(); j++)
            {
                t = objects[j]->intersect(lightRay, tempColor, 0);
                if (t > 0 && t < t_min_orgininal)
                {
                    t_min_orgininal = t;
                }
            }

            // when we are not in shadow region
            if (t_min < t_min_orgininal)
            {
                Vector R = normal * (normal * lightDir) * 2 - lightDir;
                R.normalize();
                double scalingFactor = exp(-lightDistance * spotLights[i].fall_Off_Rate);
                lambert += max(normal * lightDir, 0.0) * scalingFactor;
                phong += max(pow(R * ray.dir, shininess), 0.0) * scalingFactor;

                color = color + this->getColor(initial_Point) * (normalLights[i].color * lambert * this->diffuse);
                color.normalize();

                color = color + (normalLights[i].color * phong * this->specular);
                color.normalize();
            }
        }

        // reflection recursive
        if (level_of_recursion > level_recursion) // checking with the input level_recursion
        {
            return t_min;
        }

        Vector R = ray.dir - normal * 2.0 * (normal * ray.dir);
        R.normalize();

        Vector R_start;

        R_start = initial_Point + R * 0.001;
        Ray reflectionRay(R_start, R);

        // find out nearest intersected objects
        Color reflectionColor;
        double t_min_reflection = 1000000000;
        int nearestObjectIndex = -1;
        for (int i = 0; i < objects.size(); i++)
        {
            double t = objects[i]->intersect(reflectionRay, reflectionColor, 0);
            if (t > 0 && t < t_min_reflection)
            {
                t_min_reflection = t;
                nearestObjectIndex = i;
            }
        }

        if (nearestObjectIndex != -1)
        {
            t_min_reflection = objects[nearestObjectIndex]->intersect(reflectionRay, reflectionColor, level_of_recursion + 1);
            color = color + reflectionColor * this->reflection;
            color.normalize();
        }

        return t_min;
    }

    virtual Vector getNormal(Vector &initialPoint)
    {
        Vector normal;
        return normal;
    }

    virtual double getT(Ray &ray)
    {
        double t = -1;
        return t;
    }
};

class Floor : public Object
{
public:
    double floorWidth, tileWidth;
    Floor(double floorWidth, double tileWidth)
    {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this->reference_point = Vector(-floorWidth / 2.0, -floorWidth / 2.0, 0);
        this->length = tileWidth;
    }

    void draw() override
    {
        int num_grid = floorWidth / tileWidth;

        for (int i = 0; i < num_grid; i++)
        {
            for (int j = 0; j < num_grid; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3d(0, 0, 0);
                }
                else
                {
                    glColor3d(255, 255, 255);
                }
                glBegin(GL_QUADS);
                glVertex3f(reference_point.x + j * tileWidth, reference_point.y + i * tileWidth, reference_point.z);
                glVertex3f(reference_point.x + (j + 1) * tileWidth, reference_point.y + i * tileWidth, reference_point.z);
                glVertex3f(reference_point.x + (j + 1) * tileWidth, reference_point.y + (i + 1) * tileWidth, reference_point.z);
                glVertex3f(reference_point.x + j * tileWidth, reference_point.y + (i + 1) * tileWidth, reference_point.z);
                glEnd();
            }
        }
    }

    double getT(Ray &ray)
    {
        Vector normal(0, 0, 1);

        double t = (-1.0) * ((normal * ray.start) / (normal * ray.dir));
        Vector initialPoint = ray.start + ray.dir * t;

        if (initialPoint.x < reference_point.x || initialPoint.x > -reference_point.x)
        {
            return -1;
        }

        if (initialPoint.y < reference_point.y || initialPoint.y > -reference_point.y)
        {
            return -1;
        }

        return t;
    }

    Vector getNormal(Vector &initialPoint) override
    {
        Vector normal(0, 0, 1);
        return normal;
    }

    Color getColor(Vector &v) override
    {
        int x = (v.x - reference_point.x) / tileWidth;
        int y = (v.y - reference_point.y) / tileWidth;
        int c = (x + y) % 2;
        return Color(c, c, c);
        // return Color(0, 0, 0);
        // if ((x + y) % 2 == 0)
        // {
        //     return Color(0, 0, 0);
        // }
        // else
        // {
        //     return Color(255, 255, 255);
        // }
    }
};

class Triangle : public Object
{
public:
    Vector a, b, c;

    Triangle() {}
    Triangle(Vector a, Vector b, Vector c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw()
    {
        glColor3d(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
    }

    double getT(Ray &ray)
    {
        Vector side_ba = b - a;
        Vector side_ca = c - a;

        Vector vert = ray.dir ^ side_ca;
        double det = side_ba * vert;

        if (det > -0.000001 && det < 0.000001)
        {
            return -1;
        }

        double inv_det = 1.0 / det;
        Vector vert_origin = ray.start - a;

        double u = inv_det * (vert_origin * vert);

        if (u < 0.0 || u > 1.0)
        {
            return -1;
        }

        Vector vert_dir = vert_origin ^ side_ba;
        double v = inv_det * (ray.dir * vert_dir);

        if (v < 0.0 || u + v > 1.0)
        {
            return -1;
        }

        double t = inv_det * (side_ca * vert_dir);
        if (t < 0.000001)
        {
            return -1;
        }

        return t;
    }

    Vector getNormal(Vector &initialPoint) override
    {
        Vector normal = (b - a) ^ (c - a);
        normal.normalize();
        return normal;
    }

    // Color getColor(Vector &v){
    //     return this->color;
    // }
};

class Sphere : public Object
{
public:
    double radius;

    Sphere(Vector centre, double radius)
    {
        this->reference_point = centre;
        this->radius = radius;
    }

    void draw()
    {
        glColor3d(color.r, color.g, color.b);
        glPushMatrix();
        glTranslated(reference_point.x, reference_point.y, reference_point.z);
        glutSolidSphere(radius, 100, 100);
        glPopMatrix();
    }

    // Color getColor(Vector &v)
    // {
    //     return this->color;
    // }

    double getT(Ray &ray)
    {
        Vector origin = ray.start - reference_point;
        double a = 1;
        double b = (ray.dir * origin) * 2;
        double c = (origin * origin) - radius * radius;
        
        double d = b * b - 4 * a * c;
        if (d < 0)
        {
            return -1;
        }

        double t1 = (-b + sqrt(d)) / (2 * a);
        double t2 = (-b - sqrt(d)) / (2 * a);

        if (t1 < 0 && t2 < 0)
        {
            return -1;
        }

        if (t1 > 0)
        {
            return t1;
        }

        if (t2 > 0)
        {
            return t2;
        }

        return -1;

    }

    Vector getNormal(Vector &initialPoint)
    {
        Vector normal = initialPoint - reference_point;
        normal.normalize();
        return normal;
    }
};

#endif // _1805108_CLASSES_HPP_