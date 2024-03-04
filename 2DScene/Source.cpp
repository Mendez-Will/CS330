#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

// For textures
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM Math header
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include the shader header
#include <shader.h>
// Include the camera header
#include <camera.h>
#include <iostream>
#include <vector>

/*
* Fur texture sourced from Freepik
* Wood Texture sources from Freepik
* Glass Texture sourced from Freepik by RawPixel
* Black Texture sourced from Freepik by RawPixel
*/

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "3D Scene Recreation"; // Macro for window title

    // Variables for window width and height
    const int SCR_WIDTH = 800;
    const int SCR_HEIGHT = 600;

    // For view toggling
    bool isPerspective = true; // Defines starting view
    glm::mat4 projection; // Initiates projection

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    float PI = glm::radians(180.0f);

    // Stores the data relative to a given mesh
    struct GLMesh
    {
        unsigned int VAOs[14];       // Vertex array objects
        unsigned int VBOs[14];       // Vertex buffer objects
        unsigned int EBOs[14];       // Element buffer objects
        unsigned int indexCounts[14]; // Index counts

    };

    // Stores RGB values for specific colors
    struct color
    {
        float redValue;
        float greenValue;
        float blueValue;
        float alphaValue;
    };

    // Sets number of verices for some objects
    int sides = 20;
    // Sets the rings and segments of sphere objects
    int segments = 20;
    int rings = 20;

    // Use to determine if color should be used
    color noColor;

    // Base color of the cat
    color catColor;

    // Sets the color of the glass
    color glassColor;

    // Textures
    unsigned int texture1;
    unsigned int texture2;
    unsigned int texture3;
    unsigned int texture4;
    unsigned int texture5;
    unsigned int texture6;
    unsigned int texture7;
    unsigned int texture8;

    // Mesh data
    GLMesh mesh;

    // Main window
    GLFWwindow* window = nullptr;
}

// Function to adjust window size
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// Function for mouse callbacks
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// Function for scroll callbacks
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// Function to process input
void processInput(GLFWwindow* window);
// Function for generating cylinder side vertices
std::vector<float> genCylSideVerts(int sides, float height, float radius, color color);
// Funciton to generate cylinder side indices
std::vector<unsigned int> genCylSideIndices(int sides);
// Function for generating cylinder top vertices
std::vector<float> genCylTopVerts(int sides, float height, float radius, color color);
// Funciton to generate cylinder top indices
std::vector<unsigned int> genCylTopIndices(int sides);
// Function for generating cylinder bottom vertices
std::vector<float> genCylBottomVerts(int sides, float height, float radius, color color);
// Funciton to generate cylinder bottom indices
std::vector<unsigned int> genCylBottomIndices(int sides);
// Function to generate sphere vertices
std::vector<float> genSphereVerts(float radius, color color);
// Function to generate sphere indices
std::vector<unsigned int> genSphereIndices();
// Function to generate a pyramids vertices
std::vector<float> genPyramidVerts(int sides, float height, float radius, color color);
// Function to generate a pyramids indices
std::vector<unsigned int> genPyramidIndices(int sides);
// Function to create textures
void createTextures();

// Function for toggling view between orthographic and perspective 
void toggleView();
// Function to initialize program
bool progInitialize(GLFWwindow** window);
// Function to create the mesh
void createMesh(GLMesh& mesh);

// Function to generate a r/g/b value
float genColorValue();


