#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
Shader lightShaderInit(Shader tempShader);

// settings
const unsigned int SCR_WIDTH = 1560;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//freezing
bool freeze = false;

auto sun_model = glm::mat4(1.0f);
auto earth_model = glm::mat4(1.0f);
auto moon_model = glm::mat4(1.0f);

//scale
auto sun_scale = glm::vec3(0.003f, 0.003f, 0.003f);
auto earth_scale = glm::vec3(0.0005f, 0.0005f,0.0005f);
auto moon_scale = glm::vec3(0.02f, 0.02f, 0.02f);

//rotation
auto earth_rotation_axes = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3  lightPos(0.0f, 0.0f, 0.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader planetShader("planet_shader.vs", "planet_shader.fs");
    Shader lightSunShader("lamp.vs", "lamp.fs");

    // load models
    // -----------
    Model sunModel(FileSystem::getPath("resources/objects/sun/13913_Sun_v2_l3.obj"));
    Model earthModel(FileSystem::getPath("resources/objects/earth/earth.obj"));
    Model moonModel(FileSystem::getPath("resources/objects/planet/planet.obj"));

    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float earth_radius = 8.0f;
    float moon_radius = 0.8f;
    float earthX, earthZ;
    float moonX, moonZ;
    float startTime = glfwGetTime();
    float currentFrame;
    glm::mat4 projection, view;
    glm::vec3 tempVec;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        //glfwSetKeyCallback(window, key_callback);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //enable and init shader of planets before setting uniforms
        //planets shaders needs to reflect (and affected by) light
        planetShader = lightShaderInit(planetShader);
        
        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        // render the sun model
        lightSunShader.use();
        lightSunShader.setMat4("projection", projection);
        lightSunShader.setMat4("view", view);
        sun_model = glm::mat4(1.0f);
        sun_model = glm::translate(sun_model, lightPos/*glm::vec3(0.0f, 0.0f, 0.0f)*/); // translate it down so it's at the center of the scene
        sun_model = glm::scale(sun_model, sun_scale);	// it's a bit too big for our scene, so scale it down
        lightSunShader.setMat4("model", sun_model);
        sunModel.Draw(lightSunShader);

        planetShader.use();


        /*
         * earth rotation = 1,670 km/h
         * earth orbit the sun = 107,000 km/h
         */

        //freeze motion
        if (freeze){
            // render the earth model
            planetShader.setMat4("model", earth_model);
            earthModel.Draw(planetShader);

            // render the moon model
            planetShader.setMat4("model", moon_model);
            moonModel.Draw(planetShader);

        }
        //unfreeze motion
        else {

            startTime += 0.005;

            // check overflow
            // if (startTime >= (2.0 * 3.14159))
            //      startTime = startTime - (2.0 * 3.14159);

            // render the earth model
            earth_model = glm::mat4(1.0f);
            earthX = sin(startTime) * earth_radius;
            earthZ = cos(startTime) * earth_radius;
            earth_model = glm::translate(earth_model, glm::vec3(earthX, 0.0f, earthZ));
            earth_model = glm::rotate(earth_model, (float) startTime * 30, earth_rotation_axes);
            earth_model = glm::scale(earth_model, earth_scale);    // it's a bit too big for our scene, so scale it down
            planetShader.setMat4("model", earth_model);

            earthModel.Draw(planetShader);

            // render the moon model
            moon_model = glm::mat4(1.0f);
            moonX = sin(startTime * 10)  * moon_radius;
            moonZ = cos(startTime * 10) * moon_radius;
            moon_model = glm::translate(moon_model, glm::vec3(moonX, 0.0f, moonZ));
            moon_model = glm::translate(moon_model, glm::vec3(earthX, 0.0f, earthZ)); // translate it down so it's at the center of the scene
            planetShader.setMat4("view", view);
            moon_model = glm::scale(moon_model,moon_scale);    // it's a bit too big for our scene, so scale it down
            planetShader.setMat4("model", moon_model);
            moonModel.Draw(planetShader);

        }

        glfwSetKeyCallback(window, key_callback);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
}

//Could put it at process input but it got the same key many times
//input key lag
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        freeze = !freeze;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


Shader lightShaderInit(Shader tempShader){

    //enable shader before setting uniforms
    tempShader.use();
    tempShader.setVec3("light.position", lightPos);
    tempShader.setVec3("viewPos", camera.Position);

    // light properties
    //values that fit better to our model
    tempShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
    tempShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    tempShader.setVec3("light.specular", 0.25f, 0.25f, 0.25f);

    //those values are from openGl tutorial for point lightning
    tempShader.setFloat("light.constant", 1.0f);
    tempShader.setFloat("light.linear", 0.0014f);
    tempShader.setFloat("light.quadratic", 0.000007f);

    // material properties
    tempShader.setFloat("material.shininess", 25.0f);

    return tempShader;
}