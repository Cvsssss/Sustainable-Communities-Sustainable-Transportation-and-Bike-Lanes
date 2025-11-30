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
#include "Texture.h"
#include "modelAnim.h"

// Prototipos de funciones
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Dimensiones
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Cámara
Camera camera(glm::vec3(0.0f, 5.0f, 20.0f)); // Cámara más lejos y arriba para ver mejor
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Window dimensions

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-50.0f, 1.0f, -280.0f);
// CAMBIALO A:
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
float range = 0.0f;
float rot = 0.0f;
float animDesplazamiento = 0.0f;
float animVelocidad = 2.0f;
// Variables para la animación de patrulla
glm::vec3 animPos = glm::vec3(PosIni.x, PosIni.y, PosIni.z); // Posición inicial
float animRot = 0.0f;   // Rotación actual del modelo (grados)
int estadoPatrulla = 0; // 0:Adelante, 1:Derecha, 2:Atrás, 3:Izquierda
float recorrido = 0.0f; // Contador de cuánto ha caminado en el tramo actual
float distPatrulla = 50.0f; // Distancia antes de dar la vuelta

float timeValue = (float)glfwGetTime(); // Ejemplo de cómo obtener el tiempo

// Variables para la bici
glm::vec3 PosBici = glm::vec3(0.0f, 0.0f, -30.0f); // Posición inicial
float rotRueda = 0.0f;
float velocidadBici = 8.0f;
float anguloBici = 0.0f;      
float anguloObjetivo = 0.0f;
float distRecorrida = 0.0f;   
bool estaGirando = false;



bool animBusActiva = false;
int estadoBus = 0;

glm::vec3 busPos(30.0f, 0.0f, 20.0f);
float busAngulo = 270.0f;
float correccionVisual = 90.0f;