int main()
{
    if (!progInitialize(&window))
        return EXIT_FAILURE;

    createMesh(mesh);

    createTextures();

    glEnable(GL_DEPTH_TEST);
    

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs");

    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
    ourShader.setInt("texture3", 2);
    ourShader.setInt("texture4", 3);
    ourShader.setInt("texture5", 4);
    ourShader.setInt("texture6", 5);
    ourShader.setInt("texture7", 6);
    ourShader.setInt("texture8", 7);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // Clears frame and sets background color
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // Activate Shader
        ourShader.use();

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            // Print or log the error code for debugging
            std::cout << "OpenGL error: " << error << std::endl;
        }

        // Light properties
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Position of the light source
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Color of the light source
        ourShader.setLightPosition("lightPos", lightPos);
        ourShader.setLightColor("lightColor", lightColor);

        // initialize model for transformations
        glm::mat4 model = glm::mat4(1.0f);
       
        // Transforms the first object (Lower Cylinder)
        // Move to the left
        glm::mat4 translation = glm::translate(glm::vec3(-1.5f, -0.625, 0.0f)); // -0.625 places it ontop of the plane
        // Rotations
        glm::mat4 rotation = glm::rotate(glm::radians(30.0f), glm::vec3(0.f, 1.0f, 0.0f));
        // Sets the model
        model = translation * rotation;
        ourShader.setMat4("model", model);

        // Sets the projection (view type)
        ourShader.setMat4("projection", projection); // Changes when P is pressed

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // Bind textures For the first cylinder
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture8);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texture8);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 1);

        // First cylinder sides
        glBindVertexArray(mesh.VAOs[0]);
        // Draw the sides of the first cylinder
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[0], GL_UNSIGNED_INT, 0);

        // First cylinder top
        glBindVertexArray(mesh.VAOs[1]);
        // Draw the top of the first cylinder
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[1], GL_UNSIGNED_INT, 0);

        // First cylinder bottom
        glBindVertexArray(mesh.VAOs[2]);
        // Draw the bottom of the first cylinder
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[2], GL_UNSIGNED_INT, 0);

        // Unbind second texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Transforms the second object (Upper Cylinder)
        // Move to the left and up (to sit ontop of the other cylinder)
        translation = glm::translate(glm::vec3(-1.5f, 0.35, 0.0f)); // -0.23 places it ontop of the other cylinder
        // Sets the model
        glm::mat4 scalecylinder = glm::scale(glm::vec3(0.0f, 2.0f, 0.0f));
        model = translation * scalecylinder;

        ourShader.setMat4("model", model);

        // Second cylinder
        glBindVertexArray(mesh.VAOs[3]);

        // Bind texture for upper cylinder

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture6);
        // Transforms the third object (Pyramid)


        // Draw the second cylinder sides
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[3], GL_UNSIGNED_INT, 0);

        // second cylinder top
        glBindVertexArray(mesh.VAOs[4]);
        // Draw the top of the first cylinder
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[4], GL_UNSIGNED_INT, 0);

        // second cylinder bottom
        glBindVertexArray(mesh.VAOs[5]);
        // Draw the bottom of the first cylinder
        glDrawElements(GL_TRIANGLES, mesh.indexCounts[5], GL_UNSIGNED_INT, 0);


        // Bind textures for Pyramid
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture7);
        //Transforms the third object (Pyramid)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 2);
        // Move to the right
        translation = glm::translate(glm::vec3(-1.5f, -0.40, 0.0f)); // -0.765625 places it ontop of the plane
        // Rotate the object slightly
        rotation = glm::rotate(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // Sets the model
        model = translation * rotation;

        ourShader.setMat4("model", model);

        // Third Object (Cube)
        glBindVertexArray(mesh.VAOs[6]);

        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Bind textures for Plane
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture5);

        // Transforms the fourth object (plane)
        // Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(10.0f, 0.0f, 10.0f));
        // Move down
        translation = glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)); 
        // Sets the model
        model = translation * scale;

        ourShader.setMat4("model", model);

        // Fourth Object (Plane)
        glBindVertexArray(mesh.VAOs[7]);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[7], GL_UNSIGNED_INT, 0);

        
        float xScale = 0.625 / 0.5625;
        float zScale = 0.125 / 0.5625;

        // Bind textures sphere 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // Transforms the fifth object (sphere 1)
        // Scales the object
        scale = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
        // Rotation
        rotation = glm::rotate(glm::radians(30.0f), glm::vec3(0.0f, 0.7f, 0.0f));
        glm::mat4 rotation2 = glm::rotate(glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // Moves the object
        translation = glm::translate(glm::vec3(-0.7f, -0.7f, -1.1f));
        // Sets the model
        model = translation * rotation2 * rotation * scale;

        ourShader.setMat4("model", model);

        // Fifth Object (Sphere 1)
        glBindVertexArray(mesh.VAOs[8]);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[8], GL_UNSIGNED_INT, 0);

        // Bind textures for sphere 2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texture6); // Face Texture (Not correct currently)
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 1); // Change to 2  for face

        zScale = 0.1875 / 0.5625;

        // Transforms the fifth object (Face) (gives sphere 2)
        // Scales the object
        scale = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
        // Rotation
        rotation = glm::rotate(glm::radians(30.0f), glm::vec3(0.0f, 0.7f, 0.0f));
        rotation2 = glm::rotate(glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // Moves object
        translation = glm::translate(glm::vec3(-0.219049f, -0.7f, -0.140525f));
        // Sets the model
        model = translation * rotation2 * rotation * scale;

        ourShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[8], GL_UNSIGNED_INT, 0);
        
        // card
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 1);

        // Transforms the sixth object (card)
        // Scales the object
        scale = glm::scale(glm::vec3(xScale, 1.0f, 1.0f));
        // Rotates the object
        rotation = glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation2 = glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.7f, 0.0f));
        glm::mat4 rotation3 = glm::rotate(glm::radians(0.0f), glm::vec3(-0.5f, -0.5f, 1.0f));
        // Moves object
        translation = glm::translate(glm::vec3(0.0f, -1.45f, 2.0f)); 
        // Sets the model
        model = translation * rotation * scale; // * rotation3 * rotation2

        

        ourShader.setMat4("model", model);

        //card
        glBindVertexArray(mesh.VAOs[12]);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[12], GL_UNSIGNED_INT, 0);
        
        /*/
        // Transforms the seventh object (cone 1)
        // Moves object
        translation = glm::translate(glm::vec3(-0.02f, 0.5f, -0.58f));
        // Rotation
        rotation2 = glm::rotate(glm::radians(-25.0f), glm::vec3(0.7f, 0.0f, 0.0f));
        // Sets the model
        model = translation * rotation2;

        ourShader.setMat4("model", model);

        // seventh Object (cone)
        glBindVertexArray(mesh.VAOs[10]);

        glDrawArrays(GL_TRIANGLES, 0, mesh.indexCounts[10] / 9);

        // Transforms the seventh object (gives cone 2)
        // Moves object
        translation = glm::translate(glm::vec3(-0.55f, 0.53f, -0.30f));
        // Rotations
        rotation = glm::rotate(glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 0.1f));
        rotation2 = glm::rotate(glm::radians(-15.0f), glm::vec3(0.7f, 0.0f, 0.0f));
        // Sets the model
        model = translation * rotation2 * rotation;

        ourShader.setMat4("model", model);

        // seventh Object (cone)
        glBindVertexArray(mesh.VAOs[10]);

        glDrawArrays(GL_TRIANGLES, 0, mesh.indexCounts[10] / 9);
        */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        
        // Bind textures for Cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        //Transforms the third object (Cube)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 2);
        // Move to the right
        translation = glm::translate(glm::vec3(1.5f, -0.40, 1.0f)); // -0.765625 places it ontop of the plane
        // Rotate the object slightly
        rotation = glm::rotate(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // Sets the model
        model = translation * rotation;

        ourShader.setMat4("model", model);

        // Third Object (Cube)
        glBindVertexArray(mesh.VAOs[11]);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[11], GL_UNSIGNED_INT, 0);

        
        // Bind textures for middle cotton
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        //Transforms the third object (cotton cylinder)
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.ID, "numTextures"), 1);
        // Move to the right
        translation = glm::translate(glm::vec3(-0.45f, -0.7f, -0.6f)); // -0.765625 places it ontop of the plane
        // Rotate the object slightly
        rotation = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, -0.5f));
        // Sets the model
        model = translation * rotation;

        ourShader.setMat4("model", model);

        // Third Object (Cube)
        glBindVertexArray(mesh.VAOs[13]);

        glDrawElements(GL_TRIANGLES, mesh.indexCounts[13], GL_UNSIGNED_INT, 0);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(14, mesh.VAOs);
    glDeleteBuffers(14, mesh.VBOs);
    glDeleteBuffers(14, mesh.EBOs);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void createTextures() {
    // load textures
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // Find file
    unsigned char* data = stbi_load(("resources/FurTexture.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }


        stbi_image_free(data);
    }
    // Find file
    data = stbi_load(("resources/WoodTexture.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/visa.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture3);
        glBindTexture(GL_TEXTURE_2D, texture3);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/Black Texture.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture4);
        glBindTexture(GL_TEXTURE_2D, texture4);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/WoodTexture.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture5);
        glBindTexture(GL_TEXTURE_2D, texture5);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/tiedye.jpg"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture6);
        glBindTexture(GL_TEXTURE_2D, texture6);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/label2.png"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture7);
        glBindTexture(GL_TEXTURE_2D, texture7);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    // Find file
    data = stbi_load(("resources/lid.png"), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        glGenTextures(1, &texture8);
        glBindTexture(GL_TEXTURE_2D, texture8);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Check channels
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Not implemented to handle image with " << nrChannels << " channels" << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
}

// Function to create mesh
void createMesh(GLMesh &mesh) {

    noColor.redValue = 1.0f;
    noColor.greenValue = 1.0f;
    noColor.blueValue = 1.0f;
    noColor.alphaValue = 1.0f;

    glassColor.redValue = 0.951f;
    glassColor.greenValue = 0.9298f;
    glassColor.blueValue = 0.812f;
    glassColor.alphaValue = 1.0f;

    // All size values are 1/4 of real life sizes in inches

    // Generates the first cylinder's side vetices and indices
    std::vector<float> cylSideVertices1 = genCylSideVerts(sides, 0.35, 0.2375f, glassColor);
    std::vector<unsigned int> cylSideIndices1 = genCylSideIndices(sides);
    // Generates the first cylinder's top vetices and indices
    std::vector<float> cylTopVertices1 = genCylTopVerts(sides, 0.35, 0.2375f, glassColor);
    std::vector<unsigned int> cylTopIndices1 = genCylTopIndices(sides);
    // Generates the first cylinder's bottom vetices and indices
    std::vector<float> cylBottomVertices1 = genCylBottomVerts(sides, 0.35, 0.2375f, glassColor);
    std::vector<unsigned int> cylBottomIndices1 = genCylBottomIndices(sides);
 
    // Generates the second cylinder's vetices and indices
    std::vector<float> cylSideVertices2 = genCylSideVerts(sides, 0.03125f, 0.57f, noColor);
    std::vector<unsigned int> cylSideIndices2 = genCylSideIndices(sides);
    // Generates the second cylinder's top vetices and indices
    std::vector<float> cylTopVertices2 = genCylTopVerts(sides, 0.03125f, 0.57f, noColor);
    std::vector<unsigned int> cylTopIndices2 = genCylTopIndices(sides);
    // Generates the second cylinder's bottom vetices and indices
    std::vector<float> cylBottomVertices2 = genCylBottomVerts(sides, 0.03125f, 0.57f, noColor);
    std::vector<unsigned int> cylBottomIndices2 = genCylBottomIndices(sides);

    // Sets the length, width, and height of the pyramid
    float length = 2.21875;
    float width = 1.453125f;
    float height = 0.46875;

    // Define the vertices of the pyramid and their associated colors
    float PyramidVerts[] = {
        // Positions         // Color Coordinates    // Texture Coordinates
        -0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f,  // Bottom left (red)
         0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Bottom right (green)
         0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f,  // Top right (blue)

         0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f,  // Top right (blue)
        -0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Top left (yellow)
        -0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f,  // Bottom left (purple)

        // Front triangle
        -0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.2f,  // Bottom left (red)
         0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  1.0f, 0.2f,  // Bottom right (green)
         0.0f,   0.4f,  0.0f,   0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Top (blue)

         // Right triangle
         0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f,  // Bottom left (green)
         0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Bottom right (blue)
         0.0f,   0.4f,  0.0f,   0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f,  // Top (yellow)

          // Back triangle
         0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f,  // Bottom left (blue)
        -0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Bottom right (yellow)
         0.0f,   0.4f,  0.0f,   0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f,  // Top (purple)

           // Left triangle
        -0.35f, -0.5f,  0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f,  // Bottom left (yellow)
        -0.35f, -0.5f, -0.25f,  0.5f, 0.5f, 0.5f, 1.0f,  0.5f, 1.0f,  // Bottom right (purple)
         0.0f,   0.4f,  0.0f,   0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f,  // Top (red)
    };

    float planeVerts[] = {
        // position             color                       texture
        -1.0f,  0.0f,  1.0f,    0.6f, 0.6f, 0.6f, 1.0f,     0.0f, 10.0f,       // Vertex 0
         1.0f,  0.0f,  1.0f,    0.6f, 0.6f, 0.6f, 1.0f,     10.0f, 10.0f,     // Vertex 1
        -1.0f,  0.0f, -1.0f,    0.6f, 0.6f, 0.6f, 1.0f,     0.0f, 0.0f,         // Vertex 2
         1.0f,  0.0f, -1.0f,    0.6f, 0.6f, 0.6f, 1.0f,     10.0f, 0.0f        // Vertex 3
    };

    unsigned int planeIndices[] = {
        0, 1, 2,
        1, 2, 3
    };

    // Vertices for a cube with colors and texture coordinates
    float cubeVertices[] = {
        // Positions          // Colors               // Texture Coordinates
        // Positions          // Color                  // Texture Coordinates
         0.5f,  0.55f, 0.5f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top Right Vertex 0
         0.5f,  0.55f,-0.5f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom Right Vertex 1
        -0.5f,  0.55f,-0.5f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom Left Vertex 2
        -0.5f,  0.55f, 0.5f,   1.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Top Left Vertex 3

         0.5f,  -0.3f, 0.5f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top Right Vertex 0
         0.5f,  -0.3f,-0.5f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom Right Vertex 1
        -0.5f,  -0.3f,-0.5f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom Left Vertex 2
        -0.5f,  -0.3f, 0.5f,   1.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Top Left Vertex 3
    };

    // Indices for rendering a cube
    unsigned int cubeIndices[] = {
        0, 1, 3,  // Triangle 1
        1, 2, 3,   // Triangle 2
        4,5,6,
        5,6,7,
        0,1,4,
        1,4,5,
        1,2,5,
        2,5,6,

        2,3,6,
        3,6,7,
        0,3,4,
        3,4,7
        
    };

    float cardVerts[]{
         0.3f,  0.55f, 0.5f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top Right Vertex 0
         0.3f,  0.55f,-0.5f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom Right Vertex 1
        -0.3f,  0.55f,-0.5f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom Left Vertex 2
        -0.3f,  0.55f, 0.5f,   1.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Top Left Vertex 3
    };

    unsigned int cardIndices[]{
		0, 1, 3,  // Triangle 1
		1, 2, 3   // Triangle 2
	};

    catColor.redValue = 1.0f;
    catColor.greenValue = 1.0f;
    catColor.blueValue = 1.0f;
    catColor.alphaValue = 1.0f;

    // Shapes for the cat
    // Generates the first sphere's vertices and indices
    std::vector<float> sphereVertices1 = genSphereVerts(0.5625, catColor);
    std::vector<unsigned int> sphereIndices1 = genSphereIndices();

    // Generates the third cylinder's vertices and indices
    std::vector<float> cylVertices3 = genCylSideVerts(sides, 1.4375f, 0.5625f, catColor);
    std::vector<unsigned int> cylIndices3 = genCylSideIndices(sides);

    // Generate the pyramid/cone vertices and indices
    std::vector<float> coneVerts1 = genPyramidVerts(sides, 0.5f, 0.25f, catColor);
    std::vector<unsigned int> conIndices1 = genPyramidIndices(sides);

    std::vector<float> cylVertices4 = genCylSideVerts(sides, 1.1f, 0.1f, catColor);
    std::vector<unsigned int> cylIndices4 = genCylSideIndices(sides);


    // Initialize buffers
    glGenVertexArrays(14, mesh.VAOs);
    glGenBuffers(14, mesh.VBOs);
    glGenBuffers(14, mesh.EBOs);

    // First Cylinder Sides
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[0]);

    // VBO of the first cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, cylSideVertices1.size() * sizeof(float), cylSideVertices1.data(), GL_STATIC_DRAW);

    // EBO of the first cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylSideIndices1.size() * sizeof(unsigned int), cylSideIndices1.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // First Cylinder Top
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[1]);

    // VBO of the top of the first cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, cylTopVertices1.size() * sizeof(float), cylTopVertices1.data(), GL_STATIC_DRAW);

    // EBO of the first cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylTopIndices1.size() * sizeof(unsigned int), cylTopIndices1.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // First Cylinder Bottom
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[2]);

    // VBO of the bottom of the first cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, cylBottomVertices1.size() * sizeof(float), cylBottomVertices1.data(), GL_STATIC_DRAW);

    // EBO of the first cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylBottomIndices1.size() * sizeof(unsigned int), cylBottomIndices1.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Second Cylinder Sides
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[3]);

    // VBO of the sides of the second cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[3]);
    glBufferData(GL_ARRAY_BUFFER, cylSideVertices2.size() * sizeof(float), cylSideVertices2.data(), GL_STATIC_DRAW);

    // EBOs of second cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylSideIndices2.size() * sizeof(unsigned int), cylSideIndices2.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Second Cylinder Top
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[4]);

    // VBO of the Top of the second cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[4]);
    glBufferData(GL_ARRAY_BUFFER, cylTopVertices2.size() * sizeof(float), cylTopVertices2.data(), GL_STATIC_DRAW);

    // EBOs of second cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylTopIndices2.size() * sizeof(unsigned int), cylTopIndices2.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Second Cylinder Bottom
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[5]);

    // VBO of the bottom of the second cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[5]);
    glBufferData(GL_ARRAY_BUFFER, cylBottomVertices2.size() * sizeof(float), cylBottomVertices2.data(), GL_STATIC_DRAW);

    // EBOs of second cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylBottomIndices2.size() * sizeof(unsigned int), cylBottomIndices2.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // First Cube (Third Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[6]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidVerts), PyramidVerts, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Plane (Fourth Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[7]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Spheres (Fifth Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[8]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[8]);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices1.size() * sizeof(float), sphereVertices1.data(), GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices1.size() * sizeof(unsigned int), sphereIndices1.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Third Cylinder (Sixth Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[9]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[9]);
    glBufferData(GL_ARRAY_BUFFER, cylVertices3.size() * sizeof(float), cylVertices3.data(), GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[9]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylIndices3.size() * sizeof(unsigned int), cylIndices3.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Cones (Seventh Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[10]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[10]);
    glBufferData(GL_ARRAY_BUFFER, coneVerts1.size() * sizeof(float), coneVerts1.data(), GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, conIndices1.size() * sizeof(unsigned int), conIndices1.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Cube (Seventh Object)
// bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[11]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[11]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[11]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Plane (Fourth Object)
    // bind the Vertex Array Object
    glBindVertexArray(mesh.VAOs[12]);

    // VBO of the cube
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[12]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cardVerts), cardVerts, GL_STATIC_DRAW);

    // EBO of cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[12]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cardIndices), cardIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(mesh.VAOs[13]);

    // VBO of the first cylinder
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOs[13]);
    glBufferData(GL_ARRAY_BUFFER, cylVertices4.size() * sizeof(float), cylVertices4.data(), GL_STATIC_DRAW);

    // EBO of the first cylinder
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOs[13]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylIndices4.size() * sizeof(unsigned int), cylIndices4.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attibute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);



    mesh.indexCounts[0] = cylSideIndices1.size();
    mesh.indexCounts[1] = cylTopIndices2.size();
    mesh.indexCounts[2] = cylBottomIndices1.size();
    mesh.indexCounts[3] = cylSideIndices2.size();
    mesh.indexCounts[4] = cylTopIndices2.size();
    mesh.indexCounts[5] = cylBottomIndices2.size();
    mesh.indexCounts[7] = sizeof(planeIndices) / sizeof(unsigned int);
    mesh.indexCounts[8] = sphereIndices1.size();
    mesh.indexCounts[9] = cylIndices3.size();
    mesh.indexCounts[10] = coneVerts1.size();
    mesh.indexCounts[11] = sizeof(cubeIndices) / sizeof(unsigned int);
    mesh.indexCounts[12] = sizeof(cardIndices) / sizeof(unsigned int);
    mesh.indexCounts[13] = cylIndices4.size();
}

