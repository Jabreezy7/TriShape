/*
 * triangle.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "myLib.c"
#include <math.h>
#include <time.h>
#include "initShader.h"
#include <string.h>
#include <float.h>

int userInput;
char *fileName;

vec4 *sphere(int *num){
    float deg = 10;
    int horiz_steps = 360 / deg;
    int vert_steps = 190 / deg; // 180 + 10 because we need additonal circle at top
    int num_triangles = horiz_steps * vert_steps * 2;
    int num_vts = num_triangles * 3;

    vec4* pos = (vec4*) malloc(num_vts * sizeof(vec4));

    int currPos = 0;

    // Looping bands from bottom to top to create a single sphere
    for(int i = 0; i < vert_steps; i++){
        float v_angle =  90 - (i * deg);
        float next_v_angle = v_angle + deg;

        // Loop rectangle (2 triangles) around 360 deg to create a single band
        for (int j = 0; j < horiz_steps; j++){
            float h_angle = j * deg;
            float next_h_angle = h_angle + deg;

            vec4 base = (vec4){0.0, 0.0, 1.0, 1.0};

            // Bottom left
            mat4 m1 = mat_mat_mult(rotateY(h_angle), rotateX(v_angle));
            vec4 p1 = mat_vec_mult(m1, base);

            // Top left
            mat4 m2 = mat_mat_mult(rotateY(h_angle), rotateX(next_v_angle));
            vec4 p2 = mat_vec_mult(m2, base);

            // Bottom right
            mat4 m3 = mat_mat_mult(rotateY(next_h_angle), rotateX(v_angle));
            vec4 p3 = mat_vec_mult(m3, base);

            // Top right
            mat4 m4 = mat_mat_mult(rotateY(next_h_angle), rotateX(next_v_angle));
            vec4 p4 = mat_vec_mult(m4, base);

            // Triangle 1
            pos[currPos++] = p4;
            pos[currPos++] = p1;
            pos[currPos++] = p2;

            // Triangle 2
            pos[currPos++] = p4;
            pos[currPos++] = p3;
            pos[currPos++] = p1;
        }
    }

    *num = num_vts;
    return pos;
}

vec4 *torus(int *num){
    float deg = 10.0;
    int y_steps = 360 / deg;
    int horiz_steps = 360 / deg;
    int num_triangles_per_band = horiz_steps * 2;
    int num_triangles = num_triangles_per_band * y_steps;
    int num_vts = num_triangles * 3;

    vec4* finalpos = (vec4*) malloc(num_vts * sizeof(vec4));
    vec4* pos = (vec4*) malloc(horiz_steps*2*3 * sizeof(vec4));
    vec4* pos1 = (vec4*) malloc((horiz_steps) * sizeof(vec4));
    vec4* pos2 = (vec4*) malloc((horiz_steps) * sizeof(vec4));


    vec4 base = (vec4) {1.0, 0.0, 0.0, 1.0};
    int currDeg;

    // Create object with points on the outside radius of a circle
    for(int i = 0; i < horiz_steps; i++){
        currDeg = i * deg;
        pos1[i] = mat_vec_mult(rotateZ(currDeg), base);
    }

    // Scale object
    for(int i = 0; i < horiz_steps; i++){
        pos1[i] = mat_vec_mult(scale(0.3, 0.3, 1), pos1[i]);
    }

    // Translate object
    for(int i = 0; i < horiz_steps; i++){
        pos1[i] = mat_vec_mult(translate(0.6, 0.0, 0.0), pos1[i]);
    }

    // Create Object 2
    for(int i = 0; i < horiz_steps; i++){
        pos2[i] = mat_vec_mult(rotateY(deg), pos1[i]);
    }


    // Create one singular band of the torus
    int currPos = 0;
    for(int i = 0; i < horiz_steps; i++){
        // Need to loop back around for last index + 1
        int next = (i + 1) % horiz_steps;

        // Triangle 1
        pos[currPos++] = pos2[next];
        pos[currPos++] = pos1[i];
        pos[currPos++] = pos2[i];

        // Triangle 2
        pos[currPos++] = pos2[next];
        pos[currPos++] = pos1[next];
        pos[currPos++] = pos1[i];
    }

    // Create torus by continously rotating original band by increasing amount of degrees
    currPos = 0;
    for(int i = 0; i < y_steps; i++){
        currDeg = i * deg;
        for(int j = 0; j < horiz_steps*2*3; j++){
            finalpos[currPos++] = mat_vec_mult(rotateY(currDeg), pos[j]);
        }
    }


    free(pos);
    free(pos1);
    free(pos2);
    *num = num_vts;
    return finalpos;
}


vec4 *spring(int *num){
    float deg = 10.0;
    int loops = 5;
    int y_steps = (360 / deg) * loops;
    int horiz_steps = 360 / deg;
    int num_triangles_per_band = horiz_steps * 2;
    int num_triangles = num_triangles_per_band * y_steps;
    int num_vts = num_triangles * 3;

    vec4* finalpos = (vec4*) malloc(num_vts * sizeof(vec4));
    vec4* pos1 = (vec4*) malloc((horiz_steps) * sizeof(vec4));
    vec4* pos2 = (vec4*) malloc((horiz_steps) * sizeof(vec4));


    vec4 base = (vec4) {1.0, 0.0, 0.0, 1.0};
    int currDeg;

    for(int i = 0; i < horiz_steps; i++){
        currDeg = i * deg;
        pos1[i] = mat_vec_mult(rotateZ(currDeg), base);
        pos1[i] = mat_vec_mult(scale(0.1, 0.1, 1), pos1[i]);
        pos1[i] = mat_vec_mult(translate(0.6, 0.0, 0.0), pos1[i]);
    }

    for(int i = 0; i < horiz_steps; i++){
        pos2[i] = mat_vec_mult(rotateY(deg), pos1[i]);
    }

    int currPos = 0;
    float translateRate = .01;
    float currTr;
    float nextTr;
    for(int i = 0; i < y_steps; i++){
        currDeg = i * deg;
        currTr = i * translateRate;
        nextTr = (i+1) * translateRate;
        for(int j = 0; j < horiz_steps; j++){
            int next = (j + 1) % horiz_steps;

            // Triangle 1
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,nextTr,0),rotateY(currDeg)),pos2[next]);
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,currTr,0),rotateY(currDeg)),pos1[j]);
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,nextTr,0),rotateY(currDeg)),pos2[j]);

            // Triangle 2
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,nextTr,0),rotateY(currDeg)),pos2[next]);
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,currTr,0),rotateY(currDeg)),pos1[next]);
            finalpos[currPos++] = mat_vec_mult(mat_mat_mult(translate(0,currTr,0),rotateY(currDeg)),pos1[j]);
        }
    }

    // Scale and translate spring so that it is in full view
    for(int i = 0; i < num_vts; i++){
        finalpos[i] = mat_vec_mult(mat_mat_mult(scale(0.7,0.7,0.7),translate(0,-0.9,0)), finalpos[i]);
    }


    free(pos1);
    free(pos2);
    *num = num_vts;
    return finalpos;
}

mat4 scale_matrix;

vec4 *readFromFile(int *num){
    
    FILE* file = fopen(fileName, "rb");
    // FILE* file = fopen("Little-darth-vader.STL", "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Skip the 80-byte header
    fseek(file, 80, SEEK_SET);

    // Read the number of triangles
    uint32_t numTriangles;
    fread(&numTriangles, sizeof(uint32_t), 1, file);

    // Allocate memory for vertices (3 vertices per triangle)
    int num_vts = numTriangles * 3;
    vec4 *pos = (vec4*) malloc(sizeof(vec4) * num_vts);
    if (!pos) {
        perror("Failed to allocate memory");
        fclose(file);
        num_vts = 0;
        return pos;
    }

    float scaleMax = -FLT_MAX;
    float xMin = FLT_MAX;
    float xMax = -FLT_MAX;
    float yMin = FLT_MAX;
    float yMax = -FLT_MAX;
    float zMin = FLT_MAX;
    float zMax = -FLT_MAX;

    for (uint32_t i = 0; i < numTriangles; i++) {
        float normal[3];
        float vertex[3];

        // Read normal vector (ignored here, but you could store it)
        fread(normal, sizeof(float), 3, file);

        // Read 3 vertices
        for (int v = 0; v < 3; v++) {
            fread(vertex, sizeof(float), 3, file);
            pos[i*3 + v].x = vertex[0];
            pos[i*3 + v].y = vertex[1];
            pos[i*3 + v].z = vertex[2];
            pos[i*3 + v].w = 1.0f; // homogeneous coordinate


            if(vertex[0] < xMin){
                xMin = vertex[0];
            }
            if(vertex[0] > xMax){
                xMax = vertex[0];
            }

            if(vertex[1] < yMin){
                yMin = vertex[1];
            }
            if(vertex[1] > yMax){
                yMax = vertex[1];
            }

            if(vertex[2] < zMin){
                zMin = vertex[2];
            }
            if(vertex[2] > zMax){
                zMax = vertex[2];
            }

            if(fabs(vertex[0]) > scaleMax){
                scaleMax = fabs(vertex[0]);
            }

            if(fabs(vertex[1]) > scaleMax){
                scaleMax = fabs(vertex[1]);
            }

            if(fabs(vertex[2]) > scaleMax){
                scaleMax = fabs(vertex[2]);
            }
        }

        // Skip attribute byte count (2 bytes)
        fseek(file, 2, SEEK_CUR);
    }

    float xMid = (xMin + xMax) / 2;
    float yMid = (yMin + yMax) / 2;
    float zMid = (zMin + zMax) / 2;

    // printf("This is the smallest x: %f\n", xMin);
    // printf("This is the largest x: %f\n", xMax);
    // printf("This is the smallest y: %f\n", yMin);
    // printf("This is the largest y: %f\n", yMax);
    // printf("This is the smallest z: %f\n", zMin);
    // printf("This is the largest z: %f\n", zMax);

    // printf("The x middle point is: %f\n", xMid);
    // printf("The y middle point is: %f\n", yMid);
    // printf("The z middle point is: %f\n", zMid);

    mat4 translate_matrix = translate(-xMid, -yMid, -zMid);

    mat4 scale_matrix = scale(1/scaleMax, 1/scaleMax, 1/scaleMax);

    for(int i = 0; i < num_vts; i++){
        pos[i] = mat_vec_mult(mat_mat_mult(scale_matrix,translate_matrix), pos[i]);
    }

    fclose(file);
    *num = num_vts;
    return pos;
}


int num_vertices;
int num_vertices_sphere;
int num_vertices_torus;
int num_vertices_spring;

mat4 my_ctm = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
GLuint ctm_location;

void init(void)
{
    vec4 *positions;

    if(userInput == 1){
        // Find num of vertices for each object
        vec4 *positionsSphere = sphere(&num_vertices_sphere);
        vec4 *positionsTorus = torus(&num_vertices_torus);
        vec4 *positionsSpring = spring(&num_vertices_spring);

        // Calculate total number of vertices for all objects
        num_vertices = num_vertices_sphere + num_vertices_torus + num_vertices_spring;

        // Copy all three objects' postions into one final positions array
        positions = (vec4*) malloc(num_vertices * sizeof(vec4));
        memcpy(positions, positionsSphere, sizeof(vec4)*num_vertices_sphere);
        memcpy(positions + num_vertices_sphere, positionsTorus, sizeof(vec4) * num_vertices_torus);
        memcpy(positions + num_vertices_sphere + num_vertices_torus, positionsSpring, sizeof(vec4)*num_vertices_spring);

        free(positionsSphere);
        free(positionsTorus);
        free(positionsSpring);
    }
    else if(userInput == 2){
        positions = readFromFile(&num_vertices);
        printf("This is the number of vertices: %d\n", num_vertices);
    }


    vec4 *colors = (vec4*) malloc(num_vertices * sizeof(vec4));

    srand(time(NULL));
    for(int i = 0; i < (num_vertices / 3); i++){
        float red = (float) rand() / (float) RAND_MAX;
        float green = (float) rand() / (float) RAND_MAX;
        float blue = (float) rand() / (float) RAND_MAX;
        colors[(i*3)] = (vec4) {red, green, blue, 1};
        colors[(i*3)+1] = (vec4) {red, green, blue, 1};
        colors[(i*3)+2] = (vec4) {red, green, blue, 1};
    }

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    #ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    #else
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    #endif

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_vertices * sizeof(vec4), positions);
    glBufferSubData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4), num_vertices * sizeof(vec4), colors);

    free(positions);
    free(colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (num_vertices * sizeof(vec4)));

    ctm_location = glGetUniformLocation(program, "ctm");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

int input = 0;
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &my_ctm);

    if(input == 1){
        glDrawArrays(GL_TRIANGLES, 0, num_vertices_sphere);
    }

    if(input == 2){
        glDrawArrays(GL_TRIANGLES, num_vertices_sphere, num_vertices_torus);
    }

    if(input == 3){
        glDrawArrays(GL_TRIANGLES, num_vertices_sphere + num_vertices_torus, num_vertices_spring);
    }

    if(userInput == 2){
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }


    
    glutPostRedisplay();

    glutSwapBuffers();
}

mat4 prev =  {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	exit(0);

    if(key == '1' && userInput == 1){
        input = 1;
    }

    if(key == '2' && userInput == 1){
        input = 2;
    }

    if(key == '3' && userInput == 1){
        input = 3;
    }

    if(key == 'w'){
        my_ctm = mat_mat_mult(scale(1.02, 1.02, 1.02), prev);
        prev = my_ctm;
    }

    if(key == 's'){
        my_ctm = mat_mat_mult(scale(1.0/1.02, 1.0/1.02, 1.0/1.02), prev);
        prev = my_ctm;
    }

    glutPostRedisplay();
}

vec4 v1;
vec4 v2;
int outOfBounds;
float boundDetector;


void mouse(int button, int state, int x, int y){
    // printf("Button: %d, State: %d, Coordinate(%d, %d)\n", button, state, x, y);

    float glx = (x / 400.0) - 1;
    float gly = 1 - (y / 400.0);

    if(state == 0){
        float glz = calcZ(glx, gly);
        v1 = (vec4){glx, gly, glz, 0};
    }

    if(state == 1){
        prev = my_ctm;
    }

    glutPostRedisplay();
}

void motion(int x, int y){

    // Turn window coordinate into openGL coordinate
    float glx = (x / 400.0) - 1;
    float gly = 1 - (y / 400.0);

    // Check if user is dragging on the sphere
    boundDetector = glx*glx + gly*gly;
    if(boundDetector > 1){
        outOfBounds = 1;
    }
    else{
        outOfBounds = 0;
    }

    // Calculate z and turn point into a vector
    float glz = calcZ(glx, gly);
    v2 = (vec4){glx, gly, glz, 0};

    // Calculate about vector and normalize it
    vec4 aboutV = cross_product(v1, v2);
    aboutV = vec_norm(aboutV);

    // Calculate x rotation matrix
    float d = sqrtf(aboutV.y * aboutV.y + aboutV.z * aboutV.z);
    mat4 x_matrix = {{1,0,0,0}, {0,(aboutV.z/d),(aboutV.y/d),0}, {0,(-aboutV.y/d),(aboutV.z/d),0}, {0,0,0,1}};

    // Calculate y rotation matrix
    d = sqrtf(1 - aboutV.x * aboutV.x);
    mat4 y_matrix = {{d,0,aboutV.x,0}, {0,1,0,0}, {-aboutV.x,0,d,0}, {0,0,0,1}};

    // Calculate z rotation matrix
    float z_angle = (acosf(dot_product(v1, v2))) * (180.0/M_PI);
    mat4 z_matrix = rotateZ(z_angle);

    if (v1.x*v1.x + v1.y*v1.y >= 1){
        v1 = v2;
    }

    int isSame;
    if (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z) {
        isSame = 1;
    }
    else{
        isSame = 0;
    }

    // Apply transformation
    if(outOfBounds == 0 && isSame == 1){
        my_ctm = mat_mat_mult(rotateArbitrary(z_matrix, y_matrix, x_matrix), prev);
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    printf("Main Menu\n");
    printf("    1. View computer generated objects\n");
    printf("    2. View an object from a file\n");


    scanf("%d", &userInput);
    while(userInput != 1 && userInput != 2){
        printf("Please enter a valid choice\n");
        scanf("%d", &userInput);
    }

    if(userInput == 1){
        printf("Menu\n");
        printf("    1 - Sphere\n");
        printf("    2 - Torus\n");
        printf("    3 - Spring\n");
    }

    if(userInput == 2){
        fileName = malloc(256 * sizeof(char));
        printf("\nFile Input Choice\n");
        printf("    1 - Choose from provided STL files\n");
        printf("    2 - Enter in your own STL file\n");

        int inputOption;
        scanf("%d", &inputOption);
        while(inputOption!= 1 && inputOption!= 2){
            printf("Please enter a valid choice\n");
            scanf("%d", &inputOption);
        }

        if(inputOption == 1){
            printf("\nObject Choice\n");
            printf("    1 - Mini Darth Vader\n");
            printf("    2 - Cathedral of Learning\n");

            int fileOption;
            scanf("%d", &fileOption);
            while(fileOption!= 1 && fileOption!= 2){
                printf("Please enter a valid choice\n");
                scanf("%d", &fileOption);
            }

            if(fileOption == 1){
                fileName = "Little-darth-vader.STL";
            }

            if(fileOption == 2){
                fileName = "CL_whole.stl";
            }

        }

        if(inputOption == 2){
            printf("Please enter file name:\n");
            scanf("%s", fileName);
        }

    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Triangle");
    #ifndef __APPLE__
    glewInit();
    #endif
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}
