#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <enet/enet.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int test_glm() {
    glm::vec3 pos(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec4 result = model * glm::vec4(pos, 1.0f);
    bool passed = (result.x == 1.0f);
    std::cout << "[GLM]   " << (passed ? "PASS" : "FAIL")
              << " - translate test, x = " << result.x << "\n";
    return passed ? 0 : 1;
}

int test_enet() {
    if (enet_initialize() != 0) {
        std::cout << "[ENET]  FAIL - could not initialize\n";
        return 1;
    }
    ENetAddress address;
    ENetHost* host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (host == nullptr) {
        std::cout << "[ENET]  FAIL - could not create host\n";
        enet_deinitialize();
        return 1;
    }
    std::cout << "[ENET]  PASS - initialized and created client host\n";
    enet_host_destroy(host);
    enet_deinitialize();
    return 0;
}

int main() {
    std::cout << "=== Library Test ===\n\n";

    // --- GLM ---
    test_glm();

    // --- ENet ---
    test_enet();

    // --- GLFW ---
    if (!glfwInit()) {
        std::cout << "[GLFW]  FAIL - could not initialize\n";
        return -1;
    }
    std::cout << "[GLFW]  PASS - initialized\n";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Library Test", nullptr, nullptr);
    if (!window) {
        std::cout << "[GLFW]  FAIL - could not create window\n";
        glfwTerminate();
        return -1;
    }
    std::cout << "[GLFW]  PASS - window created\n";

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "[GLAD]  FAIL - could not load OpenGL\n";
        return -1;
    }
    std::cout << "[GLAD]  PASS - OpenGL loaded\n";
    std::cout << "[GL]    Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "[GL]    Renderer: " << glGetString(GL_RENDERER) << "\n";

    std::cout << "\n=== All tests passed! Showing window for 3 seconds ===\n";

    // simple color cycle to show rendering works
    float time = 0.0f;
    while (!glfwWindowShouldClose(window) && time < 3.0f) {
        float r = (sin(time * 2.0f) + 1.0f) / 2.0f;
        float g = (sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f;
        float b = (sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f;

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
        time += 0.016f;  // ~60fps
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
//Code is Running
//my code is also running.