// Function to generate the side veritces of a cylinder
std::vector<float> genCylSideVerts(int sides, float height, float radius, color color) {

    std::vector<float> vertices;

    float angleStep = 2.0f * PI / sides;

    float halfHeight = height / 2.0f;
    

    // Bottom circle vertices
    for (int i = 0; i < sides; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Position
        vertices.push_back(x);
        vertices.push_back(-halfHeight); // y (bottom)
        vertices.push_back(z);

        // Color 
        vertices.push_back(color.redValue);     // r
        vertices.push_back(color.greenValue);   // g
        vertices.push_back(color.blueValue);    // b
        vertices.push_back(color.alphaValue);   // a

        vertices.push_back(static_cast<float>(i) / static_cast<float>(sides));    // u
        vertices.push_back(0.0f);   // v
    }

    // Top circle vertices
    for (int i = 0; i < sides; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Position
        vertices.push_back(x);
        vertices.push_back(halfHeight); // y (top)
        vertices.push_back(z);

        // Color 
        vertices.push_back(color.redValue);     // r
        vertices.push_back(color.greenValue);   // g
        vertices.push_back(color.blueValue);    // b
        vertices.push_back(color.alphaValue);   // a
        

        vertices.push_back(static_cast<float>(i) / static_cast<float>(sides));    // u
        vertices.push_back(1.0f);   // v
    }

    return vertices;
}

