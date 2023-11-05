#define _USE_MATH_DEFINES

#include "mesh.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "object3d.hpp"
#include "texture.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gl_includes.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#define _MY_OPENGL_IS_33_

// Window parameters
GLFWwindow *g_window {};

// GPU objects
GLuint g_program {};  // A GPU program contains at least a vertex shader and a fragment shader

Camera g_camera {};

std::shared_ptr<Object3D> g_mesh {};

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow *window, int width, int height) {
    g_camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    glViewport(0, 0, (GLint)width, (GLint)height);  // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (key == GLFW_KEY_F) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
            glfwSetWindowShouldClose(window, true);  // Closes the application if the escape key is pressed
        }

    }
}

float g_cameraDistance = 5.0f;
float g_cameraAngleX = 0.0f;

// Scroll for zooming
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    g_cameraDistance -= yoffset * 0.1f;
    g_cameraDistance = std::max(g_cameraDistance, 0.1f);

    g_cameraAngleX -= xoffset * 0.04f;
}

void errorCallback(int error, const char *desc) {
    std::cout << "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
    glfwSetErrorCallback(errorCallback);

    // Initialize GLFW, the library responsible for window management
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to init GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Before creating the window, set some option flags
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create the window
    g_window = glfwCreateWindow(
        1024, 768,
        "Interactive 3D Applications (OpenGL) - Simple Solar System", nullptr, nullptr);
    if (!g_window) {
        std::cerr << "ERROR: Failed to open window" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
    glfwMakeContextCurrent(g_window);
    glfwSetWindowSizeCallback(g_window, windowSizeCallback);
    glfwSetKeyCallback(g_window, keyCallback);
    glfwSetScrollCallback(g_window, scrollCallback);
}

void initImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void initOpenGL() {
    // Load extensions for modern OpenGL
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glCullFace(GL_BACK);                   // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable(GL_CULL_FACE);                // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc(GL_LESS);                  // Specify the depth test for the z-buffer
    glEnable(GL_DEPTH_TEST);               // Enable the z-buffer test in the rasterization
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // specify the background color, used any time the framebuffer is cleared
}

void initGPUprogram() {
    g_program = glCreateProgram();  // Create a GPU program, i.e., two central shaders of the graphics pipeline
    loadShader(g_program, GL_VERTEX_SHADER, "resources/vertexShader.glsl");
    loadShader(g_program, GL_FRAGMENT_SHADER, "resources/fragmentShader.glsl");
    glLinkProgram(g_program);  // The main GPU program is ready to be handle streams of polygons
}


void initCPUgeometry() {
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    //g_mesh = std::make_shared<Object3D>(Mesh::genPlane(), texture);
    g_mesh = std::make_shared<Object3D>(Mesh::genSphere(100), texture);
}

void initCamera() {
    int width, height;
    glfwGetWindowSize(g_window, &width, &height);
    g_camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));

    g_camera.setPosition(glm::vec3(0.0, 0.0, 3.0));
    g_camera.setNear(0.1);
    g_camera.setFar(80);

    g_camera.setFoV(90);
}

void init() {
    initGLFW();
    initOpenGL();

    initCPUgeometry();
    initGPUprogram();
    initCamera();
    
    initImGui();
}

