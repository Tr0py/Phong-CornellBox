#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "objloader.hpp"
#include "model.h"


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void my_sphere(float radius, unsigned int rings, unsigned int sectors);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 1.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.6f, 0.0f);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computer Graphics Final", NULL, NULL);
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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("obj.vs", "obj.fs");
    Shader lampShader("lamp.vs", "lamp.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    float verticesOut[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,

		/*
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,
		*/

        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 1.5, configure the cube's VAO (and VBO)
    unsigned int outVBO, outVAO;
    glGenVertexArrays(1, &outVAO);
    glGenBuffers(1, &outVBO);

    glBindBuffer(GL_ARRAY_BUFFER, outVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesOut), verticesOut, GL_STATIC_DRAW);

    glBindVertexArray(outVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// ------------------ OBJ LOADER -------------------
	// Read our .obj file
	//std::vector<glm::vec3> verticesObj;
	//std::vector<glm::vec2> uvsObj;
	//std::vector<glm::vec3> normalsObj; // Won't be used at the moment.
	//bool res = loadOBJ("./sphere.obj", verticesObj, uvsObj, normalsObj);
	//printf("123123123123\n");

	//GLuint vertexbuffer;
	//glGenBuffers(1, &vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//GLuint uvbuffer;
	//glGenBuffers(1, &uvbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	//glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    //unsigned int objVBO, objVAO;
    //glGenVertexArrays(1, &objVAO);
    //glGenBuffers(1, &objVBO);

    //glBindBuffer(GL_ARRAY_BUFFER, objVBO);
	//glBufferData(GL_ARRAY_BUFFER, verticesObj.size() * sizeof(glm::vec3), &verticesObj[0], GL_STATIC_DRAW);

    //glBindVertexArray(objVAO);

    //// position attribute
	//glBufferData(GL_ARRAY_BUFFER, verticesObj.size() * sizeof(glm::vec3), &verticesObj[0], GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //// normal attribute
	//glBufferData(GL_ARRAY_BUFFER, normalsObj.size() * sizeof(glm::vec2), &normalsObj[0], GL_STATIC_DRAW);
    //glEnableVertexAttribArray(1);
	// load models
    // -----------
    Model ourModel("./sphere.obj");


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.5f)); // larger
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(outVAO);
        glDrawArrays(GL_TRIANGLES, 0, 30);

		//START------------------------
        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 0.2f, 0.7f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
		glm::vec3 obj2Pos(-0.3f, -0.6f, -0.4);
        model = glm::translate(model, obj2Pos);
        model = glm::scale(model, glm::vec3(0.3f)); // smaller
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

		//-----------OBJ------
        lightingShader.use();
        lightingShader.setVec3("objectColor", 0.8f, 0.5f, 0.4f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
		glm::vec3 obj3Pos(0.3f, -0.6f, -0.4);
        model = glm::translate(model, obj3Pos);
        model = glm::scale(model, glm::vec3(0.02f)); // smaller
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", model);
        ourModel.Draw(lightingShader);


        //// render the cube
        //glBindVertexArray(objVAO);
		//glDrawArrays(GL_TRIANGLES, 0, verticesObj.size() );


		//END-------------------------

        // also draw the lamp object
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
        lampShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
	

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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

/*
void my_sphere(float radius, unsigned int rings, unsigned int sectors)
{
	float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    int r, s;

    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator n = normals.begin();
    std::vector<GLfloat>::iterator t = texcoords.begin();
    for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
            float const y = sin( -M_PI_2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

            *t++ = s*S;
            *t++ = r*R;

            *v++ = x * radius;
            *v++ = y * radius;
            *v++ = z * radius;

            *n++ = x;
            *n++ = y;
            *n++ = z;
    }

    indices.resize(rings * sectors * 4);
    std::vector<GLushort>::iterator i = indices.begin();
    for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
            *i++ = r * sectors + s;
            *i++ = r * sectors + (s+1);
            *i++ = (r+1) * sectors + (s+1);
            *i++ = (r+1) * sectors + s;
    }

}
*/