// Function to generate the top veritces of a cylinder
std::vector<float> genCylTopVerts(int sides, float height, float radius, color color) {

    std::vector<float> vertices;

    float angleStep = 2.0f * PI / sides;

    float halfHeight = height / 2.0f;

    // Top circle vertices
    for (int i = 0; i < sides; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Position
        vertices.push_back(x);
        vertices.push_back(halfHeight); // y (top)
        vertices.push_back(z);

        // Color 
        vertices.push_back(color.redValue);     // r
        vertices.push_back(color.greenValue);   // g
        vertices.push_back(color.blueValue);    // b
        vertices.push_back(color.alphaValue);   // a


        // Texture coordinates for the top circle
        vertices.push_back(0.5f + 0.5f * cos(angle));  // u
        vertices.push_back(0.5f + 0.5f * sin(angle));  // v
    }
    
    // For triangle fan
    float y =  halfHeight; 

    // Position
    vertices.push_back(0.0f);  // x
    vertices.push_back(y);     // y
    vertices.push_back(0.0f);  // z

    // Color 
    vertices.push_back(color.redValue);     // r
    vertices.push_back(color.greenValue);   // g
    vertices.push_back(color.blueValue);    // b
    vertices.push_back(color.alphaValue);   // a
         

    // Texture coordinates for the center points
    vertices.push_back(0.5f);  // u (center point)
    vertices.push_back(0.5f);  // v (center point)
        
    return vertices;
}

