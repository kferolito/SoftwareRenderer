#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 1200;
const int height = 1200;

struct Line
{
    std::map<int, int> PixelCoordinates;
};

Line ComputeLine(int x1, int y1, int x2, int y2)
{
    Line ComputedLine;

    bool steep = false;
    if (std::abs(x2 - x1) < std::abs(y2 - y1))
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
        steep = true;
    }
    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int x = x1;
    int y = y1;

    const int dx = x2 - x1;
    const int dy = y2 - y1;
    const int dp = 2 * std::abs(dy);

    int derror = 0;

    while (x <= x2)
    {
        if (steep)
            ComputedLine.PixelCoordinates.emplace(y, x);
        else
            ComputedLine.PixelCoordinates.emplace(x, y);
        x++;

        derror += dp;
        if (derror > dx)
        {
            derror -= dx * 2;
            y += (y2 > y1 ? 1 : -1);
        }
    }

    return ComputedLine;
}

void DrawLine(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color)
{   
    bool steep = false;
    if (std::abs(x2 - x1) < std::abs(y2 - y1))
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
        steep = true;
    }
    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int x = x1;
    int y = y1;

    const int dx = x2 - x1;
    const int dy = y2 - y1;
    const int dp = 2 * std::abs(dy);

    int derror = 0;

    while (x <= x2)
    {
        if(steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        x++;

        derror += dp;
        if(derror > dx)
        {
            derror -= dx * 2;
            y += (y2 > y1 ? 1 : -1);
        }
    }
}

void DrawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, TGAColor color) 
{
    if (v0.x > v1.x)
        std::swap(v0, v1);
    if (v0.x > v2.x)
        std::swap(v0, v2);
    if (v1.x > v2.x)
        std::swap(v1, v2);

    Line TriangleEdge1 = ComputeLine(v0.x, v0.y, v1.x, v1.y);
    Line TriangleEdge2 = ComputeLine(v1.x, v1.y, v2.x, v2.y);
    Line TriangleEdge3 = ComputeLine(v0.x, v0.y, v2.x, v2.y);

    for (int x = v0.x; x <= v2.x; x++)
    {
        if (x > std::prev(TriangleEdge1.PixelCoordinates.end())->first)
            DrawLine(x, TriangleEdge3.PixelCoordinates[x], x, TriangleEdge2.PixelCoordinates[x], image, color);
        else
            DrawLine(x, TriangleEdge3.PixelCoordinates[x], x, TriangleEdge1.PixelCoordinates[x], image, color);
    }
}

int main(int argc, char** argv) 
{
    TGAImage image(width, height, TGAImage::RGB);

    model = new Model("obj/african_head.obj");

    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            DrawTriangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    
    delete model;

    return 0;
}

