>This answer is to show how I read an image (.raw), <br>
inserted its content to a Framebuffer Object and saved to a .ppm file <br>
without a "screen shot". <br>
So, the output image has the same resolution as the input image.<br>

>I used the GLEW library to deal with the Framebuffer Object. <br>
I hope it can help some one... <br>
Here you are my code: <br>

>*(I deleted part of the code to focus only on the topic of the question.<br>
Therefore, there are some variables that do not make sense!)

```cpp
#include <windows.h>
#define  GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdio>
#include <string>

#define width       2464
#define height      1648

/// Global Variables
static  GLuint texture;
GLuint  fb = 1; ///Frame-buffer Object

/// Headers
GLuint  raw_texture_load();
void    FBO_2_PPM_file();

/// Functions
GLuint raw_texture_load()
{
    GLuint texture;
    unsigned char *data;
    FILE *file;

    // open texture data
    file = fopen("C:\\Dataset\\1_4.raw", "rb");
    if (file == NULL) return 0;

    // allocate buffer
    data = (unsigned char*) malloc(width * height * 3);

    // read texture data
    fread(data, width * height * 3, 1, file);
    fclose(file);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /// Texture
    // allocate a texture name
    glGenTextures(1, &texture);
    // select our current texture
    glBindTexture(GL_TEXTURE_2D, texture);
    // select modulate to mix texture with color for shading
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // texture should tile
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // build our texture mipmapst
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // free buffer
    free(data);

    return texture;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glBindTexture(GL_TEXTURE_2D, texture);

    glFlush();
    glDisable(GL_TEXTURE_2D);

    ///FBO
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    //Attach 2D texture to this FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    /// Saving the results from the FBO (fb) to a PPM image file
    FBO_2_PPM_file();
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    texture = raw_texture_load();

    render();
}

void FBO_2_PPM_file()
{
    FILE    *output_image;
    int     output_width, output_height;

    output_width = width;
    output_height = height;

    /// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
    int             i, j, k;
    unsigned char   *pixels = (unsigned char*)malloc(output_width*output_height*3);

    /// READ THE CONTENT FROM THE FBO
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    output_image = fopen("C:\\Dataset\\output.ppm", "wt");
    fprintf(output_image,"P3\n");
    fprintf(output_image,"# Created by Ricao\n");
    fprintf(output_image,"%d %d\n",output_width,output_height);
    fprintf(output_image,"255\n");

    k = 0;
    for(i=0; i<output_width; i++)
    {
        for(j=0; j<output_height; j++)
        {
            fprintf(output_image,"%u %u %u ",(unsigned int)pixels[k],(unsigned int)pixels[k+1],
                                             (unsigned int)pixels[k+2]);
            k = k+3;
        }
        fprintf(output_image,"\n");
    }
    free(pixels);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(300, 1);
    glutCreateWindow("...");
    glutHideWindow();

    /// Checking GLEW library
    glewExperimental=TRUE;
    GLenum err=glewInit();

    if(err!=GLEW_OK)
    {
        printf("glewInit failed, aborting.");
        printf("\tError: %s\n\n",glewGetErrorString(err));
        system("pause");
    }

    if (GLEW_EXT_framebuffer_object != GL_TRUE)
    {
        printf("\n\n\t ** Error! GLEW_EXT_framebuffer_object != GL_TRUE \n\n");
        system("pause");
    }

    ///Executing the rendering process
    init();

    return 0;
}
```