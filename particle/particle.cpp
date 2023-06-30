/*
 *  Add by Kevin.Wen
 *  This test sample demonstrates particle system support using rabbitrender.
 *  It is based on the code of nehe.gamedev.net which was developed by Jeff
 * Molofee
 */

//#include <windows.h>		// Header File For Windows

#include <stdint.h>

#ifdef _MSC_VER
#include <windows.h> // Header File For Windows
#else
#include <sys/types.h>
#endif

#include <EGL/egl.h>
#include <stdio.h> // Header File For Standard Input/Output

#include <GLES/gl.h>
#include <GLES/glext.h>

#include "../windisplay/EGLUtils.h"

#include <math.h>
#include <stdlib.h>

//#include <ratio>
//#include <chrono>

using namespace android;

EGLDisplay eglDisplay;
EGLSurface eglSurface;
EGLContext eglContext;
GLuint texture[1];

static ESContext *mESContext = NULL;

#define FIXED_ONE 0x10000
#define ITERATIONS 50

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define DISPLAY_FORMAT 2

//#define  SHOW_DEBUG_INFO 0

//#define  SUPPORT_WIREFRAME

int init_gl_surface(void);
void free_gl_surface(void);
void init_scene(void);
void render(void *esContext);
void create_texture(void);
int readTimer(void);

bool keys[256];         // Array Used For The Keyboard Routine
bool active = true;     // Window Active Flag Set To TRUE By Default
bool fullscreen = true; // Fullscreen Flag Set To Fullscreen Mode By Default

bool twinkle; // Twinkling Stars
bool tp;      // 'T' Key Pressed?

const int num = 50; // Number Of Stars To Draw

typedef struct // Create A Structure For Star
{
    int r, g, b;  // Stars Color
    GLfloat dist, // Stars Distance From Center
        angle;    // Stars Current Angle
} stars;
stars star[num]; // Need To Keep Track Of 'num' Stars

GLfloat zoom = -15.0f; // Distance Away From Stars
GLfloat tilt = 90.0f;  // Tilt The View
GLfloat spin = 0;      // Spin Stars

GLuint loop = 0; // General Loop Variable
// GLint	texture[1];			// Storage For One textures

typedef struct _AUX_RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char *data;
} AUX_RGBImageRec;

bool loadtexture() // Load Bitmaps And Convert To Textures
{
    bool status = false; // Status Indicator

    AUX_RGBImageRec *TextureImage[1]; // Create Storage Space For The Texture

    memset(TextureImage, 0, sizeof(void *) * 1); // Set The Pointer To NULL

    TextureImage[0] = new AUX_RGBImageRec;
    TextureImage[0]->sizeX = 128;
    TextureImage[0]->sizeY = 128;
    TextureImage[0]->data =
        new unsigned char[TextureImage[0]->sizeX * TextureImage[0]->sizeY * 3];

    FILE *file = fopen("texture.bin", "rb");

    if (!file)
        return false;

    int ret =
        fread(TextureImage[0]->data,
              TextureImage[0]->sizeX * TextureImage[0]->sizeY * 3, 1, file);
    fclose(file);
    {
        status = true; // Set The Status To TRUE

        glGenTextures(1, &texture[0]); // Create One Texture

        // Create Linear Filtered Texture
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureImage[0]->sizeX,
                     TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     TextureImage[0]->data);
    }

    if (TextureImage[0]) // If Texture Exists
    {
        if (TextureImage[0]->data) // If Texture Image Exists
        {
            free(TextureImage[0]->data); // Free The Texture Image Memory
        }

        free(TextureImage[0]); // Free The Image Structure
    }
    return status; // Return The Status
}

GLvoid ReSizeGLScene(GLsizei width,
                     GLsizei height) // Resize And Initialize The GL Window
{
    if (height == 0) // Prevent A Divide By Zero By
    {
        height = 1; // Making Height Equal One
    }

    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glLoadIdentity();            // Reset The Projection Matrix

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity();           // Reset The Modelview Matrix
}

void perspectiveGL(double fovY, double aspect, double zNear, double zFar) {
    const GLfloat pi = 3.1415926535897932384626433832795;

    double xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovY * pi / 360);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

void init_scene(void) // All Setup For OpenGL Goes Here
{
    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    perspectiveGL(45.0f, (double)DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!loadtexture()) // Jump To Texture Loading Routine
    {
        return; // If Texture Didn't Load Return FALSE
    }

    glEnable(GL_TEXTURE_2D);              // Enable Texture Mapping
    glShadeModel(GL_SMOOTH);              // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black Background
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,
           GL_NICEST); // Really Nice Perspective Calculations
    glBlendFunc(GL_SRC_ALPHA,
                GL_ONE); // Set The Blending Function For Translucency
    glEnable(GL_BLEND);

    for (loop = 0; loop < num; loop++) {
        star[loop].angle = 0.0f;
        star[loop].dist = (float(loop) / num) * 5.0f;
        star[loop].r = rand() % 256;
        star[loop].g = rand() % 256;
        star[loop].b = rand() % 256;
    }
}

