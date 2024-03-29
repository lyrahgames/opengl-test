// STL
#include <iostream>
#include <stdexcept>
#include <string>
//
// glbinding handles the OpenGL
// inclusion and extension loading.
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
//
// Use GLFW as platform-independent library to create a window.
// Because of glbinding, we do not want GLFW to include OpenGL.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//
// Include GLM for typical math routines in 2, 3, and 4 dimensions.
#include <glm/glm.hpp>
//
#include <glm/ext.hpp>

// STL is standard. So we use its namespace everywhere.
using namespace std;
// glbinding puts OpenGL functions in namespace "gl".
// To write compatible code, we open it.
using namespace gl;

// This application shall be thought of as singleton.
// We use a namespace as an implementation alternative
// to the standard object-oriented design of a class.
// This allows the use of a global application state
// which is much easier to use with GLFW and lambdas.
namespace application {

// Default Window Parameters
int screen_width = 500;
int screen_height = 500;
const char* window_title = "OpenGL Test";

// Structure and vertex data for the triangle to be rendered.
static const struct {
  float x, y;     // 2D Position
  float r, g, b;  // Color
} vertices[3] = {{-0.6f, -0.4f, 1.f, 0.f, 0.f},
                 {0.6f, -0.4f, 0.f, 1.f, 0.f},
                 {0.f, 0.6f, 0.f, 0.f, 1.f}};

// Vertex and fragment shader source code.
// We do not need newline characters at every line ending
// due to the semicolons in the syntax of GLSL.
// Because the shader is statically coded,
// we use tab characters for readability.
const char* vertex_shader_text =
    "#version 330 core\n"
    "uniform mat4 MVP;"
    "attribute vec3 vCol;"
    "attribute vec2 vPos;"
    "varying vec3 color;"
    "void main(){"
    "  gl_Position = MVP * vec4(vPos, 0.0, 1.0);"
    "  color = vCol;"
    "}";
const char* fragment_shader_text =
    "#version 330 core\n"
    "varying vec3 color;"
    "void main(){"
    "  gl_FragColor = vec4(color, 1.0);"
    "}";

// Initialize the application.
// Can be called manually.
// Otherwise called by application::run.
void init();

// Run the application.
// If not initialized, automatically calls application::init.
void run();

// Destroy the application.
// Automatically called when exiting the program.
void free();

}  // namespace application

int main() {
  // Initialize the application.
  // Is automatically called by application::run()
  // but can be called manually.
  // application::init();

  // Run the application loop and show the triangle.
  application::run();

  // Destroy the application.
  // Is automatically called at the end of program execution
  // but can be called manually.
  // application::free();
}

