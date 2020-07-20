#include "telabrium/demo.h"

Level* lvltest;

// settings 
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
	std::cout<<"Telabrium b0.0.1"<<std::endl;

	GLFWwindow* window = init_main_window();

	if(window == nullptr)
		return -1;
	
	std::cout<<glGetString(GL_VERSION)<<std::endl;

	#ifdef TELABRIUM_MODE_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) // Only enable debug output if our version of GL supports it
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
	glFrontFace(GL_CW);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	// Shader::ACTIVE = std::make_unique<Shader>(SHADER_PATH_V, SHADER_PATH_F);
	Shader skyboxShader("skybox", "shaders/skybox.vert", "shaders/skybox.frag");

	FreeCam testCam(deltaTime);
	// TODO: put these in the constructors
	testCam.display_height = &SCR_HEIGHT;
	testCam.display_width = &SCR_WIDTH;

	// load models
	// DirLight aiee(glm::vec3(0), glm::vec3(10,0,0), glm::vec3(0,100,0));
	// std::unique_ptr<Model> testt = std::make_unique<Model>("models/rat.obj", glm::vec3(0,1,10), glm::vec3(30));

    lvltest = new Level("levels/test/index.json");
	lvltest->reload();

	Model vest("models/origin.obj", glm::vec3(0,0,0));
	Object::ptr jest = std::make_unique<Model>("models/origin.obj", glm::vec3(0,5,0));

	vest += jest;

	// std::unique_ptr<Model> vestptr = std::make_unique<Model>(vest);
	// test.add<Model>(vestptr);

	Input baz;
	bool buzz = true;

	baz.addBind("shader", [&buzz](){
		if(buzz) {
			glUseProgram(0);
			// Shader::ACTIVE->build();
			printf("[?] DEBUG: But nothing happened\n");
			buzz = false;
		}
	}, GLFW_KEY_LEFT_BRACKET);
	baz.addBind("shader_stop", [&buzz](){
		buzz = true;
	}, GLFW_KEY_PERIOD, GLFW_RELEASE);

	baz.addBind("test", [&vest](){
		vest.rotate(glm::vec3(deltaTime*20,0,0));
	}, GLFW_KEY_PERIOD);
	baz.addBind("test2", [&vest](){
		vest.rotate(glm::vec3(0,deltaTime*20,0));
	}, GLFW_KEY_COMMA);
	baz.addBind("test3", [&vest](){
		vest.rotate(glm::vec3(0,0,deltaTime*20));
	}, GLFW_KEY_SLASH);

	baz.addBind("console", [window](){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		simpleConsole();
	}, GLFW_KEY_GRAVE_ACCENT);
	baz.activate();

    testCam.position = glm::vec3(0,2,0);

	// std::string heck = "textures";
	// Mesh bab(calcVertex(Telabrium::cubeVerts), Telabrium::cubeIndices, loadTexture("shadertest.png", heck));

	// DirLight someLight(glm::vec3(70.0f, 30.0f, 20.0f), glm::vec3(0), glm::vec3(0,.8,0));
	// PointLight ee(glm::vec3(4,0,0));

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

		Input::processActive(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Telabrium::render();

		// don't forget to enable shader before setting uniforms
		// Shader::ACTIVE->use();
		// Shader::ACTIVE->set("ViewPos", Camera::ACTIVE->getWorldPos());

		// // view/projection transformations
		// Shader::ACTIVE->set("projection", Camera::ACTIVE->GetProjectionMatrix());
		// Shader::ACTIVE->set("view", Camera::ACTIVE->GetViewMatrix());
		
		// Light::updateLights(*Shader::ACTIVE);
		
		// Model::drawT.exec(*Shader::ACTIVE);

		// draw skybox last

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
		TelabriumLog("Failed to create GLFW window", 4);
		std::cin.get();
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	// Set glfw callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	Input::assignCallbacks(window);

	// glad: load all OpenGL function pointers

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		TelabriumLog("Failed to initialize GLAD", 4);
		return nullptr;
	}
	
	return window;
}

// executed when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    // useless error codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::string output = "OpenGL Error: ";
	// short sev = 0;

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
    
    // switch (severity)
    // {
    //     case GL_DEBUG_SEVERITY_HIGH:         sev = 4; break;
    //     case GL_DEBUG_SEVERITY_MEDIUM:       sev = 3; break;
    //     case GL_DEBUG_SEVERITY_LOW:          sev = 2; break;
    //     case GL_DEBUG_SEVERITY_NOTIFICATION: sev = 1; break;
    // }

	TelabriumLog(output, 2);
	glGetError();
}

void cleanup() {
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

	std::vector<std::string> command = Telabrium::segment(in, ' '); // Split the input string into a vector of strings

	if (command[0] == "rlshader"){
		glUseProgram(0);
		// Shader::ACTIVE->build();
		printf("[?] DEBUG: Nothing happened\n");
	} else if(command[0] == "quit"){
		gameState = 0;
	} else if(command[0] == "pm"){
		if(command[1] == "GL_FILL")
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(command[1] == "GL_LINE")
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if(command[1] == "GL_POINT")
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		return;
	} else if(command[0] == "aspect") {
		// Camera::ACTIVE->Aspect = std::stof(command[1]);
	} else if(command[0] == "load") {
			lvltest->loadFile("levels/test/index.json");
	} else if(command[0] == "unload") {
			lvltest->unload();
	} else {
		printf("[?] DEBUG: Command not recognized :/\n");
	}
}