#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIDTH 80
#define HEIGHT 24
#define MAX_SHAPES 100
// Character grid canvas
char canvas[HEIGHT][WIDTH];
// Enum for shape types
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE
} ShapeType;
// Shape parameter structures
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;
typedef struct {
    int xc, yc;
    int r;
} CircleParams;
typedef struct {
    int x, y;
    int w, h;
} RectParams;
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriParams;
// Main Shape struct
typedef struct {
    int id;
    ShapeType type;
    union {
        LineParams line;
        CircleParams circle;
        RectParams rect;
        TriParams tri;
    } params;
} Shape;
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;
// Clear input buffer to handle invalid character inputs gracefully
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Robust integer input scanner with min/max boundaries
int get_int_input(const char* prompt, int min_val, int max_val) {
    int val;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                clear_input_buffer();
                return val;
            }
            printf("Error: Value must be between %d and %d.\n", min_val, max_val);
        } else {
            printf("Error: Invalid input. Please enter an integer.\n");
            clear_input_buffer();
        }
    }
}
// Clear the 2D canvas with background character '_'
void clear_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}
// Safely draw a single point/pixel on the canvas
void draw_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}
// Bresenham's Line Algorithm
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    
    while (1) {
        draw_pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
// Helper to draw points symmetrically in 8 octants for circle drawing
void draw_circle_points(int xc, int yc, int x, int y) {
    draw_pixel(xc + x, yc + y);
    draw_pixel(xc - x, yc + y);
    draw_pixel(xc + x, yc - y);
    draw_pixel(xc - x, yc - y);
    draw_pixel(xc + y, yc + x);
    draw_pixel(xc - y, yc + x);
    draw_pixel(xc + y, yc - x);
    draw_pixel(xc - y, yc - x);
}
// Midpoint Circle Algorithm (Bresenham's Circle)
void draw_circle(int xc, int yc, int r) {
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    draw_circle_points(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        draw_circle_points(xc, yc, x, y);
    }
}
// Draw a rectangle using simple horizontal & vertical lines
void draw_rectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    // Top and bottom edges
    for (int i = x; i < x + w; i++) {
        draw_pixel(i, y);
        draw_pixel(i, y + h - 1);
    }
    // Left and right edges
    for (int j = y; j < y + h; j++) {
        draw_pixel(x, j);
        draw_pixel(x + w - 1, j);
    }
}
// Draw a triangle outline by connecting three vertices with line segments
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}
// Render all shapes in list onto the canvas
void render_canvas() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(s.params.line.x1, s.params.line.y1, s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_CIRCLE:
                draw_circle(s.params.circle.xc, s.params.circle.yc, s.params.circle.r);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(s.params.rect.x, s.params.rect.y, s.params.rect.w, s.params.rect.h);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s.params.tri.x1, s.params.tri.y1, s.params.tri.x2, s.params.tri.y2, s.params.tri.x3, s.params.tri.y3);
                break;
        }
    }
}
// Display the 2D array of characters row by row
void display_canvas() {
    render_canvas();
    printf("\n");
    // Print upper boundary border
    for (int x = 0; x < WIDTH + 2; x++) printf("-");
    printf("\n");
    for (int y = 0; y < HEIGHT; y++) {
        printf("|"); // left border
        for (int x = 0; x < WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n"); // right border
    }
    // Print lower boundary border
    for (int x = 0; x < WIDTH + 2; x++) printf("-");
    printf("\n");
}
// List all active shapes and their properties
void list_shapes() {
    if (shape_count == 0) {
        printf("\nNo objects in the picture.\n");
        return;
    }
    printf("\n=== Current Objects ===\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("[%d] ID: %d | ", i + 1, s.id);
        switch (s.type) {
            case SHAPE_LINE:
                printf("Line from (%d, %d) to (%d, %d)\n", s.params.line.x1, s.params.line.y1, s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_CIRCLE:
                printf("Circle at (%d, %d), radius = %d\n", s.params.circle.xc, s.params.circle.yc, s.params.circle.r);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle at (%d, %d), size = %dx%d\n", s.params.rect.x, s.params.rect.y, s.params.rect.w, s.params.rect.h);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle vertices: (%d, %d), (%d, %d), (%d, %d)\n", s.params.tri.x1, s.params.tri.y1, s.params.tri.x2, s.params.tri.y2, s.params.tri.x3, s.params.tri.y3);
                break;
        }
    }
}
// Create a new shape object
void add_shape() {
    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached!\n");
        return;
    }
    printf("\n--- Add a New Object ---\n");
    printf("1. Line\n");
    printf("2. Circle\n");
    printf("3. Rectangle\n");
    printf("4. Triangle\n");
    int type_choice = get_int_input("Choose type (1-4): ", 1, 4);
    Shape new_shape;
    new_shape.id = next_id++;
    new_shape.type = (ShapeType)type_choice;
    switch (new_shape.type) {
        case SHAPE_LINE:
            printf("Enter Line Coordinates:\n");
            new_shape.params.line.x1 = get_int_input("  Start X (0-79): ", 0, WIDTH - 1);
            new_shape.params.line.y1 = get_int_input("  Start Y (0-23): ", 0, HEIGHT - 1);
            new_shape.params.line.x2 = get_int_input("  End X (0-79): ", 0, WIDTH - 1);
            new_shape.params.line.y2 = get_int_input("  End Y (0-23): ", 0, HEIGHT - 1);
            break;
        case SHAPE_CIRCLE:
            printf("Enter Circle Coordinates:\n");
            new_shape.params.circle.xc = get_int_input("  Center X (0-79): ", 0, WIDTH - 1);
            new_shape.params.circle.yc = get_int_input("  Center Y (0-23): ", 0, HEIGHT - 1);
            new_shape.params.circle.r = get_int_input("  Radius (1-50): ", 1, 50);
            break;
        case SHAPE_RECTANGLE:
            printf("Enter Rectangle Coordinates:\n");
            new_shape.params.rect.x = get_int_input("  Top-Left X (0-79): ", 0, WIDTH - 1);
            new_shape.params.rect.y = get_int_input("  Top-Left Y (0-23): ", 0, HEIGHT - 1);
            new_shape.params.rect.w = get_int_input("  Width (1-80): ", 1, WIDTH);
            new_shape.params.rect.h = get_int_input("  Height (1-24): ", 1, HEIGHT);
            break;
        case SHAPE_TRIANGLE:
            printf("Enter Triangle Vertices:\n");
            new_shape.params.tri.x1 = get_int_input("  Vertex 1 X (0-79): ", 0, WIDTH - 1);
            new_shape.params.tri.y1 = get_int_input("  Vertex 1 Y (0-23): ", 0, HEIGHT - 1);
            new_shape.params.tri.x2 = get_int_input("  Vertex 2 X (0-79): ", 0, WIDTH - 1);
            new_shape.params.tri.y2 = get_int_input("  Vertex 2 Y (0-23): ", 0, HEIGHT - 1);
            new_shape.params.tri.x3 = get_int_input("  Vertex 3 X (0-79): ", 0, WIDTH - 1);
            new_shape.params.tri.y3 = get_int_input("  Vertex 3 Y (0-23): ", 0, HEIGHT - 1);
            break;
    }
    shapes[shape_count++] = new_shape;
    printf("Object added successfully with ID %d!\n", new_shape.id);
}
// Modify shape parameters by ID
void modify_shape() {
    if (shape_count == 0) {
        printf("\nNo objects to modify.\n");
        return;
    }
    list_shapes();
    int target_id = get_int_input("Enter the ID of the object to modify: ", 1, 99999);
    int idx = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == target_id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("Error: Object with ID %d not found.\n", target_id);
        return;
    }
    Shape* s = &shapes[idx];
    printf("\nModifying object ID %d...\n", s->id);
    switch (s->type) {
        case SHAPE_LINE:
            printf("Current: Line from (%d, %d) to (%d, %d)\n", s->params.line.x1, s->params.line.y1, s->params.line.x2, s->params.line.y2);
            s->params.line.x1 = get_int_input("  New Start X (0-79): ", 0, WIDTH - 1);
            s->params.line.y1 = get_int_input("  New Start Y (0-23): ", 0, HEIGHT - 1);
            s->params.line.x2 = get_int_input("  New End X (0-79): ", 0, WIDTH - 1);
            s->params.line.y2 = get_int_input("  New End Y (0-23): ", 0, HEIGHT - 1);
            break;
        case SHAPE_CIRCLE:
            printf("Current: Circle at (%d, %d), radius = %d\n", s->params.circle.xc, s->params.circle.yc, s->params.circle.r);
            s->params.circle.xc = get_int_input("  New Center X (0-79): ", 0, WIDTH - 1);
            s->params.circle.yc = get_int_input("  New Center Y (0-23): ", 0, HEIGHT - 1);
            s->params.circle.r = get_int_input("  New Radius (1-50): ", 1, 50);
            break;
        case SHAPE_RECTANGLE:
            printf("Current: Rectangle at (%d, %d), size = %dx%d\n", s->params.rect.x, s->params.rect.y, s->params.rect.w, s->params.rect.h);
            s->params.rect.x = get_int_input("  New Top-Left X (0-79): ", 0, WIDTH - 1);
            s->params.rect.y = get_int_input("  New Top-Left Y (0-23): ", 0, HEIGHT - 1);
            s->params.rect.w = get_int_input("  New Width (1-80): ", 1, WIDTH);
            s->params.rect.h = get_int_input("  New Height (1-24): ", 1, HEIGHT);
            break;
        case SHAPE_TRIANGLE:
            printf("Current: Triangle vertices: (%d, %d), (%d, %d), (%d, %d)\n", s->params.tri.x1, s->params.tri.y1, s->params.tri.x2, s->params.tri.y2, s->params.tri.x3, s->params.tri.y3);
            s->params.tri.x1 = get_int_input("  New Vertex 1 X (0-79): ", 0, WIDTH - 1);
            s->params.tri.y1 = get_int_input("  New Vertex 1 Y (0-23): ", 0, HEIGHT - 1);
            s->params.tri.x2 = get_int_input("  New Vertex 2 X (0-79): ", 0, WIDTH - 1);
            s->params.tri.y2 = get_int_input("  New Vertex 2 Y (0-23): ", 0, HEIGHT - 1);
            s->params.tri.x3 = get_int_input("  New Vertex 3 X (0-79): ", 0, WIDTH - 1);
            s->params.tri.y3 = get_int_input("  New Vertex 3 Y (0-23): ", 0, HEIGHT - 1);
            break;
    }
    printf("Object ID %d modified successfully!\n", s->id);
}
// Delete shape by ID
void delete_shape() {
    if (shape_count == 0) {
        printf("\nNo objects to delete.\n");
        return;
    }
    list_shapes();
    int target_id = get_int_input("Enter the ID of the object to delete: ", 1, 99999);
    int idx = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == target_id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("Error: Object with ID %d not found.\n", target_id);
        return;
    }
    // Shift remaining elements
    for (int i = idx; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    printf("Object ID %d deleted successfully!\n", target_id);
}
int main() {
    clear_canvas();
    
    // Add default demo shape (a house-like structure: rectangle, triangle roof, circle sun)
    // 1. Rectangle base of the house
    shapes[shape_count].id = next_id++;
    shapes[shape_count].type = SHAPE_RECTANGLE;
    shapes[shape_count].params.rect.x = 25;
    shapes[shape_count].params.rect.y = 10;
    shapes[shape_count].params.rect.w = 30;
    shapes[shape_count].params.rect.h = 10;
    shape_count++;
    // 2. Triangle roof
    shapes[shape_count].id = next_id++;
    shapes[shape_count].type = SHAPE_TRIANGLE;
    shapes[shape_count].params.tri.x1 = 25;
    shapes[shape_count].params.tri.y1 = 10;
    shapes[shape_count].params.tri.x2 = 54;
    shapes[shape_count].params.tri.y2 = 10;
    shapes[shape_count].params.tri.x3 = 39;
    shapes[shape_count].params.tri.y3 = 3;
    shape_count++;
    // 3. Circle sun
    shapes[shape_count].id = next_id++;
    shapes[shape_count].type = SHAPE_CIRCLE;
    shapes[shape_count].params.circle.xc = 65;
    shapes[shape_count].params.circle.yc = 6;
    shapes[shape_count].params.circle.r = 4;
    shape_count++;
    printf("Welcome to the 2D ASCII Graphics Editor!\n");
    printf("A default demo scene of a house has been loaded.\n");
    while (1) {
        printf("\n=== 2D ASCII Graphics Editor ===\n");
        printf("1. Display picture\n");
        printf("2. Add an object\n");
        printf("3. Modify an object\n");
        printf("4. Delete an object\n");
        printf("5. List current objects\n");
        printf("6. Exit\n");
        int choice = get_int_input("Select an option (1-6): ", 1, 6);
        switch (choice) {
            case 1:
                display_canvas();
                break;
            case 2:
                add_shape();
                break;
            case 3:
                modify_shape();
                break;
            case 4:
                delete_shape();
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                printf("Exiting editor. Goodbye!\n");
                return 0;
        }
    }
}