#ifdef SHOW_DEBUG_INFO
double showFps() {
    static unsigned long long totalTime = 0;
    static unsigned long frameNumber = 0;
    static unsigned long previousTime = 0;

    // unsigned long end = timeGetTime();
    unsigned long end = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();

    if (previousTime != 0) {
        totalTime += end - previousTime;
    }

    previousTime = end;

    frameNumber++;

    if ((frameNumber % 10) == 0)
        return (double)frameNumber / (totalTime / 1000);
    else
        return 0;
}
#endif

void render(void *esContext) {
    const GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f,
                                1.0f,  1.0f,  0.0f, -1.0f, 1.0f,  0.0f};

    const GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f,
                                 1.0f, 1.0f, 0.0f, 1.0f};

    const GLushort indices[] = {0, 1, 2, 0, 2, 3};

    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer
    glBindTexture(GL_TEXTURE_2D, texture[0]); // Select Our Texture

    GLfloat result[16] = {0};
    for (loop = 0; loop < num; loop++) // Loop Through All The Stars
    {
        glLoadIdentity(); // Reset The View Before We Draw Each Star
        glTranslatef(0.0f, 0.0f,
                     zoom); // Zoom Into The Screen (Using The Value In 'zoom')
        glRotatef(tilt, 1.0f, 0.0f,
                  0.0f); // Tilt The View (Using The Value In 'tilt')
        glRotatef(star[loop].angle, 0.0f, 1.0f,
                  0.0f); // Rotate To The Current Stars Angle
        glTranslatef(star[loop].dist, 0.0f,
                     0.0f); // Move Forward On The X Plane
        glRotatef(-star[loop].angle, 0.0f, 1.0f,
                  0.0f);                    // Cancel The Current Stars Angle
        glRotatef(-tilt, 1.0f, 0.0f, 0.0f); // Cancel The Screen Tilt

        glRotatef(spin, 0.0f, 0.0f, 1.0f);

        glGetFloatv(GL_MODELVIEW_MATRIX, result);
        glGetFloatv(GL_PROJECTION_MATRIX, result);

#if 1
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

        GLfloat color[] = {star[loop].r / 255.0f, star[loop].g / 255.0f,
                           star[loop].b / 255.0f, 255 / 255.0f};
        glColorPointer(4, GL_FLOAT, 0, color);

        int nelem = sizeof(indices) / sizeof(indices[0]);

#ifdef SUPPORT_WIREFRAME
        for (int i = 0; i < nelem; i += 3)
            glDrawArrays(GL_LINE_LOOP, i, 3);
#else
        glDrawElements(GL_TRIANGLES, nelem, GL_UNSIGNED_SHORT, indices);

        if (0) { // dump framebuffer through glReadPixels
            unsigned char *read_buffer =
                new unsigned char[DISPLAY_WIDTH * DISPLAY_HEIGHT * 4];
            glReadPixels(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RGBA,
                         GL_UNSIGNED_BYTE, read_buffer);
            FILE *file = fopen("swapbuffer.bin", "ab");
            if (file) {
                fwrite(read_buffer, 1, DISPLAY_WIDTH * DISPLAY_HEIGHT * 4,
                       file);
                fclose(file);
            }
            delete read_buffer;
        }
#endif

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

        spin += 0.01f;
        star[loop].angle += float(loop) / num;
        star[loop].dist -= 0.01f;
        if (star[loop].dist < 0.0f) {
            star[loop].dist += 5.0f;
            star[loop].r = rand() % 256;
            star[loop].g = rand() % 256;
            star[loop].b = rand() % 256;
        }
    }

    eglSwapBuffers(eglDisplay, eglSurface);

#ifdef SHOW_DEBUG_INFO
    double fps = showFps();
    if (fps != 0)
        printf("fps :%5.2f \n", fps);
#endif
}

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *)glGetString(s);
    fprintf(stderr, "GL %s = %s\n", name, v);
}

void printEGLConfiguration(EGLDisplay dpy, EGLConfig config) {

#define X(VAL)                                                                 \
    { VAL, #VAL }
    struct {
        EGLint attribute;
        const char *name;
    } names[] = {
        X(EGL_BUFFER_SIZE),
        X(EGL_ALPHA_SIZE),
        X(EGL_BLUE_SIZE),
        X(EGL_GREEN_SIZE),
        X(EGL_RED_SIZE),
        X(EGL_DEPTH_SIZE),
        X(EGL_STENCIL_SIZE),
        X(EGL_CONFIG_CAVEAT),
        X(EGL_CONFIG_ID),
        X(EGL_LEVEL),
        X(EGL_MAX_PBUFFER_HEIGHT),
        X(EGL_MAX_PBUFFER_PIXELS),
        X(EGL_MAX_PBUFFER_WIDTH),
        X(EGL_NATIVE_RENDERABLE),
        X(EGL_NATIVE_VISUAL_ID),
        X(EGL_NATIVE_VISUAL_TYPE),
        X(EGL_SAMPLES),
        X(EGL_SAMPLE_BUFFERS),
        X(EGL_SURFACE_TYPE),
        X(EGL_TRANSPARENT_TYPE),
        X(EGL_TRANSPARENT_RED_VALUE),
        X(EGL_TRANSPARENT_GREEN_VALUE),
        X(EGL_TRANSPARENT_BLUE_VALUE),
        X(EGL_BIND_TO_TEXTURE_RGB),
        X(EGL_BIND_TO_TEXTURE_RGBA),
        X(EGL_MIN_SWAP_INTERVAL),
        X(EGL_MAX_SWAP_INTERVAL),
        X(EGL_LUMINANCE_SIZE),
        X(EGL_ALPHA_MASK_SIZE),
        X(EGL_COLOR_BUFFER_TYPE),
        X(EGL_RENDERABLE_TYPE),
        X(EGL_CONFORMANT),
    };
#undef X

    for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); j++) {
        EGLint value = -1;
        EGLint returnVal =
            eglGetConfigAttrib(dpy, config, names[j].attribute, &value);
        EGLint error = eglGetError();
        if (returnVal && error == EGL_SUCCESS) {
            printf(" %s: ", names[j].name);
            printf("%d (0x%x)", value, value);
        }
    }
    printf("\n");
}

