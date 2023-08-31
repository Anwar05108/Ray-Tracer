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


extern int recursion_level;

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

    void normalize(){
        double len = sqrt(x*x + y*y + z*z);
        x /= len;
        y /= len;
        z /= len;
    }

    void scale(){
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

};


class NormalLight{
    public:
    Vector position;
    Color color;
    double fall_Off_Rate;

    NormalLight(){}

    NormalLight(Vector position, Color color, double fall_Off_Rate){
        this->position = position;
        this->color = color;
        this->fall_Off_Rate = fall_Off_Rate;
    }

    void setFallOffRate(double fall_Off_Rate){
        this->fall_Off_Rate = fall_Off_Rate;
    }
};

class SpotLight : public NormalLight{
    public:
    Vector direction;
    double angle;
    SpotLight(){}

    SpotLight(Vector position, Vector direction, double angle, Color color, double fall_Off_Rate){
        this->position = position;
        this->direction = direction;
        this->angle = angle;
        this->color = color;
        this->fall_Off_Rate = fall_Off_Rate;
    }

    void setFallOffRate(double fall_Off_Rate){
        this->fall_Off_Rate = fall_Off_Rate;
    }

   void draw(){
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

    Ray(Vector start, Vector dir){
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
    }

    Ray(const Ray &r){
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
    Object(){}
    virtual void draw(){}
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
};

class Floor : public Object
{
    public:
    double floorWidth, tileWidth;
    Floor(double floorWidth, double tileWidth){
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this->reference_point = Vector(-floorWidth / 2.0, -floorWidth / 2.0, 0);
        this->length = tileWidth;
    }

    void draw() override{
       int num_grid = floorWidth / tileWidth;

        for(int i = 0; i < num_grid; i++){
            for(int j = 0; j < num_grid; j++){
                if((i + j) % 2 == 0){
                    glColor3d(0, 0, 0);
                }
                else{
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

   

};

class Triangle : public Object{
    public:
    Vector a, b, c;

    Triangle(){}
    Triangle(Vector a, Vector b, Vector c){
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw(){
        glColor3d(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        glEnd();
    }

    Vector getNormal(Vector &initialPoint){
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

    Sphere(Vector centre, double radius){
        this->reference_point = centre;
        this->radius = radius;
    }

    void draw(){
        glColor3d(color.r, color.g, color.b);
        glPushMatrix();
            glTranslated(reference_point.x, reference_point.y, reference_point.z);
            glutSolidSphere(radius, 100, 100);
        glPopMatrix();
    }

    Color getColor(Vector &v){
        return this->color;
    }

    Vector getNormal(Vector &initialPoint){
        Vector normal = initialPoint - reference_point;
        normal.normalize();
        return normal;
    }

};


#endif // _1805108_CLASSES_HPP_