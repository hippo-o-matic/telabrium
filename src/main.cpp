#include "luarium/main.h"

Camera* Camera::ACTIVE = NULL;
Shader* Shader::ACTIVE = NULL;

// settings 
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 450;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//LuaFile test("lua/test.lua");

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
		Luarium::log("Failed to create GLFW window", 4);
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
		Luarium::log("Failed to initialize GLAD", 4);
		return -1;
	}

	#ifdef LUARIUM_MODE_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) //Only enable debug output if our version of GL supports it
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
		glDebugMessageCallback(debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	
	#endif


	//debug settings
	//------------------------------
	std::cout<<glGetString(GL_VERSION)<<std::endl;

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	std::cout<<test.get<std::string>("teststring")<<std::endl;

	// build and compile shaders
	Shader::ACTIVE = new Shader(SHADER_PATH_V, SHADER_PATH_F);
	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

	Camera::ACTIVE = new Camera(glm::vec3(0,0,0));
	Camera::ACTIVE->Aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;


	// load models
	Model aa("model/s/nanosuit.obj", glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0.2,0.2,0.2));
//	Model waah("model/waa.dae", glm::vec3(0,-1,-4));
	// std::string heck = "textures";
//	Mesh bab(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, loadTexture("shadertest.png", heck));

	DirLight someLight(glm::vec3(0.0f, 0.0f, 0.0f));

	std::vector<std::string> skyFaces = {
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	
//	Skybox skybox(loadCubemap(skyFaces, "textures/skybox"));

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


		// don't forget to enable shader before setting uniforms
		Shader::ACTIVE->use();
		Shader::ACTIVE->set("ViewPos", Camera::ACTIVE->Position);
		updateLights(*Shader::ACTIVE);

		// view/projection transformations
		Shader::ACTIVE->set("projection", Camera::ACTIVE->GetProjectionMatrix(SCR_WIDTH / SCR_HEIGHT));
		Shader::ACTIVE->set("view", Camera::ACTIVE->GetViewMatrix());

	//	floor.Draw(&Shader::ACTIVE);
		aa.Draw(*Shader::ACTIVE);
//		waah.Draw(*Shader::ACTIVE);
		// draw skybox last
//		bab.Draw(*Shader::ACTIVE);
//		skybox.Draw(skyboxShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	cleanup();
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
    // useless error codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::string output;
	short int sev;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             output += "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   output += "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: output += "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     output += "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     output += "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           output += "Other"; break;
    } output += " ";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               output += "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: output += "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  output += "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         output += "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         output += "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              output += "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          output += "Push Group"; 
        case GL_DEBUG_TYPE_POP_GROUP:           output += "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               output += "Other"; break;
	}
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         sev = 4; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       sev = 3; break;
        case GL_DEBUG_SEVERITY_LOW:          sev = 2; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: sev = 1; break;
    }

	Luarium::log("OpenGL Error: " + output , sev);
}

void cleanup() {
	delete Shader::ACTIVE;
	delete Camera::ACTIVE;
	//Map::cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
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
	if(command[0] == "rllevel"){} 
}