static void checkEglError(const char *op, EGLBoolean returnVal = EGL_TRUE) {
    if (returnVal != EGL_TRUE) {
        fprintf(stderr, "%s() returned %d\n", op, returnVal);
    }

    for (EGLint error = eglGetError(); error != EGL_SUCCESS;
         error = eglGetError()) {
        fprintf(stderr, "after %s() eglError %s (0x%x)\n", op,
                EGLUtils::strerror(error), error);
    }
}

int printEGLConfigurations(EGLDisplay dpy) {
    EGLint numConfig = 0;
    EGLint returnVal = eglGetConfigs(dpy, NULL, 0, &numConfig);
    checkEglError("eglGetConfigs", returnVal);
    if (!returnVal) {
        return false;
    }

    printf("Number of EGL configurations: %d\n", numConfig);

    EGLConfig *configs = (EGLConfig *)malloc(sizeof(EGLConfig) * numConfig);
    if (!configs) {
        printf("Could not allocate configs.\n");
        return false;
    }

    returnVal = eglGetConfigs(dpy, configs, numConfig, &numConfig);
    checkEglError("eglGetConfigs", returnVal);
    if (!returnVal) {
        free(configs);
        return false;
    }

    for (int i = 0; i < numConfig; i++) {
        printf("Configuration %d\n", i);
        printEGLConfiguration(dpy, configs[i]);
    }

    free(configs);
    return true;
}

int main(int argc, char **argv) {
    int q;
    int start, end;
    printf(
        "Initializing GLES 1.1 & EGL pure software stack for test sample!\n\n");
    printf("The GLES 1.1 & EGL pure software stack is encapsulated in "
           "swrender.dll. \n\n");
    printf("Please reach me at listream@126.com if you have any question.\n\n");

    if (!init_gl_surface()) {
        printf("GL initialisation failed - exiting\n");
        return 0;
    }
    init_scene();

    printf("Running...\n");

    WinLoop(mESContext);
    free_gl_surface();
    return 0;
}

int init_gl_surface(void) {
    EGLint numConfigs = 1;
    EGLConfig myConfig = {0};
    EGLint attrib[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE};

    if ((eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        printf("eglGetDisplay failed\n");
        return 0;
    }

    if (eglInitialize(eglDisplay, NULL, NULL) != EGL_TRUE) {
        printf("eglInitialize failed\n");
        return 0;
    }
#ifdef SHOW_DEBUG_INFO
    if (!printEGLConfigurations(eglDisplay)) {
        printf("printEGLConfigurations failed.\n");
        return 0;
    }
#endif
    mESContext = new ESContext;
    mESContext->context = NULL;

    EGLNativeWindowType window = createDisplaySurface(
        mESContext, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FORMAT);

    EGLUtils::selectConfigForNativeWindow(eglDisplay, attrib, window,
                                          &myConfig);

    mESContext->drawFunc = render;

    if ((eglSurface = eglCreateWindowSurface(eglDisplay, myConfig, window,
                                             0)) == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface failed\n");
        return 0;
    }

    if ((eglContext = eglCreateContext(eglDisplay, myConfig, 0, 0)) ==
        EGL_NO_CONTEXT) {
        printf("eglCreateContext failed\n");
        return 0;
    }

    if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) !=
        EGL_TRUE) {
        printf("eglMakeCurrent failed\n");
        return 0;
    }

    int w, h;

    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &w);
    checkEglError("eglQuerySurface");
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &h);
    checkEglError("eglQuerySurface");
    GLint dim = w < h ? w : h;
#ifdef SHOW_DEBUG_INFO
    fprintf(stderr, "Window dimensions: %d x %d\n", w, h);

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
#endif
    return 1;
}

void free_gl_surface(void) {
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        eglDestroyContext(eglDisplay, eglContext);
        eglDestroySurface(eglDisplay, eglSurface);
        eglTerminate(eglDisplay);
        eglDisplay = EGL_NO_DISPLAY;
    }
}