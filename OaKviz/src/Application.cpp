// dear imgui: standalone example application for GLUT/FreeGLUT + OpenGL2, using legacy fixed pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

// !!! GLUT/FreeGLUT IS OBSOLETE SOFTWARE. Using GLUT is not recommended unless you really miss the 90's. !!!
// !!! If someone or something is teaching you GLUT in 2020, you are being abused. Please show some resistance. !!!
// !!! Nowadays, prefer using GLFW or SDL instead!

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include "FileBrowser/Dirent/dirent.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "OBJLoader/OBJ_Loader.h"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include "Application.h"



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

//void mouse_callback_func(int button, int state, int x, int y);
// Our state
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static ImVec4 clear_color2 = ImVec4(0.35f, 0.45f, 0.60f, 1.00f);
objl::Loader Loader;
GLfloat ambient_light[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat diffuse_light[] = { 0.4, 0.4, 0.4, 1.0 };
//GLfloat colors[] = { 0.2, 0.2, 0.2, 0.35, 0.35, 0.35,0.5, 0.5, 0.5, 0.65, 0.65, 0.65,0.8, 0.8, 0.8, 0.95, 0.95, 0.95 };
GLfloat colors[] = { 1.0, 0.5, 0, 0.5, 0, 0,0.4, 0, 0, 0.2, 0, 0,.3, 0, 0, .7, 0, 0 };
GLfloat light_position[] = { 0.5,0.5, 1.2 , 1.0};
objl::Vector3 mRotate(0.0, 0.0, 0.0);
objl::Vector3 mTranslate(0.0, 0.0, 0.0);
float angleM = 0;
std::vector<objl::Loader*> loadedObjs;
static float xCoord = 0.0f;
static float yCoord = 0.0f;
static float zCoord = 0.0f;



// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

int main(int argc, char** argv)
{
    //const char* pathName = "box_stack.obj";
    
    //loadObj(pathName);
    // Create GLUT windows
    init_window(argc, argv);
    init_other();
    glutDisplayFunc(glut_display_func);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutDisplayFunc(glut_display_func);
    //glutKeyboardFunc(glut_keyboard_func);
    //glutMouseFunc(mouse_callback_func);
    //glRotatef(30, 1, 1.0, 0.0);
    

    // Setup GLUT display function
    // We will also call ImGui_ImplGLUT_InstallFuncs() to get all the other functions installed for us,
    // otherwise it is possible to install our own functions and call the imgui_impl_glut.h functions ourselves.
    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    glutMainLoop();

    

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    //Heap Cleanup
    for(int i = 0; i < loadedObjs.size(); i++) {
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
    glutInitWindowSize(1280, 720);
    glutCreateWindow("OakViz");
}

void normalizeCoordinates() {
    //Normalize Vertices
    
}

void init_other() {

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glShadeModel(GL_SMOOTH);
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();
    glEnable(GL_RESCALE_NORMAL);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (16.0/9.0), 0.1, 150);
    //gluLookAt(0, 0.0 ,5, 0, 0, 0, 0, 1, 0);
    //glFrustum(-5.0, 5.0, -5.0, 5.0, 0.1, 5.0);

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
       angleM+=15;

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
   else if(c == 'l') {
       mTranslate.X -= 0.2;
   }
   else if(c == 'r') {
       mTranslate.X += 0.2;
   }
   else if(c == 'u') {
       mTranslate.Y += 0.2;
   }
   else if(c == 'd') {
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

void my_display_code()
{
    int selectedIndex = 0;
    ImGui::Begin("New Window!");
    static int selected = 0;
    for (int n = 0; n < loadedObjs.size(); n++)
    {
        char buf[32];
        sprintf(buf, "Object %d", n);
        if (ImGui::Selectable(loadedObjs[n]->objectName.c_str(),selected == n)) {
            selected = n;
            selectedIndex = n;
        }
            
    }
    //if(selectedIndex != 0)
   
    if(!loadedObjs.empty()) {
        ImGui::Text("Rotation:");
       // std::string xRot = "Rotation X";
       // //xRot.append(std::to_string(selected));
       // std::string yRot = "Rotation Y";
       //// yRot.append(std::to_string(selected));
       // std::string zRot = "Rotation Z";
       // //zRot.append(std::to_string(selected));
             
        ImGui::SliderFloat("Rotation X", &loadedObjs[selected]->rotate.X, -180.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Rotation Y", &loadedObjs[selected]->rotate.Y, -180.0f, 180.0f);
        ImGui::SliderFloat("Rotation Z", &loadedObjs[selected]->rotate.Z, -180.0f, 180.0f);

        ImGui::Text("Scale:");
     
        ImGui::DragFloat("Scale X", &loadedObjs[selected]->scale.X, 0.02f, 0.0f, FLT_MAX, "%.04f");
        ImGui::DragFloat("Scale Y", &loadedObjs[selected]->scale.Y, 0.02f, 0.0f, FLT_MAX, "%.04f");
        ImGui::DragFloat("Scale Z", &loadedObjs[selected]->scale.Z, 0.02f, 0.0f, FLT_MAX, "%.04f");

        ImGui::Text("Translate:");
        //std::string xRot = "X";
        ////xRot.append(std::to_string(selected));
        //std::string yRot = "Y";
        //// yRot.append(std::to_string(selected));
        //std::string zRot = "Z";
        ////zRot.append(std::to_string(selected));
     
        ImGui::SliderFloat("Translate X", &loadedObjs[selected]->translate.X, -180.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Translate Y", &loadedObjs[selected]->translate.Y, -180.0f, 180.0f);
        ImGui::SliderFloat("Translate Z", &loadedObjs[selected]->translate.Z, -180.0f, 180.0f);
    }

    ImGui::End();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);



    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        /*static float x = 0.0f;
        static float y = 0.0f;
        static float z = 0.0f;*/
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::ColorEdit3("disffuse color", (float*)&clear_color2);
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    //ImGui::Begin("aa", NULL);
    showMainMenu();
    //ImGui::End();
    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    
   
    
}

void glut_display_func()
{
    // Start the Dear ImGui frame
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code

    glColor3f(1.0, 0.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    myLookAt();
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
    float clearCol3[] = { 0, 0, 0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, clearCol3);
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
   
    glEnable(GL_MAP2_VERTEX_3);


    int uSize = 4;
    int vSize = 4;
    int gridSize = 20;
  
    glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, uSize, 0, 1, uSize * 3, vSize, grid);
    glEvalMesh2(GL_LINE, 0, gridSize, 0, gridSize);

    glPopMatrix();


    // Grid ends-----

    int k = 0;
    float clearCol[] = {clear_color.x, clear_color.y, clear_color.z};
    float clearCol2[] = { clear_color2.x, clear_color2.y, clear_color2.z};
 
    for (int l = 0; l < loadedObjs.size(); l++){
        glPushMatrix();
        glTranslatef(loadedObjs[l]->translate.X, loadedObjs[l]->translate.Y, loadedObjs[l]->translate.Z);
        //glTranslatef(xCoord, yCoord, zCoord);
        glRotatef(loadedObjs[l]->rotate.X, 1.0, 0.0, 0.0);
        glRotatef(loadedObjs[l]->rotate.Y, 0.0, 1.0, 0.0);
        glRotatef(loadedObjs[l]->rotate.Z, 0.0, 0.0, 1.0);
        glScalef(loadedObjs[l]->scale.X, loadedObjs[l]->scale.Y, loadedObjs[l]->scale.Z);

        for (int i = 0; i < loadedObjs[l]->LoadedMeshes.size(); i++) {
            objl::Mesh currentMesh = loadedObjs[l]->LoadedMeshes[i];

            for (int j = 0, ct = 0; j < currentMesh.Indices.size(); j += 3) {

                if (j % 6 == 0 && j != 0) {
                    ct += 3;
                }

                glBegin(GL_TRIANGLES);
                //glNormal3i(currentMesh.Vertices[i].Normal.X, currentMesh.Vertices[i].Normal.Y, Loader.LoadedVertices[i].Normal.Z);
                //std::cout << currentMesh.Indices[0] << " " << currentMesh.Vertices[i].Position.Y << " " << currentMesh.Vertices[i].Position.Z << std::endl;
                glMaterialfv(GL_FRONT, GL_AMBIENT, clearCol);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, clearCol2);

                glNormal3f(currentMesh.Vertices[currentMesh.Indices[j]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j]].Normal.Z);
                glVertex3f(currentMesh.Vertices[currentMesh.Indices[j]].Position.X, currentMesh.Vertices[currentMesh.Indices[j]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j]].Position.Z);

                glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Normal.Z);
                glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 1]].Position.Z);

                glNormal3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Normal.Z);
                glVertex3f(currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.X, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Y, currentMesh.Vertices[currentMesh.Indices[j + 2]].Position.Z);

                glEnd();


            }

        }
        glPopMatrix();
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    my_display_code();

    // Rendering
    ImGui::Render();
    
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

  
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}