void clear() {
    glDeleteProgram(g_program);

    glfwDestroyWindow(g_window);
    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

struct TexturingParams {
    glm::vec3 groundColor;
    glm::vec3 sandColor;
    glm::vec3 waterColor;

    glm::vec3 treeColor;
    glm::vec3 trunkColor;
    glm::vec3 grassColor;

    int treeDensity;
    int grassDensity;
};

struct GenerationParams {
	float heightFactor;
	float terrainHeightFactor;

	int nOctaves;
	float terrainScale;
	float terrainHeightOffset;
};

struct GlobalParams {
    int nbShells;
    TexturingParams texturingParams;
    GenerationParams generationParams;
};

GlobalParams g_globalParams = {
    128,
    {
        glm::vec3(0.6f, 0.75f, 0.15f),
        glm::vec3(0.9f, 0.8f, 0.2f),
        glm::vec3(0.0f, 0.0f, 1.0f),

        glm::vec3(0.2f, 0.7f, 0.1f),
        glm::vec3(0.5f, 0.3f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        100,
        2000
    },
    {
        0.3f,
        0.2f,
        8,
        1.0f,
        0.1f
    }
};

void renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Start drawing here 

    ImGui::Begin("Parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SliderInt("Number of shells", &g_globalParams.nbShells, 8, 256);

    ImGui::LabelText("Texturing parameters", "");

    ImGui::ColorEdit3("Ground color", &g_globalParams.texturingParams.groundColor[0]);
    ImGui::ColorEdit3("Sand color", &g_globalParams.texturingParams.sandColor[0]);
    ImGui::ColorEdit3("Water color", &g_globalParams.texturingParams.waterColor[0]);
    
    ImGui::NewLine();

    ImGui::ColorEdit3("Tree color", &g_globalParams.texturingParams.treeColor[0]);
    ImGui::ColorEdit3("Trunk color", &g_globalParams.texturingParams.trunkColor[0]);
    ImGui::ColorEdit3("Grass color", &g_globalParams.texturingParams.grassColor[0]);
    
    ImGui::NewLine();

    ImGui::SliderInt("Tree density", &g_globalParams.texturingParams.treeDensity, 1, 500);
    ImGui::SliderInt("Grass density", &g_globalParams.texturingParams.grassDensity, 1, 5000);

    ImGui::End();

    ImGui::Begin("Generation parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SliderFloat("Height factor", &g_globalParams.generationParams.heightFactor, 0.0f, 1.0f);
    ImGui::SliderFloat("Terrain height factor", &g_globalParams.generationParams.terrainHeightFactor, 0.0f, 1.0f);

    ImGui::SliderInt("Number of octaves", &g_globalParams.generationParams.nOctaves, 1, 16);
    ImGui::SliderFloat("Terrain scale", &g_globalParams.generationParams.terrainScale, 0.0f, 10.0f);
    ImGui::SliderFloat("Terrain height offset", &g_globalParams.generationParams.terrainHeightOffset, 0.0f, 10.0f);

    ImGui::End();

    // End drawing here

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void setParamsUniforms() {

    // Texturing params

    setUniform(g_program, "u_texturingParams.groundColor", g_globalParams.texturingParams.groundColor);
    setUniform(g_program, "u_texturingParams.sandColor", g_globalParams.texturingParams.sandColor);
    setUniform(g_program, "u_texturingParams.waterColor", g_globalParams.texturingParams.waterColor);

    setUniform(g_program, "u_texturingParams.treeColor", g_globalParams.texturingParams.treeColor);
    setUniform(g_program, "u_texturingParams.trunkColor", g_globalParams.texturingParams.trunkColor);
    setUniform(g_program, "u_texturingParams.grassColor", g_globalParams.texturingParams.grassColor);

    setUniform(g_program, "u_texturingParams.treeDensity", g_globalParams.texturingParams.treeDensity);
    setUniform(g_program, "u_texturingParams.grassDensity", g_globalParams.texturingParams.grassDensity);

    // Generation params

    setUniform(g_program, "u_generationParams.heightFactor", g_globalParams.generationParams.heightFactor);
    setUniform(g_program, "u_generationParams.terrainHeightFactor", g_globalParams.generationParams.terrainHeightFactor);

    setUniform(g_program, "u_generationParams.nOctaves", g_globalParams.generationParams.nOctaves);
    setUniform(g_program, "u_generationParams.terrainScale", g_globalParams.generationParams.terrainScale);
    setUniform(g_program, "u_generationParams.terrainHeightOffset", g_globalParams.generationParams.terrainHeightOffset);
}

// The main rendering call
void render() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Erase the color and z buffers.

    const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
    const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();

    glUseProgram(g_program);
    
    setUniform(g_program, "u_viewMat", viewMatrix);
    setUniform(g_program, "u_projMat", projMatrix);

    setUniform(g_program, "u_cameraPosition", g_camera.getPosition());

    setUniform(g_program, "u_texture", 0);

    setUniform(g_program, "u_time", static_cast<float>(glfwGetTime()));

    setParamsUniforms();

    // Render objects

    for (int i = g_globalParams.nbShells - 1; i >= 0; i--) {
        float height = (float)i / (float)(g_globalParams.nbShells-1);
        setUniform(g_program, "u_height", height);
        g_mesh->render(g_program);
    }

    renderUI();
}

// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {

    glm::vec3 targetPosition = glm::vec3(0.05f, 0.05f, 0.0f);
    g_camera.setTarget(targetPosition);

    glm::vec3 cameraOffset = glm::normalize(glm::vec3(cos(g_cameraAngleX), 0.3f, sin(g_cameraAngleX))) * (1.1f + g_cameraDistance);
    g_camera.setPosition(targetPosition + cameraOffset);
}

int main(int argc, char **argv) {
    init();
    while (!glfwWindowShouldClose(g_window)) {
        update(static_cast<float>(glfwGetTime()));
        render();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    clear();
    return EXIT_SUCCESS;
}
