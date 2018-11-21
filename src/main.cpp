#include "luarium/core/main.h"

//void GLAPIENTRY glDebugFunc(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam);
Camera* Camera::ACTIVE = NULL;
Shader* Shader::ACTIVE = NULL;

// settings 
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

LuaFile test("lua/test.lua");

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
	std::cout<<"Luarium b0.0.1"<<std::endl;

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	 // glfw window creation
 	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hippotest", NULL, NULL);
	if (window == NULL) {
		std::cout << "[!!!] MAIN|58: Failed to create GLFW window" << std::endl;
		std::cin.get();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "[!!!] MAIN|72: Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//debug settings
	//------------------------------
	std::cout<<glGetString(GL_VERSION)<<std::endl;
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debug_callback, 0);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	std::cout<<test.get<std::string>("teststring")<<std::endl;

	// build and compile shaders
	Shader beh(SHADER_PATH_V, SHADER_PATH_F);
	Shader::ACTIVE = &beh;

	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");

	Camera::ACTIVE = new Camera(glm::vec3(0,0,0));

	// load models
	Model aa("model/red.stl", glm::vec3(0,1,0));
	Model waah("model/waa.dae", glm::vec3(0,0,-2));

//	Rigidbody(&aa, aa.meshes[0]);
	DirLight someLight(glm::vec3(0.0f, 0.0f, 0.0f));

	std::vector<std::string> skyFaces = {
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	
//	Mesh skybox(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, loadTexture("textures/shadertest.png")/*loadCubemap(skyFaces, "textures/skybox")*/);
	Skybox skybox(loadCubemap(skyFaces, "textures/skybox"));

	gameState = 1;
	Luarium::log("test", 0);

	// render loop
	// -----------
	while (gameState) {
		if(glfwWindowShouldClose(window))
			gameState = 0;
		

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Camera::ACTIVE->Aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;

		// don't forget to enable shader before setting uniforms
		Shader::ACTIVE->use();
		Shader::ACTIVE->set("ViewPos", Camera::ACTIVE->Position);
		updateLights(*Shader::ACTIVE);
		

		// view/projection transformations
		Shader::ACTIVE->set("projection", Camera::ACTIVE->projection);
		Shader::ACTIVE->set("view", Camera::ACTIVE->view);

	//	floor.Draw(&Shader::ACTIVE);
		aa.Draw(*Shader::ACTIVE);
		waah.Draw(*Shader::ACTIVE);
		// draw skybox last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		Camera::ACTIVE->view = glm::mat4(glm::mat3(Camera::ACTIVE->GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.set("view", Camera::ACTIVE->view);
		skyboxShader.set("projection", Camera::ACTIVE->projection);
		skybox.Draw(skyboxShader);

		aa.Rotation.x += 2*deltaTime;

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
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Camera::ACTIVE->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Camera::ACTIVE->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Camera::ACTIVE->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Camera::ACTIVE->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Camera::ACTIVE->Position.y+= deltaTime * 2;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		Camera::ACTIVE->Position.y-= deltaTime * 2;
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		simpleConsole();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS){
		Shader::ACTIVE->build();
		printf("[?] DEBUG: Reloaded core shader\n");
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		Camera::ACTIVE->ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Camera::ACTIVE->ProcessMouseScroll(yoffset);
}

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
	std::cout << "ree" <<std::endl;
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

std::vector<std::string> GetFirstNMessages(GLuint numMsgs)
{
	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<std::string> messages;
	messages.reserve(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for(size_t msg = 0; msg < lengths.size(); ++msg)
	{
		messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
		currPos = currPos + lengths[msg];
	}
	return messages;
}

void simpleConsole() {
	std::string in;
	std::cout << ">";
	std::cin >> in;
	std::vector<std::string> command = Luarium::segment(in, ' ');
	if (command[0] == "rlshader"){
		glUseProgram(0);
		Shader::ACTIVE->build();
		printf("[?] DEBUG: Reloaded core shader\n");
	}
	if(command[0] == "quit")
		gameState = 0;
	if(command[0] == "log"){
		std::vector<std::string> m;
		m = GetFirstNMessages(100);
		for (int i=0; i <=100; i++)
			std::cout<<m[i] <<std::endl;
	}
	if(command[0] == "rllevel"){} 
}

