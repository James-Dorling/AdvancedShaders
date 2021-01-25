#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

#include <Model.h>

#include<string>
#include <iostream>
#include <numeric>



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
//unsigned int loadTexture2(char const * path);
void setVAO(vector <float> vertices);

// camera
Camera camera(glm::vec3(0,0,9));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int VBO, VAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Light
glm::vec3 lightPos(1.f, 1.5f, 0.8f);

int debugMode = 0;
int debugDrawMode = 0;
int tessInner = 1;
int tessOuter = 1;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// simple vertex and fragment shader 
	//Shader shader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs");
	Shader shader("..\\shaders\\tessVert.vs", "..\\shaders\\tessFrag.fs", "..\\shaders\\tessGeo.gs", "..\\shaders\\tessControl.tcs", "..\\shaders\\tessEval.tes");
	Shader normShader("..\\shaders\\plainVert.vs", "..\\shaders\\normFrag.fs", "..\\shaders\\normGeo.gs");
	//Shader billShader("..\\shaders\\billboardVert.vs", "..\\shaders\\billboardFrag.fs", "..\\shaders\\billboardGeo.gs");
	//Shader tessShader("..\\shaders\\tessVert.vs", "..\\shaders\\tessControl.tcs", "..\\shaders\\tessEval.tes");

	// load model with assimp
	Model model1("..\\resources\\nano\\nanosuit\\nanosuit.obj");
	//Model model1("..\\resources\\elephant\\elefante.obj");
	


	//============================ TASK ONE ===========================================

	//float points[] = {
	//	               -2.5f, 0.0f, 0.0f, // x y z,
	//	                2.5f, 0.0f, 0.0f, 
	//	                0.0f, 2.5f, 0.0f
	//};
	//
	//unsigned int VBO, VAO;
	//glGenBuffers(1, &VBO);
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	//
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//glBindVertexArray(0);

	//=======================================================================

	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

	    shader.use();
	    shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
	
		shader.setVec3("light.ambient",  0.3f, 0.3f, 0.3f);
		shader.setVec3("light.diffuse",  0.4f, 0.4f, 0.4f);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		shader.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);

		shader.setInt("tessLevelOut", tessOuter);
		shader.setInt("tessLevelIn", tessInner);

		shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);

		if (debugDrawMode == 0)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (debugDrawMode == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else if (debugDrawMode == 2)
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		
		glBindVertexArray(VAO);
		//glDrawElements(GL_PATCHES, , GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawArrays(GL_PATCHES, 0, 3);
		model1.Draw(shader);
		
		normShader.use();
		normShader.setMat4("projection", projection);
		normShader.setMat4("view", view);
		normShader.setMat4("model", model);
		normShader.setInt("Debug", debugMode);
		model1.Draw(normShader);
		
		//Billboards should work with minimal tweaking. I dont think theyre being applied honestly.
		//unsigned int billb = loadTexture("..\\resources\\2D Images\\armos.png");
		//billShader.use();
		//billShader.setMat4("projection", projection);
		//billShader.setMat4("view", view);
		//billShader.setMat4("model", model);
		//billShader.setFloat("threshold", 0.35);
		//billShader.setInt("billboard", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, billb);

		glfwSwapBuffers(window);
		glfwPollEvents();


	}


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

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		debugMode = 0;
		debugDrawMode = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		debugMode = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		debugMode = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		debugDrawMode = 1;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		debugDrawMode = 2;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		debugMode = 0;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		debugDrawMode = 0;


	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.y += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.y -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.x -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.x += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		lightPos.z -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		lightPos.z += 0.01f;

	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
	{
		tessOuter = 1;
		tessInner = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
	{
		tessOuter = 2;
		tessInner = 2;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	{
		tessOuter = 2;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
	{
		tessInner = 2;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
	{
		tessOuter = 5;
		tessInner = 5;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		tessOuter = 5;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
	{
		tessInner = 5;
	}

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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

unsigned int loadTexture(char const * path)
{
	//stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
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
		stbi_image_free(data);
		std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID <<  std::endl;

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		
	}

	return textureID;
}


void setVAO(vector <float> vertices) {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);		
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);

	//xyz
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