// Implementation of the application functions.
namespace application {

// To model private members of the singleton application,
// we use an embedded namespace "detail".
// With this, we even allow calls to private members
// but make clear that this should only be done
// when the user knows what that means.
namespace detail {

// Window and OpenGL Context
GLFWwindow* window = nullptr;
bool is_initialized = false;
// Vertex Data Handles
GLuint vertex_array;
GLuint vertex_buffer;
// Shader Handles
GLuint program;
GLint mvp_location, vpos_location, vcol_location;
// Transformation Matrices
glm::mat4 model, view, projection;

// RAII Destructor Simulator
// To make sure that the application::free function
// can be viewed as a destructor and adheres to RAII principle,
// we use a global variable of a simple type without a state
// and a destructor calling the application::free function.
struct raii_destructor_t {
  ~raii_destructor_t() { free(); }
} raii_destructor{};

// Helper Function Declarations
// Create window with OpenGL context.
void init_window();
// Compile and link the shader program.
void init_shader();
// Set up vertex buffer, vertex array, and vertex attributes.
void init_vertex_data();
// Function called when window is resized.
void resize();
// Function called to update variables in every application loop.
void update();
// Function called to render to screen in every application loop.
void render();

}  // namespace detail

void init() {
  // "Allow" function to access private members of application.
  using namespace detail;
  // Do not initialize if it has already been done.
  if (is_initialized) return;

  init_window();
  // The shader has to be initialized before
  // the initialization of the vertex data
  // due to identifier location variables
  // that have to be set after creating the shader program.
  init_shader();
  init_vertex_data();

  // To initialize the viewport and matrices,
  // window has to be resized at least once.
  resize();

  // Update private state.
  is_initialized = true;
  cout << "Created OpenGL test application without errors!" << endl;
}

void free() {
  // "Allow" function to access private members of application.
  using namespace detail;
  // An uninitialized application cannot be destroyed.
  if (!is_initialized) return;

  // Delete vertex data.
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteVertexArrays(1, &vertex_array);
  // Delete shader program.
  glDeleteProgram(program);

  if (window) glfwDestroyWindow(window);
  glfwTerminate();

  // Update private state.
  is_initialized = false;
  cout << "Destroyed OpenGL test application without errors!" << endl;
}

void run() {
  // "Allow" function to access private members of application.
  using namespace detail;
  // Make sure application::init has been called.
  if (!is_initialized) init();

  // Start application loop.
  while (!glfwWindowShouldClose(window)) {
    // Handle user and OS events.
    glfwPollEvents();

    update();
    render();

    // Swap buffers to display the
    // new content of the frame buffer.
    glfwSwapBuffers(window);
  }
}

// Private Member Function Implementations
namespace detail {

void init_window() {
  // Create GLFW handler for error messages.
  glfwSetErrorCallback([](int error, const char* description) {
    throw runtime_error("GLFW Error " + to_string(error) + ": " + description);
  });

  // Initialize GLFW.
  glfwInit();

  // Set required OpenGL context version for the window.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Force GLFW to use the core profile of OpenGL.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the window to render in.
  window = glfwCreateWindow(screen_width, screen_height, window_title,  //
                            nullptr, nullptr);

  // Initialize the OpenGL context for the current window by using glbinding.
  glfwMakeContextCurrent(window);
  glbinding::initialize(glfwGetProcAddress);

  // Make window to be closed when pressing Escape
  // by adding key event handler.
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  });

  // Add resize handler.
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* window, int width, int height) { resize(); });
}

void init_shader() {
  // Compile and create the vertex shader.
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);
  {
    // Check for errors.
    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char info_log[512];
      glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
      throw runtime_error(
          string("OpenGL Error: Failed to compile vertex shader!: ") +
          info_log);
    }
  }

  // Compile and create the fragment shader.
  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);
  {
    // Check for errors.
    GLint success;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char info_log[512];
      glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
      throw runtime_error(
          string("OpenGL Error: Failed to compile fragment shader!: ") +
          info_log);
    }
  }

  // Link vertex shader and fragment shader to shader program.
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  {
    // Check for errors.
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
      char info_log[512];
      glGetProgramInfoLog(program, 512, nullptr, info_log);
      throw runtime_error(
          string("OpenGL Error: Failed to link shader program!: ") + info_log);
    }
  }

  // Delete unused shaders.
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // Get identifier locations in the shader program
  // to change their values from the outside.
  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");
}

void init_vertex_data() {
  // Use a vertex array to be able to reference the vertex buffer and
  // the vertex attribute arrays of the triangle with one single variable.
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);

  // Generate and bind the buffer which shall contain the triangle data.
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  // The data is not changing rapidly. Therefore we use GL_STATIC_DRAW.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Set the data layout of the position and colors
  // with vertex attribute pointers.
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void*)0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void*)(sizeof(float) * 2));
}

void resize() {
  // Update size parameters and compute aspect ratio.
  glfwGetFramebufferSize(window, &screen_width, &screen_height);
  const auto aspect_ratio = float(screen_width) / screen_height;
  // Make sure rendering takes place in the full screen.
  glViewport(0, 0, screen_width, screen_height);
  // Use a perspective projection with correct aspect ratio.
  projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 100.f);
  // Position the camera in space by using a view matrix.
  view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2));
}

void update() {
  // Compute and set MVP matrix in shader.
  model = glm::mat4{1.0f};
  const auto axis = glm::normalize(glm::vec3(1, 1, 1));
  model = rotate(model, float(glfwGetTime()), axis);
  const auto mvp = projection * view * model;
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
}

void render() {
  // Clear the screen.
  glClear(GL_COLOR_BUFFER_BIT);

  // Bind vertex array of triangle
  // and use the created shader
  // to render the triangle.
  glUseProgram(program);
  glBindVertexArray(vertex_array);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

}  // namespace detail

}  // namespace application
