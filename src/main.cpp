#include "luarium/main.h"

//void GLAPIENTRY glDebugFunc(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam);

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
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(glDebugFunc, 0);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	std::cout<<test.get<std::string>("teststring")<<std::endl;

	// build and compile shaders
	std::shared_ptr<Shader> MAIN_SHADER(new Shader(SHADER_PATH_V, SHADER_PATH_F));
	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");

	// load models
	Model aa("models/red.stl", glm::vec3(0,1,0));
	Model waah("models/waa.dae", glm::vec3(0,0,-2));

//	Rigidbody(&aa, aa.meshes[0]);
	DirLight someLight(glm::vec3(0.0f, 0.0f, 0.0f));

	std::vector<std::string> skyFaces = {
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};

//	Mesh skybox(cubeVerts, cubeIndices, loadCubemap(skyFaces), {});

	gameState = 1;
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

		ACTIVE_CAMERA->Aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;

		// don't forget to enable shader before setting uniforms
		MAIN_SHADER->use();
		MAIN_SHADER->setVec3("ViewPos", ACTIVE_CAMERA->Position);
		updateLights(MAIN_SHADER);
		

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(ACTIVE_CAMERA->fov), ACTIVE_CAMERA->Aspect, 0.1f, 100.0f);
		glm::mat4 view = ACTIVE_CAMERA->GetViewMatrix();
		MAIN_SHADER->setMat4("projection", projection);
		MAIN_SHADER->setMat4("view", view);

	//	floor.Draw(&MAIN_SHADER);
		aa.Draw(MAIN_SHADER);
		waah.Draw(MAIN_SHADER);
		// draw skybox last
//		skybox.Draw(&skyboxShader);

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
		ACTIVE_CAMERA->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		ACTIVE_CAMERA->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ACTIVE_CAMERA->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ACTIVE_CAMERA->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		ACTIVE_CAMERA->Position.y+= deltaTime * 2;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		ACTIVE_CAMERA->Position.y-= deltaTime * 2;
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		simpleConsole();
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
		ACTIVE_CAMERA->ProcessMouseMovement(xoffset, yoffset);
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
	ACTIVE_CAMERA->ProcessMouseScroll(yoffset);
}

/*void GLAPIENTRY glDebugFunc(GLenum source​, GLenum type, GLuint id​, GLenum severity, GLsizei length​, const GLchar* message, const void* userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}*/

void simpleConsole() {
	std::string in;
	std::cout << ">";
	std::cin >> in;
	std::vector<std::string> command = Luarium::segment(in, ' ');
	if (command[0] == "rlshader"){
		glUseProgram(0);
		glDeleteProgram(MAIN_SHADER->ID);
		MAIN_SHADER.reset(new Shader(SHADER_PATH_V, SHADER_PATH_F));
		MAIN_SHADER->use();
		printf("[?] DEBUG: Reloaded core shader\n");
	}
	if(command[0] == "exit"){
		gameState = 0;
	}
	if(command[0] == "rllevel"){} 
}

