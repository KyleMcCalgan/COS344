#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include "shader.hpp"
#include "src/Editor.hpp"
#include "src/Renderer.hpp"

static EditorState* g_editor  = nullptr;
static int          g_winW    = 1200;
static int          g_winH    = 700;

static void mouseButtonCB(GLFWwindow* win, int btn, int action, int /*mods*/)
{
    if (!g_editor) return;

    bool inLeft = (g_editor->mouseX < g_winW / 2.0);
    bool inRight = !inLeft;

    if (action == GLFW_PRESS) {
        if (btn == GLFW_MOUSE_BUTTON_LEFT)  g_editor->leftJustPressed  = true;
        if (btn == GLFW_MOUSE_BUTTON_RIGHT) g_editor->rightJustPressed = true;

        // Start orbit drag if right-click in right viewport
        if (btn == GLFW_MOUSE_BUTTON_RIGHT && inRight) {
            g_editor->orbitDragging = true;
            g_editor->orbitLastX   = g_editor->mouseX;
            g_editor->orbitLastY   = g_editor->mouseY;
        }
    }
    if (action == GLFW_RELEASE) {
        if (btn == GLFW_MOUSE_BUTTON_RIGHT)
            g_editor->orbitDragging = false;
    }
    (void)win;
}

static void cursorPosCB(GLFWwindow* /*win*/, double x, double y)
{
    if (!g_editor) return;

    if (g_editor->orbitDragging) {
        double dx = x - g_editor->orbitLastX;
        double dy = y - g_editor->orbitLastY;
        g_editor->camera.theta -= (float)dx * 0.005f;
        float newPhi = g_editor->camera.phi - (float)dy * 0.005f;
        if (newPhi < 0.05f) newPhi = 0.05f;
        if (newPhi > 3.09f) newPhi = 3.09f;
        g_editor->camera.phi = newPhi;
        g_editor->orbitLastX = x;
        g_editor->orbitLastY = y;
    }

    g_editor->mouseX = x;
    g_editor->mouseY = y;
}

static void scrollCB(GLFWwindow* /*win*/, double /*dx*/, double dy)
{
    if (!g_editor) return;
    bool inRight = (g_editor->mouseX >= g_winW / 2.0);
    if (inRight) {
        float r = g_editor->camera.radius - (float)dy * 0.4f;
        if (r < 1.0f)  r = 1.0f;
        if (r > 30.0f) r = 30.0f;
        g_editor->camera.radius = r;
    }
}

static void keyCB(GLFWwindow* win, int key, int /*sc*/, int action, int /*mods*/)
{
    if (!g_editor) return;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_C)      g_editor->cutKeyPressed  = true;
        if (key == GLFW_KEY_G)      g_editor->moveKeyPressed = true;
        if (key == GLFW_KEY_ESCAPE) g_editor->escapePressed  = true;
        if (key == GLFW_KEY_DELETE) g_editor->deletePressed  = true;
        if (key == GLFW_KEY_W)      g_editor->wireframe      = !g_editor->wireframe;
        if (key == GLFW_KEY_ESCAPE && g_editor->mode == EditorMode::NORMAL)
            glfwSetWindowShouldClose(win, GLFW_TRUE);
    }
}

static void fbSizeCB(GLFWwindow* /*win*/, int w, int h)
{
    g_winW = w; g_winH = h;
}

int main()
{
    if (!glfwInit()) { fprintf(stderr, "GLFW init failed\n"); return 1; }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(g_winW, g_winH, "Face Editor", NULL, NULL);
    if (!window) { fprintf(stderr, "Window creation failed\n"); glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { fprintf(stderr, "GLEW init failed\n"); return 1; }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    GLuint shader = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
    if (!shader) { fprintf(stderr, "Shader load failed\n"); return 1; }

    EditorState editor;
    Renderer    renderer;
    renderer.init(shader);
    g_editor = &editor;

    glfwSetMouseButtonCallback(window, mouseButtonCB);
    glfwSetCursorPosCallback  (window, cursorPosCB);
    glfwSetScrollCallback     (window, scrollCB);
    glfwSetKeyCallback        (window, keyCB);
    glfwSetFramebufferSizeCallback(window, fbSizeCB);

    // Print controls
    printf("Controls:\n");
    printf("  Left view: left-click to place vertices, right-click to close shape\n");
    printf("  Click face to select, C to cut, G to move (then left-click to confirm)\n");
    printf("  Delete to remove selected face, ESC to cancel/deselect\n");
    printf("  Right view: right-drag to orbit, scroll to zoom\n");
    printf("  ESC in NORMAL mode to quit\n");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetFramebufferSize(window, &g_winW, &g_winH);
        if (g_winH == 0) g_winH = 1;

        editor.processFrame(g_winW, g_winH);
        renderer.drawFrame(editor, g_winW, g_winH);

        glfwSwapBuffers(window);

        // Clear per-frame edge-triggered flags
        editor.leftJustPressed  = false;
        editor.rightJustPressed = false;
        editor.cutKeyPressed    = false;
        editor.moveKeyPressed   = false;
        editor.escapePressed    = false;
        editor.deletePressed    = false;
    }

    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
