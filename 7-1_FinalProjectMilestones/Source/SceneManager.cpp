///////////////////////////////////////////////////////////////////////////////
// SceneManager.cpp
//
// Implements scene setup, texture/material management, lighting configuration,
// and rendering for the CS-330 OpenGL scene framework.
//
// CS-330 Computational Graphics and Visualization
// Southern New Hampshire University
// CS330Content2026 Framework
// Release: 2026 C4
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/
#include <GLFW/glfw3.h> // Include GLFW to access window state and input functions

// Global variables added for modern camera navigation
namespace
{
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// Camera vectors and initial values
	glm::vec3 g_CameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
	glm::vec3 g_CameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
	glm::vec3 g_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float g_Yaw = -90.0f;
	float g_Pitch = 0.0f;

	// Mouse tracking variables
	double g_LastX = 400.0;
	double g_LastY = 300.0;
	bool   g_FirstMouse = true;

	// Delta time tracking
	float g_DeltaTime = 0.0f;
	float g_LastFrame = 0.0f;
	float g_CameraSpeed = 5.0f;

	// Flag to keep track of callback hook initialization
	bool g_bCallbacksInitialized = false;

	// Projection mode toggle (true = Perspective/3D, false = Orthographic/2D)
	bool g_bUsePerspective = true;
}

void SceneManager::DefineObjectMaterials()
{
	m_objectMaterials.clear();

	OBJECT_MATERIAL wallMaterial;
	wallMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	wallMaterial.specularColor = glm::vec3(2.5f, 2.5f, 2.5f); // High specular intensity for glint
	wallMaterial.shininess = 32.0f;                             // Broad specular highlight
	wallMaterial.tag = "cement";                                // Match tag used in render call
	m_objectMaterials.push_back(wallMaterial);

	OBJECT_MATERIAL metalMaterial;
	metalMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);  // Slightly higher diffuse so ambient light brightens it
	metalMaterial.specularColor = glm::vec3(3.5f, 3.5f, 3.5f); // Strong specular reflections for glints
	metalMaterial.shininess = 64.0f;                            // Sleek metallic highlight
	metalMaterial.tag = "metal";
	m_objectMaterials.push_back(metalMaterial);

	OBJECT_MATERIAL woodMaterial; // Not a very shiny material, so low specular and shininess values
	woodMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	woodMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.2f);
	woodMaterial.shininess = 8.0f;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);
}

void SceneManager::SetupSceneLights()
{
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Disable Directional Light
	m_pShaderManager->setBoolValue("directionalLight.bActive", false);

	// Overall lit scene fill (Point Light 0)
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);
	m_pShaderManager->setVec3Value("pointLights[0].position", 0.0f, 12.0f, 6.0f);
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.30f, 0.30f, 0.30f);
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 0.90f, 0.90f, 0.90f);
	m_pShaderManager->setVec3Value("pointLights[0].specular", 0.40f, 0.40f, 0.40f);

	// Disable remaining point lights
	for (int i = 1; i < 5; i++)
	{
		std::string base = "pointLights[" + std::to_string(i) + "].bActive";
		m_pShaderManager->setBoolValue(base.c_str(), false);
	}

	// Lamp spotlight angled at wall & grazing vase
	m_pShaderManager->setBoolValue("spotLight.bActive", true);

	glm::vec3 lampPos = glm::vec3(4.0f, 8.0f, -3.0f);
	glm::vec3 wallTarget = glm::vec3(-0.8f, 0.5f, -8.0f);
	glm::vec3 spotDir = glm::normalize(wallTarget - lampPos);

	m_pShaderManager->setVec3Value("spotLight.position", lampPos);
	m_pShaderManager->setVec3Value("spotLight.direction", spotDir);

	// Focused Wall Light Cone Settings 
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.15f, 0.15f, 0.12f);
	m_pShaderManager->setVec3Value("spotLight.diffuse", 1.85f, 1.65f, 1.30f);  // Bright warm glow on wall
	m_pShaderManager->setVec3Value("spotLight.specular", 1.50f, 1.50f, 1.20f);

	// Low attenuation so the beam maintains strength all the way to the back wall
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 0.012f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.001f);

	// Focused beam angles for a distinct spot halo
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(32.0f)));      // Widened inner beam
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(58.0f))); // Soft, broad spill across table & vase
}

