
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>

#include <GL/glew.h>

//#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <string> 
#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;

unsigned int teapot_vao = 0;
int width = 800.0;
int height = 600.0;
GLuint loc1;
GLuint loc2;

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
glm::mat4 translateX = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));

std::string readShaderSource(const std::string& fileName)
{
	std::ifstream file(fileName.c_str()); 
	if(file.fail()) {
		cout << "error loading shader called " << fileName;
		exit (1); 
	} 
	
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();

	return stream.str();
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	std::string outShader = readShaderSource(pShaderText);
	const char* pShaderSource = outShader.c_str();

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, "../Raytracer/src/shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
    AddShader(shaderProgramID, "../Raytracer/src/shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };


	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);

	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS





void generateObjectBufferTeapot () {
	GLuint vp_vbo = 0;

	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normals");
	
	glGenBuffers (1, &vp_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	//glBufferData (GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof (float), teapot_vertex_points, GL_STATIC_DRAW);
	GLuint vn_vbo = 0;
	glGenBuffers (1, &vn_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	//glBufferData (GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof (float), teapot_normals, GL_STATIC_DRAW);
  
	glGenVertexArrays (1, &teapot_vao);
	glBindVertexArray (teapot_vao);

	glEnableVertexAttribArray (loc1);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer (loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (loc2);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer (loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}


#pragma endregion VBO_FUNCTIONS


void display(){

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	//glEnable (GL_DEPTH_TEST); // enable depth-testing
	//glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram (shaderProgramID);


	////Declare your uniform variables that will be used in your shader
	//int matrix_location = glGetUniformLocation (shaderProgramID, "model");
	//int view_mat_location = glGetUniformLocation (shaderProgramID, "view");
	//int proj_mat_location = glGetUniformLocation (shaderProgramID, "proj");
	

	//Here is where the code for the viewport lab will go, to get you started I have drawn a t-pot in the bottom left
	//The model transform rotates the object by 45 degrees, the view transform sets the camera at -40 on the z-axis, and the perspective projection is setup using Antons method

	// bottom-left
	/*mat4 view = translate (identity_mat4 (), vec3 (0.0, 0.0, -40.0));
	mat4 persp_proj = perspective(45.0, (float)width/(float)height, 0.1, 100.0);
	mat4 model = rotate_z_deg (identity_mat4 (), 45);

	glViewport (0, 0, width / 2, height / 2);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	glDrawArrays (GL_TRIANGLES, 0, teapot_vertex_count);
*/
	// bottom-right
		
	// top-left

	// top-right

    //glutSwapBuffers();
}


void updateScene() {	

		// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	/*static double  last_time = 0;
	double  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;*/

	// Draw the next frame
	//glutPostRedisplay();
}


GLFWwindow* window;
const GLFWvidmode* videMode;
GLuint frameBufferTexuture;

// Create texture that is the frame buffer
void CreateFrameBufferTexture()
{
	glGenTextures(1, &frameBufferTexuture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexuture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLvoid* black = NULL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
		GL_FLOAT, black);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint vertexArrayObject;
GLuint vertexBufferObject;
GLbyte byteBuffer[] = 
{
	(GLbyte)-1, (GLbyte)-1,
	(GLbyte)1, (GLbyte)-1,
	(GLbyte)1, (GLbyte)1,
	(GLbyte)1, (GLbyte)1,
	(GLbyte)-1, (GLbyte)1,
	(GLbyte)-1, (GLbyte)-1
};

// Create a Vertex Array Object with full-screen quad Vertex Buffer Object
void QuadFullScreenVAO()
{
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, (2 * 6), byteBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_BYTE, false, 0, NULL);
	glBindVertexArray(0);
}

void init()
{

	if (glfwInit() != GL_TRUE)
	{
		std::cout << "Failed to Initialise\n";
		return;
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "Raytracer Compute Shader", NULL, NULL);

	videMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (videMode->width - width)/2, (videMode->height - height)/2);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwShowWindow(window);

	// Create frame buffer textures
	CreateFrameBufferTexture();
	// Create a Vertex Array Object with full-screen quad Vertex Buffer Object
	QuadFullScreenVAO();

}

void loop()
{
	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		glfwPollEvents();
		glViewport(0, 0, width, height);

		glfwSwapBuffers(window);
	}
}

int main(int argc, char** argv){
	
	init();

	loop();

    return 0;
}











