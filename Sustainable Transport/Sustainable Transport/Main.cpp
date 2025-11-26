#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "modelAnim.h"

// Prototipos de funciones
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Dimensiones
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Cámara
Camera camera(glm::vec3(0.0f, 5.0f, 20.0f)); 
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
glm::vec3 PosIni(0.0f, 0.0f, -15.0f);
glm::vec3 animPos = glm::vec3(PosIni.x + 3.0f, PosIni.y - 1.0f, PosIni.z);
float animRot = 0.0f;
int estadoPatrulla = 0;
float recorrido = 0.0f;
float distPatrulla = 50.0f;
float animVelocidad = 15.0f;

// Luces
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 5.0f, 5.0f),
	glm::vec3(-10.0f, 5.0f, 5.0f),
	glm::vec3(10.0f, 5.0f, 5.0f),
	glm::vec3(20.0f, 5.0f, 5.0f),
	glm::vec3(30.0f, 5.0f, 5.0f)
};

// Geometría básica
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

float skyboxVertices[] = {
	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
};

unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID); glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error al cargar textura de Cubemap: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}

int main() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PROYECTO FINAL", nullptr, nullptr);
	if (!window) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { std::cout << "Failed to initialize GLEW" << std::endl; return -1; }
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");

	// CARGA DE LA ANIMACIÓN DEL PERSONAJE
	ModelAnim animacionPersonaje("Animaciones/Personaje2/Walking.fbx");
	animacionPersonaje.initShaders(animShader.Program);

	// CARGA DE LOS MODELOS



	//Model Antena_Telecom((char*)"Models/Antena_Telecom/Antena_Telecom.obj");
	//Model arbol((char*)"Models/arbol/arbol.obj");
	//Model arbol2((char*)"Models/arbol2/arbol2.obj");
	//Model arbusto((char*)"Models/arbusto/arbusto.obj");
	//Model bancaMad((char*)"Models/bancaMad/bancaMad.obj");
	//Model bancamadera((char*)"Models/bancamadera/bancamadera.obj");
	//Model Banqueta((char*)"Models/Banqueta/Banqueta.obj");


	//BICICLETA
	/*
	Model llantasbici((char*)"Models/BICI/llantasbici.obj");
	Model cadena((char*)"Models/BICI/cadena.obj");
	Model cuerpo((char*)"Models/BICI/cuerpo.obj");
	*/

	//Model bolardocar((char*)"Models/bolardocar/bolardocar.obj");
	//Model bolardopeq((char*)"Models/bolardopeq/bolardopeq.obj");
	//Model BoteBasura((char*)"Models/BoteBasura/BoteBasura.obj");
	//Model BotedeReciclaje((char*)"Models/BotedeReciclaje/BotedeReciclaje.obj");
	//Model boteVerdeBasura((char*)"Models/boteVerdeBasura/boteVerdeBasura.obj");
	//Model bulardo((char*)"Models/bulardo/bulardo.obj");
	//Model Bus((char*)"Models/Bus/Bus.obj");
	//Model BuzonDeCorreo((char*)"Models/BuzonDeCorreo/BuzonDeCorreo.obj");
	//Model CamaraSeguridad((char*)"Models/CamaraSeguridad/CamaraSeguridad_Velocimetro.obj");
	//Model Ciclovia((char*)"Models/Ciclovia/Ciclovia.obj");
	//Model camino((char*)"Models/camino/camino.obj");
	//Model casco((char*)"Models/casco/casco.obj");
	//Model Cono((char*)"Models/Cono/Cono.obj");
	//Model CuboMoldeParaCasa((char*)"Models/CuboMoldeParaCasa/CuboMoldeParaCasa.obj");
	//Model Edificio((char*)"Models/Edificio/Edificio.obj");
	//Model edificio2((char*)"Models/edificio2/edificio2.obj");
	//Model edificio3((char*)"Models/edificio3/edificio3.obj");
	//Model edificio4((char*)"Models/edificio4/edificio4.obj");
	//Model edificio5((char*)"Models/edificio5/edificio5.obj");
	//Model edificio6((char*)"Models/edificio6/edificio6.obj");
	//Model edificio7((char*)"Models/edificio7/edificio7.obj"); 
	//Model edificio8((char*)"Models/edificio8/edificio8.obj");
	//Model EdificioSinDiseno((char*)"Models/EdificioSinDiseno/EdificioSinDiseno.obj");
	//Model hidrante((char*)"Models/hidrante/hidrante.obj");
	//Model jardinera((char*)"Models/jardinera/jardinera.obj");
	//Model kiosko((char*)"Models/kiosko/kiosko.obj");
	//Model LamparaModerna((char*)"Models/LamparaModerna/LamparaModerna.obj");
	//Model LetreroAutobus((char*)"Models/LetreroAutobus/LetreroAutobus.obj");
	//Model LetreroCalle((char*)"Models/LetreroCalle/LetreroCalle.obj");
	//Model LetreroVelocidad((char*)"Models/LetreroVelocidad/LetreroVelocidad.obj");
	//Model llantas((char*)"Models/llantas/llantas.obj");
	//Model mesa((char*)"Models/mesa/mesa.obj");

	//MDOELOS ANGEL

	//columpio
	/*
	Model cuerpoColumpio((char*)"Models/columpio/cuerpoColumpio.obj");
	Model asientoColumpio((char*)"Models/columpio/asientosColumpio.obj");
	*/

	//Model palapa((char*)"Models/palapa/palapa.obj");
	//Model paradaAutobus((char*)"Models/paradaAutobus/paradaAutobus.obj");
	//Model pasto((char*)"Models/pasto/pasto.obj");

	//perro
	/*
	Model DogBody((char*)"Models/perro/DogBody.obj");
	Model DogHead((char*)"Models/perro/DogHead.obj");
	Model DogLeftLegB((char*)"Models/perro/DogLeftLegB.obj");
	Model DogLeftLegF((char*)"Models/perro/DogLeftLegF.obj");
	Model DogRightLegB((char*)"Models/perro/DogRightLegB.obj");
	Model DogRightLegF((char*)"Models/perro/DogRightLegF.obj");
	Model DogTail((char*)"Models/perro/DogTail.obj");
	*/

	//Model Piedra_Decorativa((char*)"Models/Piedra_Decorativa/Piedra_Decorativa.obj");
	//Model placas((char*)"Models/placas/placas.obj");
	//Model poste((char*)"Models/poste/poste.obj");
	//Model posteAlto((char*)"Models/posteAlto/posteAlto.obj");
	//Model RampaDiscapacitados((char*)"Models/RampaDiscapacitados/RampaDiscapacitados.obj");
	//Model scooter((char*)"Models/scooter/scooter.obj");
	//Model scooterMoto((char*)"Models/scooterMoto/scooterMoto.obj");
	//Model StopLight((char*)"Models/StopLight/StopLight.obj");
	//Model StopSign((char*)"Models/StopSign/StopSign.obj");
	//Model TapaBuzon((char*)"Models/BuzonDeCorreo/TapaBuzon.obj");
	//Model tope((char*)"Models/tope/tope.obj");
	//Model Turbina((char*)"Models/TurbinaEolica/Turbina.obj"); 
	//Model valla((char*)"Models/valla/valla.obj");



	// Buffers
	GLuint VBO, VAO, skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
	glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); glBindVertexArray(0);

	glGenVertexArrays(1, &skyboxVAO); glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO); glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces{
		"Skybox/Daylight Box_Right.tga", "Skybox/Daylight Box_Left.tga",
		"Skybox/Daylight Box_Top.tga", "Skybox/Daylight Box_Bottom.tga",
		"Skybox/Daylight Box_Front.tga", "Skybox/Daylight Box_Back.tga"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.Use();
	glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime(); deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;
		glfwPollEvents(); 
		DoMovement();
		float velocidadReal = animVelocidad * deltaTime;

		//Animacion de persona caminando por la calle
		switch (estadoPatrulla)
		{
		case 0: 
			animPos.z += velocidadReal;
			animRot = 0.0f; 
			break;
		case 1: 
			animPos.x += velocidadReal;
			animRot = 90.0f;
			break;
		case 2: 
			animPos.z -= velocidadReal;
			animRot = 180.0f;
			break;
		case 3:
			animPos.x -= velocidadReal;
			animRot = 270.0f;
			break;
		}

	//GIro
		recorrido += velocidadReal;
		if (recorrido > distPatrulla)
		{
			recorrido = 0.0f; // Reiniciar 
			estadoPatrulla++; 
			if (estadoPatrulla > 3) 
				estadoPatrulla = 0;
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.Use();
		glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.15f, 0.15f, 0.15f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		for (int i = 0; i < 5; i++) {
			std::string num = std::to_string(i);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].position").c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].ambient").c_str()), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].diffuse").c_str()), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].specular").c_str()), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].linear").c_str()), 0.09f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].quadratic").c_str()), 0.032f);
		}

		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// RENDERIZADO DE MODELOS 

		glm::mat4 model(1.0f);
		float spacing = 0.0f;
		float currentX = 0.0f;


		// --- Maqueta ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte1.Draw(lightingShader);
		currentX += spacing;

		// --- Maqueta ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte1.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte2.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte3.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte4.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte5.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte6.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte7.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte8.Draw(lightingShader);
		currentX += spacing;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Parte9.Draw(lightingShader);
		currentX += spacing;
		*/

		/*
		// --- Antena_Telecom ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Antena_Telecom.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Arbol ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		arbol.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Arbol2 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		arbol2.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Arbusto ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		arbusto.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Bancamadera ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bancaMad.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Bancamadera ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bancamadera.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Banqueta ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Banqueta.Draw(lightingShader);
		currentX += spacing;
		*/

		/*
		//    -------- BICCICLETA ------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		llantasbici.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cadena.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cuerpo.Draw(lightingShader);



		//      -------------------


		// --- Bolardocar ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bolardocar.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Bolardopeq ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bolardopeq.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- BoteBasura ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		BoteBasura.Draw(lightingShader);
		currentX += spacing;
		currentX += spacing;
		*/

		// --- BotedeReciclaje ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		BotedeReciclaje.Draw(lightingShader);
		currentX += spacing;
		*/
		// --- boteVerdeBasura ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		boteVerdeBasura.Draw(lightingShader);
		currentX += spacing;
		*/
		// --- Bulardo ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bulardo.Draw(lightingShader);
		currentX += spacing;

		// --- Bus ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Bus.Draw(lightingShader);
		currentX += spacing;

		// --- BuzonDeCorreo ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		BuzonDeCorreo.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- CamaraSeguridad ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		CamaraSeguridad.Draw(lightingShader);
		currentX += spacing;
		*/


		// --- Camino ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		camino.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Casco ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		casco.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Ciclovia ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Ciclovia.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Cono ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Cono.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- CuboMoldeParaCasa ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		CuboMoldeParaCasa.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Edificio.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio2 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio2.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio3 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio3.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio4 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio4.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio5 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio5.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio6 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio6.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio7 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio7.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Edificio8 ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		edificio8.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- EdificioSinDiseno ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		EdificioSinDiseno.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Hidrante ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		hidrante.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Jardinera ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		jardinera.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- kiosko ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		kiosko.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Lampara Moderna ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		LamparaModerna.Draw(lightingShader);
		currentX += spacing;
		*/


		// --- LetreroAutobus ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		LetreroAutobus.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- LetreroCalle ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		LetreroCalle.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- LetreroVelocidad ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		LetreroVelocidad.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- llantas ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		llantas.Draw(lightingShader);
		currentX += spacing;
		*/

		/*

		// --- Mesa ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);
		currentX += spacing;
		*/

		// ---	MODELOS ANGEL ---

		//COLUMPIO
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cuerpoColumpio.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		asientoColumpio.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- palapa ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		palapa.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Parada Autobus ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		paradaAutobus.Draw(lightingShader);
		currentX += spacing;
		*/

		/*
		// --- Pasto ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		pasto.Draw(lightingShader);
		currentX += spacing;
		*/


		// ------------ PERRO ------------
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogHead.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogLeftLegB.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogLeftLegF.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogRightLegB.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogRightLegF.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0F, 0.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogTail.Draw(lightingShader);
		*/

		// --------------------------------
		/*
		// -- Piedra_Decorativa ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Piedra_Decorativa.Draw(lightingShader);
		currentX += spacing;



		// --- Placas ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		placas.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Poste ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		poste.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- PosteAlto ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		posteAlto.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- RampaDiscapacitados ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		RampaDiscapacitados.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- SCOOTER  ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		scooter.Draw(lightingShader);
		currentX += spacing;

		// --- scooterMoto  ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		scooterMoto.Draw(lightingShader);
		currentX += spacing;

		// --- StopLight ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		StopLight.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- StopSign ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		StopSign.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- TapaBuzon ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		TapaBuzon.Draw(lightingShader);
		currentX += spacing;


		// --- Tope ---
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tope.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Turbina ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Turbina.Draw(lightingShader);
		currentX += spacing;
		*/

		// --- Valla ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		valla.Draw(lightingShader);
		*/

		// Dibujo Luces
		lampShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		for (int i = 0; i < 5; i++) {
			model = glm::mat4(1.0f); model = glm::translate(model, pointLightPositions[i]); model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Dibujo Skybox
		glDepthFunc(GL_LEQUAL); skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36); glBindVertexArray(0); glDepthFunc(GL_LESS);

		//AQUI SE CARGAN LOS FBX CON PROPIEDADES
		animShader.Use();


		glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// 2. Configurar Materiales y Luz (Para que no se vea negro)
		glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), 0.0f, -1.0f, -1.0f);

		glm::mat4 modelAnim = glm::mat4(1.0f);
		modelAnim = glm::translate(modelAnim, animPos);
		modelAnim = glm::rotate(modelAnim, glm::radians(animRot), glm::vec3(0.0f, 1.0f, 0.0f));
		modelAnim = glm::scale(modelAnim, glm::vec3(0.02f)); 

		glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelAnim));

		animacionPersonaje.Draw(animShader);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO); glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &skyboxVBO);
	glfwTerminate(); return 0;
}

void DoMovement() {
	if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) { if (action == GLFW_PRESS) keys[key] = true; else if (action == GLFW_RELEASE) keys[key] = false; }
}
void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) { lastX = xPos; lastY = yPos; firstMouse = false; }
	camera.ProcessMouseMovement(xPos - lastX, lastY - yPos); lastX = xPos; lastY = yPos;
}