// Function to generate the bottom veritces of a cylinder
std::vector<float> genCylBottomVerts(int sides, float height, float radius, color color) {

    std::vector<float> vertices;

    float angleStep = 2.0f * PI / sides;

    float halfHeight = height / 2.0f;

    // Bottom circle vertices
    for (int i = 0; i < sides ; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Position
        vertices.push_back(x);
        vertices.push_back(-halfHeight); // y (bottom)
        vertices.push_back(z);

        // Color 
        vertices.push_back(color.redValue);     // r
        vertices.push_back(color.greenValue);   // g
        vertices.push_back(color.blueValue);    // b
        vertices.push_back(color.alphaValue);   // a
        

        // Texture coordinates for the bottom circle
        vertices.push_back(0.5f + 0.5f * cos(angle));  // u
        vertices.push_back(0.5f + 0.5f * sin(angle));  // v
    }

    // For triangle fan
    float y = -halfHeight;

    // Position
    vertices.push_back(0.0f);  // x
    vertices.push_back(y);     // y
    vertices.push_back(0.0f);  // z

    // Color 
    vertices.push_back(color.redValue);     // r
    vertices.push_back(color.greenValue);   // g
    vertices.push_back(color.blueValue);    // b
    vertices.push_back(color.alphaValue);   // a


    // Texture coordinates for the center points
    vertices.push_back(0.5f);  // u (center point)
    vertices.push_back(0.5f);  // v (center point)

    return vertices;
}