/***********************************************************
 * ScrollCallback()
 *
 * Connected to GLFW scroll wheel events to dynamically
 * adjust g_CameraSpeed up or down.
 ***********************************************************/
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_CameraSpeed += static_cast<float>(yoffset);

	if (g_CameraSpeed < 1.0f)
		g_CameraSpeed = 1.0f;
	if (g_CameraSpeed > 50.0f)
		g_CameraSpeed = 50.0f;

	std::cout << "Current Camera Speed: " << g_CameraSpeed << std::endl;
}

/***********************************************************
 * InternalInputHandler()
 ***********************************************************/
static void InternalInputHandler(GLFWwindow* window)
{
	if (!window) return;

	if (!g_bCallbacksInitialized)
	{
		glfwSetScrollCallback(window, ScrollCallback);
		g_bCallbacksInitialized = true;
	}

	float currentFrame = static_cast<float>(glfwGetTime());
	g_DeltaTime = currentFrame - g_LastFrame;
	g_LastFrame = currentFrame;

	float velocity = g_CameraSpeed * g_DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		g_CameraPos += velocity * g_CameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		g_CameraPos -= velocity * g_CameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		g_CameraPos -= glm::normalize(glm::cross(g_CameraFront, g_CameraUp)) * velocity;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		g_CameraPos += glm::normalize(glm::cross(g_CameraFront, g_CameraUp)) * velocity;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		g_CameraPos -= velocity * g_CameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		g_CameraPos += velocity * g_CameraUp;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		g_bUsePerspective = true;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		g_bUsePerspective = false;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (g_FirstMouse)
	{
		g_LastX = xpos;
		g_LastY = ypos;
		g_FirstMouse = false;
	}

	float xoffset = static_cast<float>(xpos - g_LastX);
	float yoffset = static_cast<float>(g_LastY - ypos);

	g_LastX = xpos;
	g_LastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	g_Yaw += xoffset;
	g_Pitch += yoffset;

	if (g_Pitch > 89.0f)  g_Pitch = 89.0f;
	if (g_Pitch < -89.0f) g_Pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(g_Yaw)) * cos(glm::radians(g_Pitch));
	front.y = sin(glm::radians(g_Pitch));
	front.z = sin(glm::radians(g_Yaw)) * cos(glm::radians(g_Pitch));
	g_CameraFront = glm::normalize(front);
}


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	DefineObjectMaterials();
	SetupSceneLights();

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();             // Table top
	m_basicMeshes->LoadSphereMesh();          // Vase base and flowers
	m_basicMeshes->LoadTaperedCylinderMesh(); // Tapered neck/body of vase and flowers
	m_basicMeshes->LoadCylinderMesh();        // Lamp pole
	m_basicMeshes->LoadConeMesh(); // Parts of plants 

	CreateGLTexture("textures/wood table.jpeg", "table_wood");
	CreateGLTexture("textures/vase body.jpeg", "vase_body");
	CreateGLTexture("textures/lamp.jpeg", "lamp");
	CreateGLTexture("textures/coaster.png", "coaster");
	CreateGLTexture("textures/red tulip.jpeg", "red flower");
	CreateGLTexture("textures/white tulip.jpeg", "white flower");

	BindGLTextures();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	GLFWwindow* currentWindow = glfwGetCurrentContext();
	InternalInputHandler(currentWindow);

	SetupSceneLights(); // Reapply lighting each frame in case of camera movement
	BindGLTextures(); // Rebind textures each frame to ensure correct texture mapping

	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	glm::mat4 projection;
	glm::mat4 view;
	float aspect = 800.0f / 600.0f;

	if (g_bUsePerspective) // Perspective projection for 3D navigation
	{
		projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		view = glm::lookAt(g_CameraPos, g_CameraPos + g_CameraFront, g_CameraUp);
	}
	else // Orthographic projection for 2D-like view
	{
		float orthoSize = 10.0f;
		projection = glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, 0.1f, 100.0f);

		glm::vec3 orthoCamPos = glm::vec3(0.0f, 3.0f, 15.0f);
		glm::vec3 orthoTarget = glm::vec3(0.0f, 3.0f, 0.0f);
		view = glm::lookAt(orthoCamPos, orthoTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		m_pShaderManager->setMat4Value(g_ViewName, view);
		m_pShaderManager->setVec3Value("viewPosition", g_CameraPos);
	}

	/***********************************************************
	 * Ground Plane
	 ***********************************************************/
	scaleXYZ = glm::vec3(30.0f, 1.0f, 30.0f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.412f, 0.412f, 0.412f, 1.0f);
	SetShaderMaterial("cement");
	m_basicMeshes->DrawPlaneMesh();

	/***********************************************************
	* Wall Plane
	/***********************************************************/
	scaleXYZ = glm::vec3(30.0f, 1.0f, 30.0f);
	positionXYZ = glm::vec3(0.0f, 10.0f, -15.0f);

	SetTransformations(scaleXYZ, 90.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.275f, 0.510f, 0.706f, 1.0f); // blue wall
	SetShaderMaterial("cement");
	m_basicMeshes->DrawPlaneMesh();

	/***********************************************************
	 * Table (box mesh)
	***********************************************************/
	float tableThickness = 0.6f;
	scaleXYZ = glm::vec3(6.3f, tableThickness, 8.0f);
	positionXYZ = glm::vec3(0.0f, tableThickness / 2.0f, 0.0f);

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("table_wood");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawBoxMesh();

	/***********************************************************
	* Coaster (box mesh)
	***********************************************************/
	float coasterThickness = 0.20f;
	scaleXYZ = glm::vec3(1.3f, coasterThickness, 1.3f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(1.8f, tableThickness + (coasterThickness / 2.0f), 2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("coaster"); // A lighter color wood texture
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("wood");

	m_basicMeshes->DrawBoxMesh();

	/***********************************************************
	* Vase (half-sphere + tapered cylinder)
	***********************************************************/
	float tableSurfaceY = tableThickness;
	float vaseX = -1.5f;
	float vaseZ = 2.5f;

	float hemisphereRadius = 1.1f;
	float curvedBottomHeight = 0.4f; // Shallow base height
	float neckHeight = 2.6f;

	// Shallow Curved Bottom (Half-Sphere)
	scaleXYZ = glm::vec3(hemisphereRadius, curvedBottomHeight, hemisphereRadius);
	positionXYZ = glm::vec3(vaseX, tableSurfaceY + curvedBottomHeight, vaseZ);

	SetTransformations(
		scaleXYZ,
		180.0f, // Flipped so the curve rests on the table
		0.0f,
		0.0f,
		positionXYZ);

	SetShaderTexture("vase_body");
	SetTextureUVScale(6.0f, 1.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawHalfSphereMesh();

	// Main Body / Neck (Tapered Cylinder)
	scaleXYZ = glm::vec3(hemisphereRadius, neckHeight, hemisphereRadius);
	positionXYZ = glm::vec3(vaseX, tableSurfaceY + curvedBottomHeight - 0.05f, vaseZ);

	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		positionXYZ);

	SetShaderTexture("vase_body");
	SetTextureUVScale(6.0f, 3.5f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawTaperedCylinderMesh();

	/***********************************************************
	 * Plant leaves (cone mesh)
	 ***********************************************************/
	struct LeavePlacement {
		float heightVariation;
		float xOffset;
		float zOffset;
		float xRot;
		float yRot;
		float zRot;
	};

	// Leaves positioned neatly inside the rim, leaning gently outward
	LeavePlacement leaves[] =
	{
		// --- Short Front Leaves (Specifically placed in the white circled spots) ---

		{ 0.15f,   -0.09f,  0.07f,   38.0f,  -28.0f,  -22.0f }, 
		{ 0.12f,    0.09f,  0.07f,   42.0f,   32.0f,   28.0f }, 
		// --- Front Accent Leaves ---
		{ 0.40f,   -0.08f,  0.05f,   18.0f,  -25.0f,  -18.0f }, // Front Left
		{ 0.40f,    0.08f,  0.05f,   18.0f,   25.0f,   18.0f }, // Front Right

		// --- Side Leaves & Gap Fillers ---
		{ 0.45f,   -0.12f,  0.00f,   10.0f,  -50.0f,  -30.0f }, // Side Left Primary
		{ 0.45f,    0.12f,  0.00f,   10.0f,   50.0f,   30.0f }, // Side Right Primary
		{ 0.50f,   -0.15f,  0.02f,   15.0f,  -60.0f,  -35.0f }, // New Left Side Outer Filler
		{ 0.60f,   -0.10f, -0.01f,   12.0f,  -35.0f,  -22.0f }, // New Left Mid Gap Filler
		{ 0.50f,    0.15f,  0.02f,   15.0f,   60.0f,   35.0f }, // New Right Side Outer Filler
		{ 0.60f,    0.10f, -0.01f,   12.0f,   35.0f,   22.0f }, // New Right Mid Gap Filler

		// --- Inner Tall Support Leaves ---
		{ 0.70f,   -0.04f, -0.02f,    8.0f,  -15.0f,   -8.0f }, // Back Inner Left
		{ 0.70f,    0.04f, -0.02f,    8.0f,   15.0f,    8.0f }, // Back Inner Right
		{ 0.65f,    0.00f, -0.05f,   -8.0f,    0.0f,    0.0f }, // Back Center
		{ 0.55f,   -0.06f,  0.02f,   12.0f,  -20.0f,  -12.0f }, // Mid Left Back Gap
		{ 0.55f,    0.06f,  0.02f,   12.0f,   20.0f,   12.0f }, // Mid Right Back Gap
		{ 0.35f,    0.00f,  0.06f,   22.0f,    0.0f,    0.0f }  // Low Front Center Base
		};

		float leavesBaseHeight = 1.4f;
		float vaseRimY = tableThickness + hemisphereRadius + neckHeight;
		// Spawn origin sunk deep enough into the neck so leaf bases are hidden
		float leafSpawnY = vaseRimY - 0.6f;

		for (size_t i = 0; i < 13; i++)
		{
			// Slender scale so leaves look like natural tulip foliage
			scaleXYZ = glm::vec3(0.22f, leavesBaseHeight + leaves[i].heightVariation, 0.06f);

			XrotationDegrees = leaves[i].xRot;
			YrotationDegrees = leaves[i].yRot;
			ZrotationDegrees = leaves[i].zRot;

			positionXYZ = glm::vec3(
				vaseX + leaves[i].xOffset,
				leafSpawnY,
				vaseZ + leaves[i].zOffset
			);

			SetTransformations(
				scaleXYZ,
				XrotationDegrees,
				YrotationDegrees,
				ZrotationDegrees,
				positionXYZ);

			SetShaderColor(0.15f, 0.35f, 0.15f, 1.0f);
			SetShaderMaterial("wood");
			m_basicMeshes->DrawConeMesh();
		}

	/***********************************************************
	 * Flowers (half-sphere, cone, and tapered cylinder)
	 ***********************************************************/
	struct FlowerPlacement { float xOffset; float zOffset; float xRot; float yRot; float zRot; bool isWhite; float stemLen; };

	FlowerPlacement flowers[] = {
		// --- Red Foreground & Lower Layer ---
	
		{ -0.10f,  0.06f, 40.0f, -30.0f, -28.0f, false, 1.6f }, // Low Front Left Red
		{  0.10f,  0.06f, 40.0f,  30.0f,  28.0f, false, 1.6f }, // Low Front Right Red
		{ -0.14f,  0.02f, 25.0f, -50.0f, -42.0f, false, 1.8f }, // Low Side Left Red
		{  0.14f,  0.02f, 25.0f,  50.0f,  42.0f, false, 1.8f }, // Low Side Right Red
		{  0.00f,  0.02f, 20.0f,   0.0f,   0.0f, false, 2.0f }, // Mid Center Red
		{ -0.06f, -0.02f, 12.0f, -20.0f, -15.0f, false, 2.1f }, // Mid Left Red
		{  0.06f, -0.02f, 12.0f,  20.0f,  15.0f, false, 2.1f }, // Mid Right Red

		// --- White Tulips (Widely Fanned Outer Arc & Top Fill) ---
		{ -0.18f,  0.00f, 15.0f, -65.0f, -55.0f, true,  2.1f }, // Far Left White (Wide Fan)
		{ -0.13f, -0.03f, 10.0f, -45.0f, -38.0f, true,  2.3f }, // Mid-Outer Left White
		{ -0.08f, -0.05f,  5.0f, -25.0f, -20.0f, true,  2.5f }, // Inner Left White
		{  0.00f, -0.06f,  0.0f,   0.0f,   0.0f, true,  2.6f }, // High Center White
		{  0.08f, -0.05f,  5.0f,  25.0f,  20.0f, true,  2.5f }, // Inner Right White
		{  0.13f, -0.03f, 10.0f,  45.0f,  38.0f, true,  2.3f }, // Mid-Outer Right White
		{  0.18f,  0.00f, 15.0f,  65.0f,  55.0f, true,  2.1f }, // Far Right White (Wide Fan)

		// --- Back Tier Fillers ---
		{ -0.06f, -0.08f, -10.0f, -30.0f, -22.0f, true, 2.4f }, // Back Left Filler
		{  0.06f, -0.08f, -10.0f,  30.0f,  22.0f, true, 2.4f }, // Back Right Filler
		{  0.00f, -0.09f, -15.0f,   0.0f,   0.0f, true, 2.5f }  // Back Center Filler
		
		};

		float vaseHeight = hemisphereRadius + neckHeight;
		float stemBaseY = tableThickness + vaseHeight - 1.2f;

		glm::vec3 vasePosition = glm::vec3(vaseX, tableSurfaceY + hemisphereRadius, vaseZ);

		for (size_t i = 0; i < 18; i++)
		{
			float currentStemLen = flowers[i].stemLen;

			glm::vec3 stemBasePos = glm::vec3(
				vasePosition.x + flowers[i].xOffset,
				stemBaseY,
				vasePosition.z + flowers[i].zOffset);

			// Thinner stems
			scaleXYZ = glm::vec3(0.035f, currentStemLen, 0.035f);
			XrotationDegrees = flowers[i].xRot;
			YrotationDegrees = flowers[i].yRot;
			ZrotationDegrees = flowers[i].zRot;
			positionXYZ = stemBasePos;

			SetTransformations(
				scaleXYZ,
				XrotationDegrees,
				YrotationDegrees,
				ZrotationDegrees,
				positionXYZ);

			SetShaderColor(0.22f, 0.42f, 0.22f, 1.0f);
			SetShaderMaterial("wood");
			m_basicMeshes->DrawCylinderMesh();

			// Calculate flower head attachment
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, glm::radians(flowers[i].zRot), glm::vec3(0, 0, 1));
			rot = glm::rotate(rot, glm::radians(flowers[i].yRot), glm::vec3(0, 1, 0));
			rot = glm::rotate(rot, glm::radians(flowers[i].xRot), glm::vec3(1, 0, 0));

			glm::vec3 stemDir = glm::normalize(glm::vec3(rot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
			glm::vec3 coneTip = stemBasePos + stemDir * currentStemLen;

			// Reduced flower radius offset for tighter attachment
			float flowerRadius = 0.22f;
			glm::vec3 flowerHeadPos = coneTip - stemDir * flowerRadius;

			// --- Reduced Flower Head Size (scaled down from 0.32f to 0.23f) ---
			scaleXYZ = glm::vec3(0.23f, 0.23f, 0.23f);
			XrotationDegrees = 180.0f + flowers[i].xRot;
			YrotationDegrees = flowers[i].yRot;
			ZrotationDegrees = flowers[i].zRot;
			positionXYZ = flowerHeadPos;

			SetTransformations(
				scaleXYZ,
				XrotationDegrees,
				YrotationDegrees,
				ZrotationDegrees,
				positionXYZ);

			if (flowers[i].isWhite) {
				SetShaderTexture("white flower");
				SetTextureUVScale(1.0f, 1.0f);
				SetShaderMaterial("wood");
			}
			else {
				SetShaderTexture("red flower");
				SetTextureUVScale(1.0f, 1.0f);
				SetShaderMaterial("wood");
			}

			m_basicMeshes->DrawHalfSphereMesh();

			// Flower head buds 
			scaleXYZ = glm::vec3(0.23f, 0.38f, 0.23f);
			XrotationDegrees = flowers[i].xRot;
			YrotationDegrees = flowers[i].yRot;
			ZrotationDegrees = flowers[i].zRot;
			positionXYZ = flowerHeadPos;

			SetTransformations(
				scaleXYZ,
				XrotationDegrees,
				YrotationDegrees,
				ZrotationDegrees,
				positionXYZ);

			if (flowers[i].isWhite) {
				SetShaderTexture("white flower");
				SetTextureUVScale(1.0f, 1.0f);
				SetShaderMaterial("wood");
			}
			else {
				SetShaderTexture("red flower");
				SetTextureUVScale(1.0f, 1.0f);
				SetShaderMaterial("wood");
			}

			m_basicMeshes->DrawTaperedCylinderMesh();
		}

	/***********************************************************
	* Lamp (cylinder mesh + half-spheres)
	***********************************************************/
	float lampPoleHeight = 8.0f;
	scaleXYZ = glm::vec3(0.2f, lampPoleHeight, 0.2f);
	glm::vec3 lampPolePosition = glm::vec3(4.0f, 0.0f, -3.0f);

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, lampPolePosition);
	SetShaderTexture("lamp");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawCylinderMesh();

	// Lower Decorator
	scaleXYZ = glm::vec3(0.7f, 0.7f, 0.7f);
	positionXYZ = glm::vec3(lampPolePosition.x, lampPolePosition.y + lampPoleHeight, lampPolePosition.z);

	SetTransformations(scaleXYZ, 180.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("lamp");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawHalfSphereMesh();

	// Upper Decorator
	scaleXYZ = glm::vec3(0.9f, 1.25f, 0.9f);

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("lamp");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawHalfSphereMesh();
}