float busVelocidad = 10.0f;      
float busVelGiro = 40.0f;      
float busDistanciaRecorrida = 0.0f;
float busAnguloAcumulado = 0.0f;
float largoCuadra = 60.0f;
// Tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Luces
glm::vec3 pointLightPositions[] = {
	// Fila Trasera (Z negativa)
	glm::vec3(-95.0f, -2.0f, -95.0f), // Cuadra 1
	glm::vec3(0.0f, -2.0f, -95.0f), // Cuadra 2
	glm::vec3(95.0f, -2.0f, -95.0f), // Cuadra 3

	// Fila Central (Z cerca de 0)
	glm::vec3(-95.0f, -2.0f,    0.0f), // Cuadra 4
	glm::vec3(0.0f, -2.0f,    0.0f), // Cuadra 5 (CENTRO - Luz ambiental alta sobre el parque)
	glm::vec3(95.0f, -2.0f,    0.0f), // Cuadra 6

	// Fila Delantera (Z positiva)
	glm::vec3(-95.0f, -2.0f,  95.0f), // Cuadra 7
	glm::vec3(0.0f, -2.0f,  95.0f), // Cuadra 8
	glm::vec3(95.0f, -2.0f,  95.0f)  // Cuadra 9
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

	
	//Modelo de animación
	
	ModelAnim animacionPersonaje("Animaciones/Personaje2/Walking.fbx");
	animacionPersonaje.initShaders(animShader.Program);
	/*
	ModelAnim animacionPersonaje2("Animaciones/Personaje3/Sentado.fbx");
	animacionPersonaje2.initShaders(animShader.Program);
	*/
	
	/*
	ModelAnim animacionPersonaje3("Animaciones/Personaje4/Pedaleando.fbx");
	animacionPersonaje3.initShaders(animShader.Program);
	*/


	//Model Cuadra((char*)"Models/Cuadra/Cuadra.obj");
	Model pisoCuadra((char*)"Models/Cuadra/pisoCuadra.obj");
	Model banquetaCuadra((char*)"Models/Cuadra/banquetaCuadra.obj");
	//Model jardineraCuadra((char*)"Models/Cuadra/jardineraCuadra.obj");
	Model crucesCuadra((char*)"Models/Cuadra/crucesCuadra.obj");
	//Model extrasCuadra((char*)"Models/Cuadra/extrasCuadra.obj");
	
	//Model Cuadra1((char*)"Models/Cuadra/Cuadra1.obj");
	//Model Cuadra2((char*)"Models/Cuadra/Cuadra2.obj");
	//Model Cuadra3((char*)"Models/Cuadra/Cuadra3.obj");
	//Model Cuadra4((char*)"Models/Cuadra/Cuadra4.obj");
	
	Model Cuadra5((char*)"Models/Cuadra/Cuadra5.obj");
	
	//Model Cuadra6((char*)"Models/Cuadra/Cuadra6.obj");
	//Model Cuadra7((char*)"Models/Cuadra/Cuadra7.obj");
	//Model Cuadra8((char*)"Models/Cuadra/Cuadra8.obj");
	//Model Cuadra9((char*)"Models/Cuadra/Cuadra9.obj");
	
	//Model Bus((char*)"Models/Bus/Bus.obj");
	//Model LlantasBus((char*)"Models/Bus/BusLlantas.obj");
	
	
	/*
	//BICICLETA
	Model llantasbici((char*)"Models/BICI/llantasbici.obj");
	Model cadena((char*)"Models/BICI/cadena.obj");
	Model cuerpo((char*)"Models/BICI/cuerpo.obj");

	

	//perro
	Model DogBody((char*)"Models/perro/DogBody.obj");
	Model DogHead((char*)"Models/perro/DogHead.obj");
	Model DogLeftLegB((char*)"Models/perro/DogLeftLegB.obj");
	Model DogLeftLegF((char*)"Models/perro/DogLeftLegF.obj");
	Model DogRightLegB((char*)"Models/perro/DogRightLegB.obj");
	Model DogRightLegF((char*)"Models/perro/DogRightLegF.obj");
	Model DogTail((char*)"Models/perro/DogTail.obj");


	*/
	
	
	
	


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

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime(); deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;
		glfwPollEvents(); DoMovement();

		animDesplazamiento += animVelocidad * deltaTime;
		float velocidadReal = animVelocidad * deltaTime;

		switch (estadoPatrulla)
		{
		case 0: // Caminar en Z positivo
			animPos.z += velocidadReal;
			animRot = 0.0f; // Mirando hacia +Z
			break;
		case 1: // Caminar en X positivo
			animPos.x += velocidadReal;
			animRot = 90.0f; // Girar 90 grados
			break;
		case 2: // Caminar en Z negativo
			animPos.z -= velocidadReal;
			animRot = 180.0f; // Mirando hacia atrás
			break;
		case 3: // Caminar en X negativo
			animPos.x -= velocidadReal;
			animRot = 270.0f; // Mirando hacia izquierda
			break;
		}

		// Aumentar el contador de distancia recorrida
		recorrido += velocidadReal;

		// ¿Llegamos a la esquina?
		if (recorrido > distPatrulla)
		{
			recorrido = 0.0f; // Reiniciar contador para el siguiente tramo
			estadoPatrulla++; // Cambiar al siguiente estado/dirección

			// Si pasamos del estado 3, volvemos al 0 (bucle infinito)
			if (estadoPatrulla > 3)
				estadoPatrulla = 0;
		}

		if (animBusActiva) {
			float avance = busVelocidad * deltaTime;


			switch (estadoBus) {

			case 0:
				busPos.x -= avance;
				busDistanciaRecorrida += avance;
				busAngulo = 270.0f;

				if (busDistanciaRecorrida >= largoCuadra) {
					busDistanciaRecorrida = 0.0f;
					estadoBus = 1;
				}
				break;


			case 1:
			{   
				float giro1 = 55.0f * deltaTime;

				busPos.x += avance * sin(glm::radians(busAngulo));
				busPos.z += avance * cos(glm::radians(busAngulo));

				busAngulo -= giro1;
				busAnguloAcumulado += giro1;

				if (busAnguloAcumulado >= 90.0f) {
					busAngulo = 180.0f;
					busAnguloAcumulado = 0.0f;
					estadoBus = 2;
				}
			}
			break;

			case 2:
				busPos.z -= avance;
				busDistanciaRecorrida += avance;
				busAngulo = 180.0f;

				if (busDistanciaRecorrida >= largoCuadra) {
					busDistanciaRecorrida = 0.0f;
					estadoBus = 3;
				}
				break;

			case 3:
			{
				float giro2 = 25.0f * deltaTime; 

				busPos.x += avance * sin(glm::radians(busAngulo));
				busPos.z += avance * cos(glm::radians(busAngulo));

				busAngulo -= giro2;
				busAnguloAcumulado += giro2;

				if (busAnguloAcumulado >= 90.0f) {
					busAngulo = 90.0f;
					busAnguloAcumulado = 0.0f;
					estadoBus = 4;
				}
			}
			break;

			case 4:
				busPos.x += avance;
				busDistanciaRecorrida += avance;
				busAngulo = 90.0f;

				if (busDistanciaRecorrida >= largoCuadra) {
					busDistanciaRecorrida = 0.0f;
					estadoBus = 5;
				}
				break;


			case 5:
			{

				float giro3 = 95.0f * deltaTime;

				busPos.x += avance * sin(glm::radians(busAngulo));
				busPos.z += avance * cos(glm::radians(busAngulo));

				busAngulo -= giro3;
				busAnguloAcumulado += giro3;

				if (busAnguloAcumulado >= 90.0f) {
					busAngulo = 0.0f;
					busAnguloAcumulado = 0.0f;
					estadoBus = 6;
				}
			}
			break;

			case 6:
				busPos.z += avance;
				busDistanciaRecorrida += avance;
				busAngulo = 0.0f;

				if (busDistanciaRecorrida >= largoCuadra) {
					busDistanciaRecorrida = 0.0f;
					estadoBus = 7;
				}
				break;

			case 7:
			{
				float giro4 = 65.0f * deltaTime; 

				busPos.x += avance * sin(glm::radians(busAngulo));
				busPos.z += avance * cos(glm::radians(busAngulo));

				busAngulo -= giro4;
				busAnguloAcumulado += giro4;

				if (busAnguloAcumulado >= 90.0f) {
					busAngulo = 270.0f;
					busAnguloAcumulado = 0.0f;
					estadoBus = 0; 
				}
			}
			break;
			}
		}

		// Animacion de la bici

		float velocidadGiro = 45.0f;
		float avance = velocidadBici * deltaTime;
		float giro = velocidadGiro * deltaTime;

		if (!estaGirando) {
			PosBici.x += avance * sin(glm::radians(anguloBici));
			PosBici.z += avance * cos(glm::radians(anguloBici));
			distRecorrida += avance;
			rotRueda -= avance; 
			// Aqui cambiar el 50 por el valor de distancia al que quieran que gire la bici 
			if (distRecorrida >= 30.0f) {
				distRecorrida = 0.0f;      
				estaGirando = true;        
				anguloObjetivo += 90.0f; 
			}
		}

		else {
			anguloBici += giro;
			PosBici.x += avance * sin(glm::radians(anguloBici));
			PosBici.z += avance * cos(glm::radians(anguloBici));
			rotRueda -= avance;

			if (anguloBici >= anguloObjetivo) {
				anguloBici = anguloObjetivo;
				estaGirando = false;
			}
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

		for (int i = 0; i < 9; i++) {
			std::string num = std::to_string(i);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].position").c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].ambient").c_str()), 0.15f, 0.15f, 0.15f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].diffuse").c_str()), 0.15f, 0.15f, 0.15f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].specular").c_str()), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].linear").c_str()), 0.022f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + num + "].quadratic").c_str()), 0.0019f);
		}

		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// RENDERIZADO DE MODELOS 

		glm::mat4 model(1.0f);
		glm::mat4 modelAux(1.0f);
		float spacing = 0.0f;
		float currentX = 0.0f;


		//Carga de Modelos

		/*
		
		/*
		//    -------- BICICLETA ------	
		model = glm::mat4(1.0f);
		model = glm::translate(model, PosBici);
		model = glm::rotate(model, glm::radians(anguloBici), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelAux = model;
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cuerpo.Draw(lightingShader);

		model = modelAux;
		model = glm::translate(model, glm::vec3(0.0f, 1.7f, 3.16f));
		model = glm::rotate(model, rotRueda, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		llantasbici.Draw(lightingShader);

		model = modelAux;
		model = glm::translate(model, glm::vec3(0.0f, 1.7f, -1.9f));
		model = glm::rotate(model, rotRueda, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		llantasbici.Draw(lightingShader);

		model = modelAux;
		model = glm::translate(model, glm::vec3(0.07f, 1.55f, 0.9f));
		model = glm::rotate(model, rotRueda, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cadena.Draw(lightingShader);
		*/
		/*

		//      -------------------



		// --- Bus ---

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Bus.Draw(lightingShader);
		currentX += spacing;
		// --- llantas ---


		 //------------ PERRO ------------
		/*
		glm::vec3 dogPos = glm::vec3(5.0f, 0.0f, 5.0f); 
		float dogRotAngle = 0.0f;      
		float movDogHead = 0.0f;        
		float movDogTail = 0.0f;        
		// Variables para el Movimiento Constante
		float timeValue = (float)glfwGetTime(); 
		float forwardSpeed = 0.5f;              
		float animationSpeed = 5.0f;            
		float legMaxAngle = 20.0f;              
		float tailMaxAngle = 15.0f;             
		float forwardMovement = forwardSpeed * timeValue; 
		float legMovement = legMaxAngle * sin(animationSpeed * timeValue);
		float tailMovement = tailMaxAngle * sin(8.0f * timeValue);
		glm::mat4 modelTemp; 

		// BODY (CUERPO) - Matriz Base  
		model = glm::mat4(1.0f);
		dogPos.z = dogPos.z + forwardMovement;
		model = glm::translate(model, dogPos);
		model = glm::rotate(model, glm::radians(dogRotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		modelTemp = model;
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		// 2. HEAD (CABEZA)  
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(0.0f, 0.093f, 0.208f)); // Traslación Local de tu modelo
		modelTemp = glm::rotate(modelTemp, glm::radians(movDogHead), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogHead.Draw(lightingShader);

		// 3. TAIL (COLA) 
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(0.0f, 0.026f, -0.288f)); 
		modelTemp = glm::rotate(modelTemp, glm::radians(movDogTail), glm::vec3(0.0f, 0.0f, -1.0f));
		modelTemp = glm::rotate(modelTemp, glm::radians(tailMovement), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogTail.Draw(lightingShader);

		//  4. FRONT LEFT LEG  
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(0.112f, -0.044f, 0.074f));
		modelTemp = glm::rotate(modelTemp, glm::radians(legMovement), glm::vec3(1.0f, 0.0f, 0.0f)); // Movimiento positivo
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogLeftLegF.Draw(lightingShader);

		//  5. FRONT RIGHT LEG 
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(-0.111f, -0.055f, 0.074f));
		modelTemp = glm::rotate(modelTemp, glm::radians(-legMovement), glm::vec3(1.0f, 0.0f, 0.0f)); // Movimiento negativo/contrafase
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogRightLegF.Draw(lightingShader);

		//  6. BACK LEFT LEG (PATA TRASERA IZQUIERDA) - Contrafase  
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(0.082f, -0.046f, -0.218f));
		modelTemp = glm::rotate(modelTemp, glm::radians(-legMovement), glm::vec3(1.0f, 0.0f, 0.0f)); // Movimiento negativo/contrafase
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogLeftLegB.Draw(lightingShader);

		//  7. BACK RIGHT LEG (PATA TRASERA DERECHA) - Caminata  
		modelTemp = model;
		modelTemp = glm::translate(modelTemp, glm::vec3(-0.083f, -0.057f, -0.231f));
		modelTemp = glm::rotate(modelTemp, glm::radians(legMovement), glm::vec3(1.0f, 0.0f, 0.0f)); // Movimiento positivo
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTemp));
		DogRightLegB.Draw(lightingShader);
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
		// --- Turbina ---
		/*
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentX, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Turbina.Draw(lightingShader);
		currentX += spacing;
		*/

	
		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		pisoCuadra.Draw(lightingShader);
		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		banquetaCuadra.Draw(lightingShader);

		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//jardineraCuadra.Draw(lightingShader);
		

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		crucesCuadra.Draw(lightingShader);

		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra1.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra2.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra3.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra4.Draw(lightingShader);

		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Cuadra5.Draw(lightingShader);
		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra6.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra7.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra8.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Cuadra9.Draw(lightingShader);
		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::translate(model, glm::vec3(-3.91f, 0.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//extrasCuadra.Draw(lightingShader);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, busPos);
		model = glm::rotate(model, glm::radians(busAngulo + correccionVisual), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//Bus.Draw(lightingShader);
		//LlantasBus.Draw(lightingShader);

		
		glBindVertexArray(0);
		// Dibujo Luces
		lampShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		for (int i = 0; i < 9; i++) {
			model = glm::mat4(1.0f); model = glm::translate(model, pointLightPositions[i]); model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		glBindVertexArray(0);



		//_______________________________Personaje Animado___________________________
		animShader.Use();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		modelLoc = glGetUniformLocation(animShader.Program, "model");
		viewLoc = glGetUniformLocation(animShader.Program, "view");
		projLoc = glGetUniformLocation(animShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), 0.0f, -1.0f, -1.0f);

	
		model = glm::mat4(1);
		model = glm::translate(model, animPos); // <--- Volvemos a usar animPos
		model = glm::rotate(model, glm::radians(animRot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f)); // O la escala que te funcionó en la prueba (ej. 0.1f)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		animacionPersonaje.Draw(animShader);

		/*
		animShader.Use();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.304f, -45.075f, -8.294f));
		model = glm::rotate(model, glm::radians(-9.398f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-1.011f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(2.267f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		animacionPersonaje2.Draw(animShader);
		*/

		/*
		// Animacion de humano en la bici 
		model = modelAux;
		model = glm::translate(model, glm::vec3(0.0f, 6.0f,1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		animacionPersonaje3.Draw(animShader);
		glBindVertexArray(0);
		*/


		// Dibujo Skybox
		glDepthFunc(GL_LEQUAL); skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36); glBindVertexArray(0); glDepthFunc(GL_LESS);


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

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		animBusActiva = !animBusActiva;
	}
}
void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) { lastX = xPos; lastY = yPos; firstMouse = false; }
	camera.ProcessMouseMovement(xPos - lastX, lastY - yPos); lastX = xPos; lastY = yPos;
}