// Function to generate a cylinder's side indices
std::vector<unsigned int> genCylSideIndices(int sides) {

    std::vector<unsigned int> indices;

    for (int i = 0; i < sides; ++i) {
        // Calculate indices for the two triangles that make up each side face
        int currentVertex = i;
        int nextVertex = (i + 1) % sides;
        int topCurrentVertex = i + sides;
        int topNextVertex = (i + 1) % sides + sides;

        // First triangle
        indices.push_back(currentVertex);
        indices.push_back(nextVertex);
        indices.push_back(topNextVertex);

        // Second triangle
        indices.push_back(topNextVertex);
        indices.push_back(topCurrentVertex);
        indices.push_back(currentVertex);
    }

    return indices;

}

// Function to generate a cylinder's bottom indices
std::vector<unsigned int> genCylBottomIndices(int sides) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < sides; ++i) {
        int currentVertex = i;
        int nextVertex = (i + 1) % sides;

        // Triangle fan indices
        indices.push_back(currentVertex);
        indices.push_back(nextVertex);
        indices.push_back(sides);
    }

    return indices;
}

// Function to generate a cylinder's top indices
std::vector<unsigned int> genCylTopIndices(int sides) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < sides; ++i) {
        int currentVertex = i;
        int nextVertex = (i + 1) % sides;

        // Triangle fan indices
        indices.push_back(currentVertex);
        indices.push_back(nextVertex);
        indices.push_back(sides); // Center point
    }

    return indices;
}

