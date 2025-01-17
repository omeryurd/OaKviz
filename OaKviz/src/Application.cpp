#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"
#include "Application.h"
#include "core/Camera.h"
#include "core/shader.h"
#include "core/Light.h"

extern Camera m_camera;
extern bool orthoOn;
extern bool wireFrameOn;

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <algorithm>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif
#define FREEGLUT

#define M_PI 3.14159265358979323846
#define LIGHT_ID 0x4000
#define ATTEN_TYPE 0x1207

//void mouse_callback_func(int button, int state, int x, int y);
// Our state
extern "C" bool loadTexture(std::string path, objl::Loader * &objectModel);
void deleteTexture(objl::Loader*& objectModel);
void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides);
static bool show_demo_window = true;
static bool show_lighting_window = false;
static bool show_material_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static ImVec4 clear_color2 = ImVec4(0.35f, 0.45f, 0.60f, 1.00f);
objl::Loader Loader;
GLfloat global_light[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat ambient_light[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat diffuse_light[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat specular_light[] = { 0.2, 0.2, 0.2, 1.0 };
//GLfloat colors[] = { 0.2, 0.2, 0.2, 0.35, 0.35, 0.35,0.5, 0.5, 0.5, 0.65, 0.65, 0.65,0.8, 0.8, 0.8, 0.95, 0.95, 0.95 };
GLfloat colors[] = { 1.0, 0.5, 0, 0.5, 0, 0,0.4, 0, 0, 0.2, 0, 0,.3, 0, 0, .7, 0, 0 };
GLfloat light_position[] = { 0,0, -10 , 1.0 };
objl::Vector3 mRotate(0.0, 0.0, 0.0);
objl::Vector3 mTranslate(0.0, 0.0, 0.0);
glm::vec4 tempLightDir(1.0f);
float angleM = 0;
std::vector<objl::Loader*> loadedObjs;
static float xCoord = 0.0f;
static float yCoord = 0.0f;
static float zCoord = 0.0f;
GLuint* textureIds;
bool hasTexture = false;
static int selected = 0;
static int selectedLight = -1;
std::vector<Light*> lightVector;

Shader* skyboxShader;
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;
unsigned int loadCubemap(std::vector<std::string> faces);
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


int main(int argc, char** argv)
{
	//const char* pathName = "box_stack.obj";

	// Create GLUT windows
	init_window(argc, argv);

	init_other();

	glutDisplayFunc(glut_display_func);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutDisplayFunc(glut_display_func);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer bindings
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	//Heap Cleanup
	for (int i = 0; i < loadedObjs.size(); i++) {
		delete loadedObjs[i];
	}

	return 0;
}


//Window Initializatioon
void init_window(int argc, char** argv) {
	glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
	glEnable(GL_BLEND);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("OakViz");
	GLenum err = glewInit();
}



void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
	GLint numberOfVertices = numberOfSides + 1;

	GLfloat doublePi = 2.0f * M_PI;

	GLfloat* circleVerticesX;
	GLfloat* circleVerticesY;
	GLfloat* circleVerticesZ;
	circleVerticesX = new GLfloat[numberOfVertices];
	circleVerticesY = new GLfloat[numberOfVertices];
	circleVerticesZ = new GLfloat[numberOfVertices];

	for (int i = 0; i < numberOfVertices; i++)
	{
		circleVerticesX[i] = x + (radius * cos(i * doublePi / numberOfSides));
		circleVerticesY[i] = y + (radius * sin(i * doublePi / numberOfSides));
		circleVerticesZ[i] = z;
	}

	GLfloat* allCircleVertices;
	allCircleVertices = new GLfloat[numberOfVertices * 3];

	for (int i = 0; i < numberOfVertices; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
	glDrawArrays(GL_LINE_STRIP, 0, numberOfVertices);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void init_other() {

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glShadeModel(GL_SMOOTH);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_light);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (16.0 / 9.0), 0.1, 150);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

}

bool loadTexture(std::string path, objl::Loader*& objectModel) {
	int x, y, n;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, STBI_rgb_alpha);
	unsigned int* texId;
	texId = new unsigned int;
	glGenTextures(static_cast<GLsizei>(1), texId);

	if (data != nullptr)
	{
		// Binding of texture name
		glBindTexture(GL_TEXTURE_2D, *texId);
		// redefine standard texture values
		// We will use linear interpolation for magnification filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// We will use linear interpolation for minifying filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// Texture specification
		glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);// Texture specification.
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// we also want to be able to deal with odd texture dimensions
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		stbi_image_free(data);
		//glDeleteTextures()
		objectModel->hasTexture = true;
		objectModel->texId = texId;
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		/* Error occurred */
		printf("Can't open texture file!");
		return false;
	}
}