void addToScene(std::vector<objl::Loader*>& loadedObjs, const char* path) {
    std::string tempString(path);

    //int counter = 0;
    //bool objNameFound = true;
    //while (objNameFound) {
    //    for (int i = 0; i < loadedObjs.size(); i++) {

    //        if (!loadedObjs[i]->objectName.compare(path)) {
    //            if (counter != 0) {
    //                tempString.append(std::to_string(counter));
    //                counter++;
    //            }
    //        }
    //        else {
    //            objNameFound = false;
    //        }

    //    }
    //}
    
    // int counter = 0; 
    /*
        for(int . ...) {
            if(load...compare(path)) {
                loadedObjs->counter+=1

            }
        }
    */

  /*  if(counter!=0) {
        tempString.append(std::to_string(counter));
    }*/
    for(int i = 0; i < loadedObjs.size(); i++) {
        if(!loadedObjs[i]->objectName.compare(path)) {
            loadedObjs[i]->counter+=1;
            tempString.append(std::to_string(loadedObjs[i]->counter));
        }
    }

    loadedObjs.push_back(loadObject(path, tempString));
   
}

objl::Loader* loadObject(const char* path, std::string objectName) {
    objl::Loader* loader = new objl::Loader();
    loader->objectName = objectName;
    if(!loader) {
        perror("Unable to Allocate Space in the Heap\n");
        return NULL;
    }
    bool loadout = loader->LoadFile(path);
    if (!loadout) {
        return NULL;
    }
    return loader;
}

