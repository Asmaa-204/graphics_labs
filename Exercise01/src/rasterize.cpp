#include "rasterize.hpp"

#include <stdio.h>
#include <string.h>
#include <glm/glm.hpp>

// Some helper functions that you may need
template<typename T>
void swap(T& a, T& b) { T temp = a; a = b; b = temp; }
template<typename T>
T min(T a, T b) { return a < b ? a : b; }
template<typename T>
T max(T a, T b) { return a > b ? a : b; }

void draw_point(const glm::ivec2& point, const Color& color, Image<Color>& framebuffer){
    framebuffer.set_pixel(point.x, point.y, color);
}

// Note: It is recommended that you apply bresenham's line drawing algorithm here
void draw_line(const glm::ivec2 line[2], const Color& color, Image<Color>& framebuffer) {
    glm::ivec2 p2 = line[1];
    glm::ivec2 p1 = line[0];

    if(p2.x == p1.x && p2.y == p1.y) {
        draw_point(p1, color, framebuffer);
        return;
    }
    
    if(abs(p2.x - p1.x) > abs(p2.y - p1.y)) {
        if(p1.x > p2.x) {
            swap(p1, p2);
        }
        
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int change = dy > 0 ? 1 : -1;
        dy *= change;

        int f = dx - 2 * dy;
        int y = p1.y;
        
        for (int x = p1.x; x <= p2.x; x++) {
            framebuffer.set_pixel(x, y, color);
            if (f < 0) {
                y += change;
                f += 2 * dx - 2 * dy;
            }
            else {
                f -= 2 * dy;
            }
        }
    }

    else {
        if(p1.y > p2.y) {
            swap(p1, p2);
        }
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int change = dx > 0 ? 1 : -1;
        dx *= change;

        int f = dy - 2 * dx;
        int x = p1.x;

        for (int y = p1.y; y <= p2.y; y++) {
            framebuffer.set_pixel(x, y, color);
            if (f < 0) {
                x += change;
                f += 2 * dy - 2 * dx;
            }
            else {
                f -= 2 * dx;
            }
        }
    }
}

// Note: It is recommended that you apply the scanline triangle rasterization algorithm here
void draw_triangle(const glm::ivec2 triangle[3], const Color& color, Image<Color>& framebuffer) {
    glm::ivec2 p1 = triangle[0], p2 = triangle[1], p3 = triangle[2];

    if (p2.y < p1.y) swap(p1, p2);
    if (p3.y < p1.y) swap(p1, p3);
    if (p3.y < p2.y) swap(p2, p3);

   float x_min = p1.x, x_max = x_min;

    if(p1.y == p3.y) {
        x_min = min(p3.x, min(p2.x, p3.x));
        x_max = max(p3.x, max(p2.x, p3.x));
        for (int x = x_min; x <= x_max; x++) framebuffer.set_pixel(x, p1.y, color);
        return;
    }
    
    float slope12 = float(p2.x - p1.x) / (p2.y - p1.y);
    float slope13 = float(p3.x - p1.x) / (p3.y - p1.y);
    float slope23 = float(p3.x - p2.x) / (p3.y - p2.y);

    bool is_p2_left = slope13 >= slope12;
    float slope_min_bottom, slope_max_bottom, slope_min_top, slope_max_top;

    if(is_p2_left) {
        slope_min_bottom = slope12;
        slope_max_bottom = slope13;
        slope_min_top = slope23;
        slope_max_top = slope13;
    } else {
        slope_min_bottom = slope13;
        slope_max_bottom = slope12;
        slope_min_top = slope13;
        slope_max_top = slope23;
    }

    bool is_flat_bottom = p1.y == p2.y;

    if(is_flat_bottom) {
        x_min = min(p2.x, p1.x);
        x_max = max(p2.x, p1.x);
    }

    for (int y = p1.y; y < p2.y; y++) {
       for (int x = int(round(x_min)); x <= int(round(x_max)); x++) {
           framebuffer.set_pixel(x, y, color);
        }
        x_min += slope_min_bottom;
        x_max += slope_max_bottom;
    }
    
    for (int y = p2.y; y <= p3.y; y++) {
        for (int x = int(round(x_min)); x <= int(round(x_max)); x++) {
            framebuffer.set_pixel(x, y, color);
        }
        x_min += slope_min_top;
        x_max += slope_max_top;
    }
}

/******************************************************************/
/******************************************************************/
/**                                                              **/
/****                                                          ****/
/******                                                      ******/
/******** Below is the function that draws the whole scene ********/
/******                                                      ******/
/****                                                          ****/
/**                                                              **/
/******************************************************************/
/******************************************************************/

// Yes, you have just arrived at the scene drawing function
bool draw_scene(const char *scene_file_path, Image<Color> &framebuffer)
{
    FILE* file = fopen(scene_file_path, "r");
    if(!file){
        printf("Failed to open file: %s\n", scene_file_path);
        return false; // Return false if we failed to open the file
    } else {
        printf("Rendering file: %s\n", scene_file_path);
    }

    // The first line in the scene file should contain the magic string "GFXEX01"
    // followed by the width and height of the image (framebuffer) to which the shapes will be rasterized 
    char magic_string[256];
    int width, height;
    fscanf(file, "%s %i %i", magic_string, &width, &height);
    if(strcmp(magic_string, "GFXEX01") != 0){
        printf("Invalid scene file format\n");
        return false;
    }

    framebuffer.allocate(width, height);

    // Since the scene files store color using the hex notation (e.g. FF0000 means red),
    // we read the hex color as an integer then decodes it in a Color (a 3d vector containing the RGB channels). 
    auto decode_rgb = [](unsigned int encoded) -> Color {
        float r = decode((encoded >> 16) & 0xFF);
        float g = decode((encoded >> 8) & 0xFF);
        float b = decode(encoded & 0xFF);
        return Color(r, g, b);
    };

    char command[256];
    while(!feof(file)){
        fscanf(file, "%s", command);
        
        if(strlen(command) == 0) continue; // Ignore empty lines

        if(strcmp(command, "clear") == 0) { // Clear the framebuffer

            unsigned int encoded_rgb;
            fscanf(file, "%x", &encoded_rgb);
            framebuffer.clear(decode_rgb(encoded_rgb));

        } else if(strcmp(command, "point") == 0) { // Draw a point

            glm::ivec2 point;
            unsigned int encoded_rgb;
            fscanf(file, "%x %i %i", 
                &encoded_rgb,
                &point.x, &point.y
            );
            draw_point(point, decode_rgb(encoded_rgb), framebuffer);
        
        } else if(strcmp(command, "line") == 0) { // Draw a line

            glm::ivec2 line[2];
            unsigned int encoded_rgb;
            fscanf(file, "%x %i %i > %i %i",
                &encoded_rgb,
                &line[0].x, &line[0].y,
                &line[1].x, &line[1].y
            );
            draw_line(line, decode_rgb(encoded_rgb), framebuffer);

        } else if(strcmp(command, "triangle") == 0) { // Draw a line

            glm::ivec2 triangle[3];
            unsigned int encoded_rgb;
            fscanf(file, "%x %i %i > %i %i > %i %i", 
                &encoded_rgb,
                &triangle[0].x, &triangle[0].y,
                &triangle[1].x, &triangle[1].y,
                &triangle[2].x, &triangle[2].y
            );
            draw_triangle(triangle, decode_rgb(encoded_rgb), framebuffer);

        } else if(command[0] == '#') { // Ignore comments

            fscanf(file, "%*[^\n]");

        }
        
    }

    return true;
}