// Function for generating a sphere's vertices
std::vector<float> genSphereVerts(float radius, color color) {
    std::vector<float> vertices;
    float phi, theta;

    // Generate vertices and colors for the sphere
    for (int i = 0; i <= rings; ++i) {
        phi = static_cast<float>(PI) * i / rings; // Vertical angle

        for (int j = 0; j <= segments; ++j) {
            theta = static_cast<float>(2 * PI) * j / segments; // Horizontal angle

            // Calculate the vertex position
            float x = radius * std::sin(phi) * std::cos(theta);
            float y = radius * std::sin(phi) * std::sin(theta);
            float z = radius * std::cos(phi);

            // Add the vertex position to the vector
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Color 
            vertices.push_back(color.redValue);     // r
            vertices.push_back(color.greenValue);   // g
            vertices.push_back(color.blueValue);    // b
            vertices.push_back(color.alphaValue);   // a

            // Calculate and push texture coordinates
            float u = static_cast<float>(j) / segments; // U texture coordinate
            float v = static_cast<float>(i) / rings;    // V texture coordinate
            /*
            float v = 2 * ((y - (-1.0f)) / ((1.0f) - (-1.0f))) - 1;    // V texture coordinate
            v = v * 1.25;
            v = v * 0.5 + 0.5;
            */
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    return vertices;
}

// Function for generating a sphere's indices
std::vector<unsigned int> genSphereIndices()
{
    std::vector<unsigned int> indices;

    // Generate indices for rendering triangles
    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < segments; ++j) {
            int current = i * (segments + 1) + j;
            int next = current + segments + 1;

            // Define two triangles using indices
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return indices;
}

// Function to generate a pyramid's vertices
std::vector<float> genPyramidVerts(int sides, float height, float radius, color color)
{
    std::vector<float> vertices;
    vertices.reserve((sides + 1) * 9); // Each vertex has 9 components now (including alpha)

    float angleIncrement = 2 * PI / static_cast<float>(sides);

    float r = color.redValue;
    float g = color.greenValue;
    float b = color.blueValue;
    float a = color.alphaValue;

    // Vertex at the top of the pyramid/cone
    float topVertex[] = { 0.0f, height / 2.0f, 0.0f, r, g, b, a, 0.5f, 1.0f }; // Center of the texture

    for (int i = 0; i < sides; ++i) {
        // Calculate the coordinates for the base of the pyramid/cone
        float x1 = radius * cos(i * angleIncrement);
        float z1 = radius * sin(i * angleIncrement);
        float x2 = radius * cos((i + 1) * angleIncrement);
        float z2 = radius * sin((i + 1) * angleIncrement);

        // Calculate the texture coordinates for the sides
        float u1 = static_cast<float>(i) / static_cast<float>(sides - 1); // Range [0, 1]
        float u2 = static_cast<float>(i + 1) / static_cast<float>(sides - 1);

        // Add vertices for the base triangle with texture coordinates
        vertices.insert(vertices.end(), {
            x1, -height / 2.0f, z1,
            r, g, b, a,
            u1, 0.0f,

            x2, -height / 2.0f, z2,
            r, g, b, a,
            u2, 0.0f,

            0.0f, -height / 2.0f, 0.0f,
            r, g, b, a,
            0.5f, 0.5f
            });

        // Add vertices for the side triangle with texture coordinates
        vertices.insert(vertices.end(), {
            x1, -height / 2.0f, z1,
            r, g, b, a,
            u1, 0.0f,

            x2, -height / 2.0f, z2,
            r, g, b, a,
            u2, 0.0f,

            0.0f, height / 2.0f, 0.0f,
            r, g, b, a,
            0.5f, 1.0f
            });
    }

    // Add vertices for the base
    for (int i = 0; i < sides; ++i) {
        float x1 = radius * cos(i * angleIncrement);
        float z1 = radius * sin(i * angleIncrement);
        float x2 = radius * cos((i + 1) * angleIncrement);
        float z2 = radius * sin((i + 1) * angleIncrement);

        // Calculate the texture coordinates for the base
        float u1 = 0.5f + 0.5f * cos(i * angleIncrement);
        float u2 = 0.5f + 0.5f * cos((i + 1) * angleIncrement);

        // Add vertices for the base triangle with texture coordinates
        vertices.insert(vertices.end(), {
            x1, -height / 2.0f, z1,
            r, g, b, a,
            u1, 0.0f,

            x2, -height / 2.0f, z2,
            r, g, b, a,
            u2, 0.0f,

            0.0f, -height / 2.0f, 0.0f,
            r, g, b, a,
            0.5f, 0.5f
            });
    }

    return vertices;
}

// Function to generate a pyramid's indices
std::vector<unsigned int> genPyramidIndices(int sides)
{
    std::vector<unsigned int> indices;

    // Connect the apex vertex to each base vertex
    for (unsigned int i = 0; i < sides; ++i) {
        indices.push_back(0);  // Apex vertex is 0
        indices.push_back(2 + i);
        indices.push_back(2 + (i +1) % sides);
    }

    // Connect base vertices to form the base
    for (unsigned int i = 0; i < sides; ++i) {
        indices.push_back(1);  // Center vertex of base is 1
        indices.push_back(2 + i);
        indices.push_back(2 + (i + 1) % sides);
    }

    return indices;
}

bool progInitialize(GLFWwindow** window) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2DScene Recreation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    // Setting callbacks
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
    glfwSetCursorPosCallback(*window, mouse_callback);
    glfwSetScrollCallback(*window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

// Function to generate a random r/g/b value
float genColorValue() {
    return (float) rand()/RAND_MAX;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)	// When pressing W
        camera.ProcessKeyboard(FORWARD, deltaTime);			// move forwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	// When pressing S
        camera.ProcessKeyboard(BACKWARD, deltaTime);		// move backwards
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	// When pressing A
        camera.ProcessKeyboard(LEFT, deltaTime);			// move to the left
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	// When pressing D
        camera.ProcessKeyboard(RIGHT, deltaTime);			// move to the right
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)	// When pressing Q
        camera.ProcessKeyboard(UP, deltaTime);				// move up
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)	// When pressing E
        camera.ProcessKeyboard(DOWN, deltaTime);			// move down
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)	// When P is pressed
        isPerspective = !isPerspective;                     // toggle orthographic and 
        toggleView();                                       // perspective views
}

// Function for toggling view between orthograpic and persepctive
void toggleView() {
    if (isPerspective)
    {
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    }
    else
    {
        float orthoWidth = 1.0f; 
        float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        projection = glm::ortho(-orthoWidth * aspectRatio, orthoWidth * aspectRatio, -orthoWidth, orthoWidth, 0.0001f, 100.0f);
    }
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}