void loadObj(const char * path) {
    // Initialize Loader

    // Load .obj File
    bool loadout = Loader.LoadFile(path);

    // Check to see if it loaded

    // If so continue
    if (loadout)
    {
        // Create/Open e1Out.txt
        std::ofstream file("e1Out.txt");

        // Go through each loaded mesh and out its contents
        for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
        {
            // Copy one of the loaded meshes to be our current mesh
            objl::Mesh curMesh = Loader.LoadedMeshes[i];

            // Print Mesh Name
            file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

            // Print Vertices
            file << "Vertices:\n";

            // Go through each vertex and print its number,
            //  position, normal, and texture coordinate
            for (int j = 0; j < curMesh.Vertices.size(); j++)
            {
                file << "V" << j << ": " <<
                    "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                    "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                    "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
            }

            // Print Indices
            file << "Indices:\n";

            // Go through every 3rd index and print the
            //	triangle that these indices represent
            for (int j = 0; j < curMesh.Indices.size(); j += 3)
            {
                file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
            }

            // Print Material
            file << "Material: " << curMesh.MeshMaterial.name << "\n";
            file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
            file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
            file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
            file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
            file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
            file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
            file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
            file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
            file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
            file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
            file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
            file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

            // Leave a space to separate from the next mesh
            file << "\n";
        }

        // Close File
        file.close();
    }
    // If not output an error
    else
    {
        // Create/Open e1Out.txt
        std::ofstream file("e1Out.txt");

        // Output Error
        file << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";

        // Close File
        file.close();
    }
}