void deleteTexture(objl::Loader*& objectModel) {
	glDeleteTextures(1, objectModel->texId);
	objectModel->hasTexture = false;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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


void createLight(Light* light) {

	if (PointLight* pointLight = dynamic_cast<PointLight*>(light)) {
		glLightfv(pointLight->getLightID(), GL_AMBIENT, glm::value_ptr(pointLight->getAmbientProperty()));
		glLightfv(pointLight->getLightID(), GL_DIFFUSE, glm::value_ptr(pointLight->getDiffusedProperty()));
		glLightfv(pointLight->getLightID(), GL_SPECULAR, glm::value_ptr(pointLight->getSpecularProperty()));
		glLightfv(pointLight->getLightID(), GL_POSITION, glm::value_ptr(pointLight->getLightPosition()));
		if (pointLight->visible) {
			glEnable(pointLight->getLightID());
		}
		else
			glDisable(pointLight->getLightID());
	}
	else if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(light)) {
		glLightfv(dirLight->getLightID(), GL_AMBIENT, glm::value_ptr(dirLight->getAmbientProperty()));
		glLightfv(dirLight->getLightID(), GL_DIFFUSE, glm::value_ptr(dirLight->getDiffusedProperty()));
		glLightfv(dirLight->getLightID(), GL_SPECULAR, glm::value_ptr(dirLight->getSpecularProperty()));
		glLightfv(dirLight->getLightID(), GL_POSITION, glm::value_ptr(dirLight->getLightPosition()));
		glLightfv(dirLight->getLightID(), GL_SPOT_DIRECTION, glm::value_ptr(dirLight->getLightDirection()));
		if (dirLight->visible) {
			glEnable(dirLight->getLightID());
		}
		else
			glDisable(dirLight->getLightID());
	}
	else if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(light)) {
		glLightfv(posLight->getLightID(), GL_AMBIENT, glm::value_ptr(posLight->getAmbientProperty()));
		glLightfv(posLight->getLightID(), GL_DIFFUSE, glm::value_ptr(posLight->getDiffusedProperty()));
		glLightfv(posLight->getLightID(), GL_SPECULAR, glm::value_ptr(posLight->getSpecularProperty()));
		glLightfv(posLight->getLightID(), GL_POSITION, glm::value_ptr(posLight->getLightPosition()));
		glLightfv(posLight->getLightID(), GL_SPOT_DIRECTION, glm::value_ptr(posLight->getLightDirection()));
		glLightf(posLight->getLightID(), GL_SPOT_CUTOFF, posLight->getCutoff());
		glLightf(posLight->getLightID(), GL_SPOT_EXPONENT, posLight->getExponent());
		glLightf(posLight->getLightID(), posLight->getAttenutationType(), posLight->getattenuationCoeff());
		printf("Atten2:%d\n", posLight->attenuationType);
		if (posLight->visible) {
			glEnable(posLight->getLightID());
		}
		else
			glDisable(posLight->getLightID());
	}


}


void showMainMenu()
{
	static imgui_addons::ImGuiFileBrowser file_dialog;
	bool open = false, save = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Open", NULL))
				open = true;
			if (ImGui::MenuItem("Save", NULL))
				save = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
	if (open)
		ImGui::OpenPopup("Open File");
	if (save)
		ImGui::OpenPopup("Save File");


	/* Optional third parameter. Support opening only compressed rar/zip files.
	 * Opening any other file will show error, return false and won't close the dialog.
	 */
	if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".obj,.zip,.7z"))
	{
		std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
		const char* pathName = file_dialog.selected_fn.c_str();

		addToScene(loadedObjs, pathName);
		//loadedObjs[selected]->hasTexture  = loadTexture("Cottage_Dirt_Base_Color.png", loadedObjs[selected]->texId);

		std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
	}
	if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
	{
		std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
		std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
		std::cout << file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)
		//Do writing of files based on extension here
	}
}

void glut_keyboard_func(unsigned char c, int x, int y) {
	std::cout << "Key pressed!" << std::endl;

	if (c == 'x') {
		mRotate.X = 1.0;
		mRotate.Y = 0.0;
		mRotate.Z = 0.0;
		angleM += 15;

	}
	else if (c == 'y') {
		mRotate.X = 0.0;
		mRotate.Y = 1.0;
		mRotate.Z = 0.0;
		angleM += 15;
	}
	else if (c == 'z') {
		mRotate.X = 0.0;
		mRotate.Y = 0.0;
		mRotate.Z = 1.0;
		angleM += 15;
	}
	else if (c == 'l') {
		mTranslate.X -= 0.2;
	}
	else if (c == 'r') {
		mTranslate.X += 0.2;
	}
	else if (c == 'u') {
		mTranslate.Y += 0.2;
	}
	else if (c == 'd') {
		mTranslate.Y -= 0.2;
	}
	else if (c == 'i') {
		mTranslate.Z += 0.2;
	}
	else if (c == 'o') {
		mTranslate.Z -= 0.2;
	}

	glutPostRedisplay();
}

