#include "luarium/main.h"

Camera* Camera::ACTIVE = NULL;
Shader* Shader::ACTIVE = NULL;

// settings 
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
	std::cout<<"Luarium b0.0.1"<<std::endl;

	GLFWwindow* window = init_main_window();

	if(window == nullptr)
		return -1;
	
	std::cout<<glGetString(GL_VERSION)<<std::endl;

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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	Shader::ACTIVE = new Shader(SHADER_PATH_V, SHADER_PATH_F);
	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
	Shader unshaded("shaders/Standard.vert", "shaders/unshaded.frag");

	Camera::ACTIVE = new Camera(glm::vec3(0,0,0));
	Camera::ACTIVE->Aspect = 1;
	
	Luarium::Archive::pack("archive/test1", "archive/test1.hpak");

	Luarium::Archive::unpack("archive/test1.hpak", "archive/test2/");

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// load models
	Model aa("model/s/nanosuit.obj");
	Model box("model/cube.obj", glm::vec3(2,1,0));
	std::string heck = "textures";
	Mesh bab(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, loadTexture("shadertest.png", heck));

	DirLight someLight(glm::vec3(70.0f, 0.0f, 20.0f), glm::vec3(0.001));
	SpotLight spot(glm::vec3(0));

	std::vector<std::string> skyFaces = {
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	
	Skybox skybox(loadCubemap(skyFaces, "textures/skybox"));

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

		spot.Position = Camera::ACTIVE->Position;
		spot.Rotation = Camera::ACTIVE->Front;


		// don't forget to enable shader before setting uniforms
		Shader::ACTIVE->use();
		Shader::ACTIVE->set("ViewPos", Camera::ACTIVE->Position);

		// view/projection transformations
		Shader::ACTIVE->set("projection", Camera::ACTIVE->GetProjectionMatrix());
		Shader::ACTIVE->set("view", Camera::ACTIVE->GetViewMatrix());
		unshaded.set("projection", Camera::ACTIVE->GetProjectionMatrix());
		unshaded.set("view", Camera::ACTIVE->GetViewMatrix());
		
		updateLights(*Shader::ACTIVE, &unshaded);

		aa.Draw();
	//	box.Draw();
		
		// draw skybox last
		bab.Draw(*Shader::ACTIVE);
		skybox.Draw(skyboxShader);

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

GLFWwindow* init_main_window() {
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
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Luarium::log("Failed to initialize GLAD", 4);
		return nullptr;
	}
	
	return window;
}

// executed when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

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
	glGetError();
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
	std::getline(std::cin, in);
	std::cin.clear();
	std::cin.sync();

	if (in == "\0") // Do nothing if nothing is input
		return;

	std::vector<std::string> command = Luarium::segment(in, ' '); // Split the input string into a vector of strings

	if (command[0] == "rlshader"){
		glUseProgram(0);
		Shader::ACTIVE->build();
		printf("[?] DEBUG: Reloaded core shader\n");
	}
	else if(command[0] == "quit"){
		gameState = 0;
	}
	else if(command[0] == "pm"){
		if(command[1] == "GL_FILL")
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(command[1] == "GL_LINE")
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if(command[1] == "GL_POINT")
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		return;
	} 

	else {
		printf("[?] DEBUG: Command not recognized :/\n");
	}
}