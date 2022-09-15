#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"


#include <iostream>
#include <filesystem>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Texture;

    bool operator==(const Vertex& rhs) const noexcept
    {
        bool pos = rhs.Position.x == this->Position.x && rhs.Position.y == this->Position.y && rhs.Position.z == this->Position.z;
        bool tex = rhs.Texture.x == this->Texture.x && rhs.Texture.y == this->Texture.y;
        return pos && tex;
    }

};

//timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//pitch and yaw
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400;
float lastY = 300;
bool firstMouse = true;

glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

float ambientStrength = 0.1f; //ambient lighting coefficient
float specularStrength = 0.5f; //specular lighting coefficient

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(1200, 900, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    Shader lightCubeShader("shader_light.vs", "shader_light.fs");
    Shader lightingShader("shader.vs", "shader.fs");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };


    std::ifstream readObj;
    readObj.open("Aerospace.obj");

    if (!readObj) {
        std::cout << "Unable to open file";
        exit(1); // terminate with error
    }

    std::string currentLine;

    std::vector<glm::vec3> point_vertex({});
    std::vector<glm::vec3> normal_vertex;
    std::vector<glm::vec2> texture_vertex;


    std::vector<int> indices;

    std::vector<Vertex> vertex_data;


    int count = 0;
    while (getline(readObj, currentLine))
    {
        std::string sample;
        std::stringstream str;
        if (currentLine[0] == 'v')
        {
            if (currentLine[1] == 'n')
            {
                glm::vec3 temp;
                str << currentLine.substr(3, currentLine.length());
                getline(str, sample, ' ');
                float num = std::stof(sample);
                temp.x = num;

                getline(str, sample, ' ');
                num = std::stof(sample);
                temp.y = num;

                getline(str, sample, ' ');
                num = std::stof(sample);
                temp.z = num;

                normal_vertex.push_back(temp);
            }
            else if (currentLine[1] == 't')
            {
                str << currentLine.substr(3, currentLine.length());
                
                glm::vec2 temp;
                str << currentLine.substr(3, currentLine.length());
                getline(str, sample, ' ');
                float num = std::stof(sample);
                temp.x = num;

                getline(str, sample, ' ');
                num = std::stof(sample);
                temp.y = num;

                texture_vertex.push_back(temp);
            }

            else
            {
                str << currentLine.substr(2, currentLine.length());
                glm::vec3 temp;
                str << currentLine.substr(3, currentLine.length());
                getline(str, sample, ' ');
                float num = std::stof(sample);
                temp.x = num;

                getline(str, sample, ' ');
                num = std::stof(sample);
                temp.y = num;

                getline(str, sample, ' ');
                num = std::stof(sample);
                temp.z = num;

                point_vertex.push_back(temp);
            }
        }
        else if (currentLine[0] == 'f')
        {
            std::stringstream next_layer;
            str << currentLine.substr(2, currentLine.length());
            
            
            Vertex point = {};
            while (getline(str, sample, ' '))
            {
                if (sample.find("//") == -1)
                {
                    std::stringstream c;
                    c << sample;
                    getline(c, sample, '/');
                    point.Position = point_vertex[std::stoi(sample)];
                    getline(c, sample, '/');
                    point.Texture = texture_vertex[std::stoi(sample)];
                    getline(c, sample, '/');
                    point.Normal = normal_vertex[std::stoi(sample)];
                    std::vector<Vertex>::iterator itr = std::find(vertex_data.begin(), vertex_data.end(), point);

                    if (itr != vertex_data.end())
                    {
                        indices.push_back(count);
                        vertex_data.push_back(point);
                        count++;
                    }
                    else
                    {
                        int recur_ind = std::distance(vertex_data.begin(), itr);
                        indices.push_back(recur_ind);
                    }
                }

                else
                {
                    std::size_t ind = sample.find("//");
                    unsigned int v = std::stoi(sample.substr(0, ind));
                    point.Position = point_vertex[v - 1];
                    indices.push_back(v - 1);
                }
            }
            
        }
    }

    std::cout << indices[100];

    unsigned int VBO, VAO, IBO;
    //Generate Vertex buffer objects and vertex array object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, point_vertex.size() * sizeof(glm::vec3), &point_vertex[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices).size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  //set vertex attribute pointers


    /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); //set normal vec attribute pointers

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); //set texcoords for vertex shaders*/

    //Lighting VAO
    unsigned int lightVAO, VBO_2;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO_2);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  //set vertex attribute pointers
    */

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    lightCubeShader.use();
    glUniform1i(glGetUniformLocation(lightCubeShader.ID, "texture1"), 0); //manually
    lightCubeShader.setInt("texture2", 1); //using shaders' function previous defined
    
    

    float visibility = 0.0f;
    
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    //unsigned int specularMap = loadTexture("lighting_maps_specular_color.png");
    //unsigned int emissionMap = loadTexture("matrix.jpg");

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    //lightingShader.setInt("material.emission", 2);

    glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f);
    float ang = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        


        //rendering commands
        glClearColor(0.6f, 0.7f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        lightColor.x = (float)sin(glfwGetTime() * 2.0);
        lightColor.y = (float)sin(glfwGetTime() * 1.3);
        lightColor.z = (float)sin(glfwGetTime() * 0.7);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        lightingShader.use();

        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        //directional light setup
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        //point light setup
        for (int i = 0; i < 4; i++) {
            std::string number = std::to_string(i);
            lightingShader.setVec3("potLight[" + number + "].position", pointLightPositions[0]);
            lightingShader.setFloat("potLight[" + number + "].constant", 1.0f);
            lightingShader.setFloat("potLight[" + number + "].linear", 0.09f);
            lightingShader.setFloat("potLight[" + number + "].quadratic", 0.032f);
            lightingShader.setVec3("potLight[" + number + "].ambient", 0.2f, 0.2f, 0.2f);
            lightingShader.setVec3("potLight[" + number + "].diffuse", 0.5f, 0.5f, 0.5f);
            lightingShader.setVec3("potLight[" + number + "].specular", 1.0f, 1.0f, 1.0f);
        }
        
        //spotlight setup
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

        lightingShader.setFloat("material.shininess", 64.0f);

        

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.f);
        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        

        glm::mat4 model = glm::mat4(1.0f);
        
        model = glm::translate(model, trans); // translate it down so it's at the center of the scene
        model = glm::rotate(model, ang, glm::vec3(0.0f, 1.0f, 0.0f));

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            trans.y += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            trans.y -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            trans.x -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            trans.x += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            trans.z -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            trans.z += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            ang += 0.01f;

        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, emissionMap);

        //glBindVertexArray(VAO);
        /*
        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        lightCubeShader.use();


        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);  
        lightCubeShader.setVec3("lightColor", lightColor);

        glBindVertexArray(lightVAO);
        
        for (int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //check and call events and swap the buffers

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);*/

    glfwTerminate();
    return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyBoard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyBoard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyBoard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyBoard(RIGHT, deltaTime);

}

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

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    
    if (data) {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}