void deleteObjPopUp(std::vector<objl::Loader*>& objects, unsigned int index) {
	if (ImGui::Button("Delete.."))
		ImGui::OpenPopup("Delete?");

	// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you want to delete the object?\n\n");
		ImGui::Separator();

		static bool dont_ask_me_next_time = false;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			objects.erase(objects.begin() + index);
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

void my_display_code()
{
	static imgui_addons::ImGuiFileBrowser file_dialog2;
	bool tex = false;
	static bool objectSelected = true;
	static float  ligtRotX = 0.0f;
	static float  ligtRotY = 0.0f;
	static float  ligtRotZ = 0.0f;
	static int selected_mesh = 0;
	static float cutoff;

	int selectedIndex = 0;

	ImGui::Begin("OaKviz Editing Panel");
	if (ImGui::CollapsingHeader("Loaded Objects", ImGuiTreeNodeFlags_None)) {
		for (int n = 0; n < loadedObjs.size(); n++)
		{
			char buf[32];
			sprintf(buf, "Object %d", n);
			if (ImGui::Selectable(loadedObjs[n]->objectName.c_str(), selected == n)) {
				selected = n;
				selectedIndex = n;
				selectedLight = -1;
				selected_mesh = 0;
			}

		}


	}

	if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_None)) {
		std::string lightName = "Light-";
		for (int n = 0; n < lightVector.size(); n++)
		{
			lightName.append(std::to_string(n));
			lightName += " (";
			lightName.append(lightVector[n]->typeName);
			lightName += ")";
			char buf[32];
			sprintf(buf, "Object %d", n);
			if (ImGui::Selectable(lightName.c_str(), selectedLight == n)) {
				selectedLight = n;
				selected = -1;
				ligtRotX = 0.0f;
				ligtRotY = 0;
				ligtRotZ = 0;

			}
			lightName = "Light-";
		}

	}
	if (selected >= 0) {
		bool objectExists = true;

		if (!loadedObjs.empty()) {

			ImGui::Text("Rotation:");

			ImGui::SliderFloat("X##rotateX", &loadedObjs[selected]->rotate.X, -180.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Y##rotateY", &loadedObjs[selected]->rotate.Y, -180.0f, 180.0f);
			ImGui::SliderFloat("Z##rotateZ", &loadedObjs[selected]->rotate.Z, -180.0f, 180.0f);

			ImGui::Text("Scale:");

			ImGui::DragFloat("X##scaleX", &loadedObjs[selected]->scale.X, 0.02f, 0.0f, FLT_MAX, "%.04f");
			ImGui::DragFloat("Y##scaleY", &loadedObjs[selected]->scale.Y, 0.02f, 0.0f, FLT_MAX, "%.04f");
			ImGui::DragFloat("Z##scaleY", &loadedObjs[selected]->scale.Z, 0.02f, 0.0f, FLT_MAX, "%.04f");
			ImGui::Text("Translate:");

			ImGui::DragFloat("X##translateX", &loadedObjs[selected]->translate.X, 0.02f, -180.0f, 180.0f, "%.04f");            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::DragFloat("Y##translateY", &loadedObjs[selected]->translate.Y, 0.02f, -180.0f, 180.0f, "%.04f");
			ImGui::DragFloat("Z##translateZ", &loadedObjs[selected]->translate.Z, 0.02f, -180.0f, 180.0f, "%.04f");

			if (!loadedObjs.empty()) {
				// Delete an Object
				if (ImGui::Button("Delete Object.."))
					ImGui::OpenPopup("Delete?");

				// Always center this window when appearing
				ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Are you sure you want to delete the object?\n\n");
					ImGui::Separator();

					static bool dont_ask_me_next_time = false;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
					ImGui::PopStyleVar();

					if (ImGui::Button("OK", ImVec2(120, 0))) {
						ImGui::CloseCurrentPopup();
						if (loadedObjs[selected]->hasTexture)
							deleteTexture(loadedObjs[selected]);
						loadedObjs.erase(loadedObjs.begin() + selected);
						selected = -1;
						objectExists = false;
					}
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
				}



				//------ Delete object ends
				if (objectExists) {
					ImGui::Separator();
					ImGui::Text("Texture: ", loadedObjs[selected]->texId);
					// Load Texture Button
					if (ImGui::Button("Load Texture"))
						tex = true;
					if (tex)
						ImGui::OpenPopup("Load Texture");

					if (file_dialog2.showFileDialog("Load Texture", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png,.jpg"))
					{
						std::cout << file_dialog2.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
						const char* pathName = file_dialog2.selected_fn.c_str();

						loadedObjs[selected]->hasTexture = loadTexture(pathName, loadedObjs[selected]);

						std::cout << file_dialog2.selected_path << std::endl;    // The absolute path to the selected file
					}
					//Delete Texture
					ImGui::SameLine(); if (ImGui::Button("Delete Texture")) {
						if (loadedObjs[selected]->hasTexture) {
							deleteTexture(loadedObjs[selected]);
						}

					}
					// Load Texture Button Ends--------
					if (loadedObjs[selected]->hasTexture) {
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, *loadedObjs[selected]->texId);


						ImGui::Text("Size = %d x %d", 64, 64);
						ImGui::Image((void*)(intptr_t)*loadedObjs[selected]->texId, ImVec2(64, 64));

					}
					else {
						glDisable(GL_TEXTURE_2D);
					}
				}
			}
		}
	}
	else if (selectedLight >= 0) {
		PositionalLight* posLight1 = dynamic_cast<PositionalLight*>(lightVector[selectedLight]);
		DirectionalLight* dirLight1 = dynamic_cast<DirectionalLight*>(lightVector[selectedLight]);
		ImGui::Checkbox("Visible", &lightVector[selectedLight]->visible);
		if (PointLight* pointLight = dynamic_cast<PointLight*>(lightVector[selectedLight])) {
		}
		else {
			ImGui::Text("Rotation:");
			ImGui::SliderFloat("X##rotatelightX", &ligtRotX, -180.0f, 180.0f);
			if (ImGui::IsItemActivated()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(posLight->getLightDirection(), 1.0);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(dirLight->getLightDirection(), 1.0);
				}
			}
			if (ImGui::IsItemActive()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotX), glm::vec3(1.0, 0.0, 0.0));

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-posLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(posLight->getLightPosition()));
					posLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotX), glm::vec3(1.0, 0.0, 0.0));

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-dirLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(dirLight->getLightPosition()));
					dirLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
			}
			else if (ImGui::IsItemDeactivated()) {
				ligtRotX = 0.0f;
			}

			ImGui::SliderFloat("Y##rotatelightY", &ligtRotY, -180.0f, 180.0f);
			if (ImGui::IsItemActivated()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(posLight->getLightDirection(), 1.0);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(dirLight->getLightDirection(), 1.0);
				}
			}
			if (ImGui::IsItemActive()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotY), glm::vec3(0.0, 1.0, 0.0));

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-posLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(posLight->getLightPosition()));
					posLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotZ), glm::vec3(0.0, 1.0, 0.0)); // Z

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-dirLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(dirLight->getLightPosition()));
					dirLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
			}
			else if (ImGui::IsItemDeactivated()) {
				ligtRotY = 0.0f;
			}
			ImGui::SliderFloat("Z##rotatelightZ", &ligtRotZ, -180.0f, 180.0f);
			if (ImGui::IsItemActivated()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(posLight->getLightDirection(), 1.0);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					tempLightDir = glm::vec4(dirLight->getLightDirection(), 1.0);
				}
			}
			if (ImGui::IsItemActive()) {
				if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotZ), glm::vec3(0.0, 0.0, 1.0)); // Y

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-posLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(posLight->getLightPosition()));
					posLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
				if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
					glm::mat4x4 rotMat(1.0f);
					rotMat = glm::rotate(glm::radians(ligtRotZ), glm::vec3(0.0, 0.0, 1.0));

					glm::mat4x4 translateLight = glm::translate(glm::vec3(-dirLight->getLightPosition()));
					glm::mat4x4 translateLightBack = glm::translate(glm::vec3(dirLight->getLightPosition()));
					dirLight->setLightDirection(translateLightBack * rotMat * translateLight * tempLightDir);
				}
			}
			else if (ImGui::IsItemDeactivated()) {
				ligtRotZ = 0.0f;
			}

		}
		if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
			static const char* attenTypes[3] = { "CONSTANT", "LINEAR", "QUADRATIC" };
			int currentAttenType2 = posLight->attenuationType - ATTEN_TYPE;
			ImGui::SliderFloat("Cutoff Angle", &posLight->cutoff, 0.0f, 89.99f);
			ImGui::SliderInt("Exponent", (int*)(&posLight->exponent), 0, 128, "%d");
			ImGui::Text("Positional Light - Attenuation Type");
			if (ImGui::Combo("Type", &currentAttenType2, attenTypes, IM_ARRAYSIZE(attenTypes))) {
				if (currentAttenType2 == 0) {
					posLight->setAttenutationType(ATTEN_TYPE);
				}
				else if (currentAttenType2 == 1) {
					posLight->setAttenutationType(ATTEN_TYPE + 1);
				}
				else if (currentAttenType2 == 2) {
					posLight->setAttenutationType(ATTEN_TYPE + 2);
				}
			}
		}
		ImGui::Text("Position:");

		ImGui::DragFloat("X##lightPosX", &lightVector[selectedLight]->lightPosition.x, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
		ImGui::DragFloat("Y##lightPosY", &lightVector[selectedLight]->lightPosition.y, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
		ImGui::DragFloat("Z##lightPosZ", &lightVector[selectedLight]->lightPosition.z, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
		if (posLight1) {
			ImGui::Text("Direction:");

			ImGui::DragFloat("X##lightDirX", &posLight1->lightDirection.x, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
			ImGui::DragFloat("Y##lightDirY", &posLight1->lightDirection.y, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
			ImGui::DragFloat("Z##lightDirZ", &posLight1->lightDirection.z, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
		}
		if (dirLight1) {
			ImGui::Text("Direction:");

			ImGui::DragFloat("X##lightDirX", &dirLight1->lightDirection.x, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
			ImGui::DragFloat("Y##lightDirY", &dirLight1->lightDirection.y, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
			ImGui::DragFloat("Z##lightDirZ", &dirLight1->lightDirection.z, 0.02f, -FLT_MAX, FLT_MAX, "%.04f");
		}
		ImGui::Text("Light Intensity Properties:");
		ImGui::ColorEdit4("Ambient##ambientP", (float*)&lightVector[selectedLight]->ambientProperty, ImGuiColorEditFlags_Float);
		ImGui::ColorEdit4("Diffuse##diffuseP", (float*)&lightVector[selectedLight]->diffuseProperty, ImGuiColorEditFlags_Float);
		ImGui::ColorEdit4("Specular##specularP", (float*)&lightVector[selectedLight]->specularProperty, ImGuiColorEditFlags_Float);

	}

	ImGui::Separator();
	ImGui::Text("OaK Editing Panel");
	ImGui::Checkbox("Create Light", &show_lighting_window);

	if (show_lighting_window)
	{
		static int radButtonToggled = 0;
		ImGui::Begin("Light Creation Window", &show_lighting_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::RadioButton("Point Light", &radButtonToggled, 0); ImGui::SameLine();
		ImGui::RadioButton("Directional Light", &radButtonToggled, 1); ImGui::SameLine();
		ImGui::RadioButton("Positional Light", &radButtonToggled, 2);

		if (radButtonToggled == 0) {
			static glm::vec4 pointAmbientColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 pointDiffuseColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 pointSpecularColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 pointLightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			ImGui::Text("Point Light - Ambient Property");
			ImGui::ColorEdit4("Color##2f", (float*)&pointAmbientColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Point Light - Diffuse Property");
			ImGui::ColorEdit4("Color##3f", (float*)&pointDiffuseColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Point Light - Specular Property");
			ImGui::ColorEdit4("Color##4f", (float*)&pointSpecularColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Point Light - Position");
			ImGui::DragFloat("Position: X", &pointLightPos.x, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Position: Y", &pointLightPos.y, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Position: Z", &pointLightPos.z, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");

			if (ImGui::Button("Create Point Light")) {

				ImGui::OpenPopup("Maximum Lights Warning!");

				if (lightVector.size() == 8) {

					if (ImGui::BeginPopupModal("Maximum Lights Warning!", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("There are already 8 lights within the scene.\nNo Additional Lights can be added.\n\n");
						ImGui::Separator();

						ImGui::SetItemDefaultFocus();
						if (ImGui::Button("Close Message", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
				}
				else {
					PointLight* newLight = new PointLight(LIGHT_ID + lightVector.size());
					newLight->setAmbientProperty(pointAmbientColor);
					newLight->setDiffuseProperty(pointDiffuseColor);
					newLight->setSpecularProperty(pointSpecularColor);
					newLight->setLightPosition(pointLightPos);
					lightVector.push_back(newLight);
				}
			}

		}
		else if (radButtonToggled == 1) {

			static glm::vec4 directionalAmbientColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 directionalDiffuseColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 directionalSpecularColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec3 directionalLightDir = glm::vec3(0.0f, -10.0f, 0.0f);
			static glm::vec4 directionalLightPos = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);

			ImGui::Text("Directional Light - Ambient Property");
			ImGui::ColorEdit4("Color##5f", (float*)&directionalAmbientColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Directional Light - Diffuse Property");
			ImGui::ColorEdit4("Color##6f", (float*)&directionalDiffuseColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Directional Light - Specular Property");
			ImGui::ColorEdit4("Color##7f", (float*)&directionalSpecularColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Directional Light - Direction:");
			ImGui::DragFloat("X##dirDirX", &directionalLightDir.x, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Y##dirDirY", &directionalLightDir.y, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Z##dirDirZ", &directionalLightDir.z, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");

			ImGui::Text("Directional Light - Position:");
			ImGui::DragFloat("X##dirPosX", &directionalLightPos.x, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Y##dirPosY", &directionalLightPos.y, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Z##dirPosZ", &directionalLightPos.z, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");

			if (ImGui::Button("Create Directional Light")) {

				ImGui::OpenPopup("Maximum Lights Warning!");

				if (lightVector.size() == 8) {

					if (ImGui::BeginPopupModal("Maximum Lights Warning!", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("There are already 8 lights within the scene.\nNo Additional Lights can be added.\n\n");
						ImGui::Separator();

						ImGui::SetItemDefaultFocus();
						if (ImGui::Button("Close Message", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
				}
				else {
					DirectionalLight* newLight = new DirectionalLight(LIGHT_ID + lightVector.size());
					newLight->setAmbientProperty(directionalAmbientColor);
					newLight->setDiffuseProperty(directionalDiffuseColor);
					newLight->setSpecularProperty(directionalSpecularColor);
					newLight->setLightDirection(directionalLightDir);
					newLight->setLightPosition(directionalLightPos);
					lightVector.push_back(newLight);
				}
			}

		}
		else if (radButtonToggled == 2) {

			static glm::vec4 positionalAmbientColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 positionalDiffuseColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 positionalSpecularColor = glm::vec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static glm::vec4 positionalLightDir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			static glm::vec4 positionalLightPos = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
			static float spotCutoff = 45.0f;
			static int spotExponent = 0.0f;
			static float attenCoef = 1.0f;
			static int currentAttenType = 0;
			static const char* attenTypes[3] = { "CONSTANT", "LINEAR", "QUADRATIC" };

			ImGui::Text("Positional Light - Ambient Property");
			ImGui::ColorEdit4("Color##8f", (float*)&positionalAmbientColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Positional Light - Diffuse Property");
			ImGui::ColorEdit4("Color##9f", (float*)&positionalDiffuseColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Positional Light - Specular Property");
			ImGui::ColorEdit4("Color##10f", (float*)&positionalSpecularColor, ImGuiColorEditFlags_Float);

			ImGui::Text("Positional Light - Direction");
			ImGui::DragFloat("Direction - X", &positionalLightDir.x, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Direction - Y", &positionalLightDir.y, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Direction - Z", &positionalLightDir.z, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");

			ImGui::Text("Positional Light - Position");
			ImGui::DragFloat("Position - X", &positionalLightPos.x, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Position - Y", &positionalLightPos.y, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");
			ImGui::DragFloat("Position - Z", &positionalLightPos.z, 0.2f, -FLT_MAX, FLT_MAX, "%.02f");

			ImGui::Text("Positional Light - Spot Exponent");
			ImGui::SliderInt("Exponent", &spotExponent, 0, 128, "%d");

			ImGui::Text("Positional Light - Spot Cutoff Angle");
			ImGui::SliderFloat("Cutoff", &spotCutoff, 0.0f, 89.99f, "%0.2f");

			ImGui::Text("Positional Light - Attenuation Type");
			ImGui::Combo("Type", &currentAttenType, attenTypes, IM_ARRAYSIZE(attenTypes));

			ImGui::Text("Positional Light - Attenuation Coefficient");
			ImGui::SliderFloat("Coefficient", &attenCoef, 1.0f, 60.0f, "%f");

			if (ImGui::Button("Create Positional Light")) {

				ImGui::OpenPopup("Maximum Lights Warning!");

				if (lightVector.size() == 8) {

					if (ImGui::BeginPopupModal("Maximum Lights Warning!", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("There are already 8 lights within the scene.\nNo Additional Lights can be added.\n\n");
						ImGui::Separator();

						ImGui::SetItemDefaultFocus();
						if (ImGui::Button("Close Message", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
				}
				else {
					PositionalLight* newLight = new PositionalLight(LIGHT_ID + lightVector.size());
					newLight->setAmbientProperty(positionalAmbientColor);
					newLight->setDiffuseProperty(positionalDiffuseColor);
					newLight->setSpecularProperty(positionalSpecularColor);
					newLight->setLightDirection(positionalLightDir);
					newLight->setLightPosition(positionalLightPos);
					newLight->setCutoff(spotCutoff);
					newLight->setExponent(spotExponent);
					newLight->setattenuationCoeff(attenCoef);

					if (currentAttenType == 0) {
						newLight->setAttenutationType(ATTEN_TYPE);
					}
					else if (currentAttenType == 1) {
						newLight->setAttenutationType(ATTEN_TYPE + 1);
					}
					else if (currentAttenType == 2) {
						newLight->setAttenutationType(ATTEN_TYPE + 2);
					}

					lightVector.push_back(newLight);
				}

			}

		}

		ImGui::End();
	}

	if (selected >= 0 && !loadedObjs.empty()) {

		ImGui::Checkbox("Edit Material Properties", &show_material_window);

		if (show_material_window)
		{
			ImGui::Begin("Edit Material Properties", &show_material_window);

			std::vector<std::string> meshNames;

			//Creating Mesh Names
			for (int i = 0; i < loadedObjs[selected]->LoadedMeshes.size(); i++) {
				meshNames.push_back(loadedObjs[selected]->LoadedMeshes[i].MeshName);
			}

			ImGui::ListBox("Mesh Names", &selected_mesh, vector_getter, static_cast<void*>(&meshNames), meshNames.size(), 5);

			ImGui::Text("Ambient Property");
			ImGui::ColorEdit3("Color##amb", (float*)&(loadedObjs[selected]->LoadedMeshes[selected_mesh].MeshMaterial.Ka), ImGuiColorEditFlags_Float);

			ImGui::Text("Diffuse Property");
			ImGui::ColorEdit3("Color##diff", (float*)&(loadedObjs[selected]->LoadedMeshes[selected_mesh].MeshMaterial.Kd), ImGuiColorEditFlags_Float);

			ImGui::Text("Specular Property");
			ImGui::ColorEdit3("Color##spec", (float*)&(loadedObjs[selected]->LoadedMeshes[selected_mesh].MeshMaterial.Ks), ImGuiColorEditFlags_Float);

			ImGui::Text("Emmisive Property");
			ImGui::ColorEdit3("Color##emes", (float*)&(loadedObjs[selected]->LoadedMeshes[selected_mesh].MeshMaterial.Ke), ImGuiColorEditFlags_Float);

			ImGui::Text("Shineness Property");
			ImGui::SliderInt("Shineness##shi", (int*)&(loadedObjs[selected]->LoadedMeshes[selected_mesh].MeshMaterial.Ns), 0, 128, "%d");

			ImGui::End();

		}
	}

	ImGui::End();

	showMainMenu();
}

void glut_display_func()
{
	// Start the Dear ImGui frame
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	myLookAt();

	glDisable(GL_LIGHTING);

	//Grid -------
	GLfloat grid2x2[2][2][3] = {
  {{-2.0, -2.0, 0.0}, {4.0, -2.0, 0.0}},
  {{-2.0, 3.0, 0.0}, {3.0, 4.0, 0.0}}
	};
	GLfloat grid4x4[4][4][3] =
	{
	  {
		{-2.0, -2.0, 0.0},
		{-0.5, -2.0, 0.0},
		{0.5, -2.0, 0.0},
		{2.0, -2.0, 0.0}},
	  {
		{-2.0, -0.5, 0.0},
		{-0.5, -0.5, 0.0},
		{0.5, -0.5, 0.0},
		{2.0, -0.5, 0.0}},
	  {
		{-2.0, 0.5, 0.0},
		{-0.5, 0.5, 0.0},
		{0.5, 0.5, 0.0},
		{2.0, 0.5, 0.0}},
	  {
		{-2.0, 2.0, 0.0},
		{-0.5, 2.0, 0.0},
		{0.5, 2.0, 0.0},
		{2.0, 2.0, 0.0}}
	};
	GLfloat* grid = &grid4x4[0][0][0];
	glPushMatrix();
	glRotatef(90, 1, 0, 0);

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_COLOR_4);
	glColor4f(0, 0, 0, 1);
	glEnable(GL_AUTO_NORMAL);

	int uSize = 4;
	int vSize = 4;
	int gridSize = 20;

	glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, uSize, 0, 1, uSize * 3, vSize, grid);
	glEvalMesh2(GL_LINE, 0, gridSize, 0, gridSize);

	glPopMatrix();
	glEnable(GL_LIGHTING);

	// Grid ends-----

	int k = 0;
	float clearCol[] = { clear_color.x, clear_color.y, clear_color.z };
	float clearCol2[] = { clear_color2.x, clear_color2.y, clear_color2.z };

	for (int l = 0; l < loadedObjs.size(); l++) {
		glPushMatrix();
		glTranslatef(loadedObjs[l]->translate.X, loadedObjs[l]->translate.Y, loadedObjs[l]->translate.Z);

		glm::quat MyQuaternion;

		// Direct specification of the 4 components
		// You almost never use this directly
		MyQuaternion = glm::quat(1, 1, 1, 1);

		// Conversion from Euler angles (in radians) to Quaternion
		glm::vec3 EulerAngles(90, 45, 0);
		MyQuaternion = glm::quat(EulerAngles);

		// Conversion from axis-angle
		// In GLM the angle must be in degrees here, so convert it.
		MyQuaternion = glm::angleAxis(glm::radians(loadedObjs[l]->rotate.Y), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 RotationMatrix = glm::toMat4(MyQuaternion);

		float dArray[16] = { 0.0 };

		const float* pSource = (const float*)glm::value_ptr(RotationMatrix);
		for (int i = 0; i < 16; ++i)
			dArray[i] = pSource[i];

		glMultMatrixf(dArray);
		glRotatef(loadedObjs[l]->rotate.X, 1.0, 0.0, 0.0);
		//glRotatef(loadedObjs[l]->rotate.Y, 0.0, 1.0, 0.0);
		glRotatef(loadedObjs[l]->rotate.Z, 0.0, 0.0, 1.0);
		glScalef(loadedObjs[l]->scale.X, loadedObjs[l]->scale.Y, loadedObjs[l]->scale.Z);

		if (loadedObjs[l]->hasTexture) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, *loadedObjs[l]->texId);
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}

		for (int i = 0; i < loadedObjs[l]->LoadedMeshes.size(); i++) {
			objl::Mesh currentMesh = loadedObjs[l]->LoadedMeshes[i];

			if (!wireFrameOn) {

				for (int j = 0, ct = 0; j < currentMesh.Indices.size(); j += 3) {

					if (j % 6 == 0 && j != 0) {
						ct += 3;
					}

					glBegin(GL_TRIANGLES);
					glm::vec3 Ka(currentMesh.MeshMaterial.Ka.X, currentMesh.MeshMaterial.Ka.Y, currentMesh.MeshMaterial.Ka.Z);
					glm::vec3 Kd(currentMesh.MeshMaterial.Kd.X, currentMesh.MeshMaterial.Kd.Y, currentMesh.MeshMaterial.Kd.Z);
					glm::vec3 Ks(currentMesh.MeshMaterial.Ks.X, currentMesh.MeshMaterial.Ks.Y, currentMesh.MeshMaterial.Ks.Z);
					glm::vec3 Ke(currentMesh.MeshMaterial.Ke.X, currentMesh.MeshMaterial.Ke.Y, currentMesh.MeshMaterial.Ke.Z);
					glMaterialfv(GL_FRONT, GL_AMBIENT, glm::value_ptr(Ka));
					glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(Kd));
					glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(Ks));
					glMaterialfv(GL_FRONT, GL_EMISSION, glm::value_ptr(Ke));
					glMateriali(GL_FRONT, GL_SHININESS, currentMesh.MeshMaterial.Ns);
					glTexCoord2f(currentMesh.Vertices[currentMesh.Indices[j]].TextureCoordinate.X, currentMesh.Vertices[currentMesh.Indices[j]].TextureCoordinate.Y);
					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j]].Position.X, currentMesh.Vertices[currentMesh.Indices[j]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j]].Position.Z);
					glTexCoord2f(currentMesh.Vertices[currentMesh.Indices[j + 1]].TextureCoordinate.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].TextureCoordinate.Y);
					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Z);
					glTexCoord2f(currentMesh.Vertices[currentMesh.Indices[j + 2]].TextureCoordinate.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].TextureCoordinate.Y);
					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Z);

					glEnd();


				}

			}
			else {
				glDisable(GL_LIGHTING);
				for (int j = 0, ct = 0; j < currentMesh.Indices.size(); j += 3) {

					if (j % 6 == 0 && j != 0) {
						ct += 3;
					}

					glBegin(GL_LINE_LOOP);

					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j]].Position.X, currentMesh.Vertices[currentMesh.Indices[j]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j]].Position.Z);

					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Z);

					glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Z);
					glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Z);

					glEnd();
				}

			}

		}

		glPopMatrix();
	}

	glEnable(GL_LIGHTING);
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	my_display_code();

	for (int i = 0; i < lightVector.size(); i++) {
		createLight(lightVector[i]);
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);


	if (selectedLight > -1) {
		glPushMatrix();
		glColor4f(0.5, 0.6, 1, 1);
		glPointSize(10);
		if (PositionalLight* posLight = dynamic_cast<PositionalLight*>(lightVector[selectedLight])) {
			glColor4f(0.9, 0.7, 0.1, 1);
			glBegin(GL_POINTS);
			glVertex4fv(glm::value_ptr(glm::vec4(posLight->getLightDirection(), 1.0f)));
			glColor4f(0.9, 0.6, 0.7, 1);
			glVertex4fv(glm::value_ptr(posLight->getLightPosition()));
			glEnd();
			glColor4f(1.0, 0.0, 0.3, 1);
			glBegin(GL_LINES);
			glVertex4fv(glm::value_ptr(glm::vec4(posLight->getLightDirection(), 1.0f)));
			glVertex4fv(glm::value_ptr(posLight->getLightPosition()));
			glEnd();
			glm::vec3 temp = glm::abs(glm::vec3(posLight->getLightPosition()) - posLight->getLightDirection());
			float distance = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
			float radius = glm::tan(glm::radians(posLight->getCutoff())) * distance;
			drawCircle(posLight->getLightDirection().x, posLight->getLightDirection().y, posLight->getLightDirection().z, radius, 10);
		}
		else if (DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(lightVector[selectedLight])) {
			glBegin(GL_LINES);
			glVertex4fv(glm::value_ptr(glm::vec4(dirLight->getLightDirection(), 1.0f)));
			glVertex3fv(glm::value_ptr(glm::vec3(dirLight->getLightPosition())));
			glEnd();
			glColor4f(0.9, 0.7, 0.1, 1);
			glBegin(GL_POINTS);
			glVertex4fv(glm::value_ptr(glm::vec4(dirLight->getLightDirection(), 1.0f)));
			glEnd();
		}
		else if (PointLight* pointLight = dynamic_cast<PointLight*>(lightVector[selectedLight])) {
			glColor4f(0.9, 0.7, 0.1, 1);
			glBegin(GL_POINTS);
			glVertex3fv(glm::value_ptr(pointLight->getLightPosition()));
			glEnd();
			glBegin(GL_LINES);
			glVertex3fv(glm::value_ptr(pointLight->getLightPosition()));
			float x = pointLight->getLightPosition().x;
			float z = pointLight->getLightPosition().z;
			glVertex3f(x, 0, z);
			glEnd();
		}

		glPopMatrix();
	}
	// Rendering
	ImGui::Render();

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	glEnable(GL_LIGHTING);


	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}


void addToScene(std::vector<objl::Loader*>& loadedObjs, const char* path) {
	std::string tempString(path);
	for (int i = 0; i < loadedObjs.size(); i++) {
		if (!loadedObjs[i]->objectName.compare(path)) {
			loadedObjs[i]->counter += 1;
			tempString.append(std::to_string(loadedObjs[i]->counter));
		}
	}

	loadedObjs.push_back(loadObject(path, tempString));

}

objl::Loader* loadObject(const char* path, std::string objectName) {
	objl::Loader* loader = new objl::Loader();
	loader->objectName = objectName;
	if (!loader) {
		perror("Unable to Allocate Space in the Heap\n");
		return NULL;
	}
	bool loadout = loader->LoadFile(path);
	if (!loadout) {
		return NULL;
	}
	return loader;
}