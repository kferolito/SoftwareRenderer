#include <iostream>
#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

void line(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color) 
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

int main(int argc, char** argv) 
{
    TGAImage image(width, height, TGAImage::RGB);

    model = new Model("obj/african_head.obj");

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    
    delete model;

    return 0;
}

