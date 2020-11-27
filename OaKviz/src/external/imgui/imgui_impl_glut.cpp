// dear imgui: Platform Binding for GLUT/FreeGLUT
// This needs to be used along with a Renderer (e.g. OpenGL2)

// !!! GLUT/FreeGLUT IS OBSOLETE SOFTWARE. Using GLUT is not recommended unless you really miss the 90's. !!!
// !!! If someone or something is teaching you GLUT in 2020, you are being abused. Please show some resistance. !!!
// !!! Nowadays, prefer using GLFW or SDL instead!

// Issues:
//  [ ] Platform: GLUT is unable to distinguish e.g. Backspace from CTRL+H or TAB from CTRL+I
//  [ ] Platform: Missing mouse cursor shape/visibility support.
//  [ ] Platform: Missing clipboard support (not supported by Glut).
//  [ ] Platform: Missing gamepad support.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2019-04-03: Misc: Renamed imgui_impl_freeglut.cpp/.h to imgui_impl_glut.cpp/.h.
//  2019-03-25: Misc: Made io.DeltaTime always above zero.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-03-22: Added GLUT Platform binding.

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "../../src/global_init.h"




#include <stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#endif

static int g_Time = 0;          // Current time, in milliseconds
float diffx = 0;
float diffy = 0;

bool ImGui_ImplGLUT_Init()
{
    ImGuiIO& io = ImGui::GetIO();

#ifdef FREEGLUT
    io.BackendPlatformName = "imgui_impl_glut (freeglut)";
#else
    io.BackendPlatformName = "imgui_impl_glut";
#endif

    g_Time = 0;

    // Glut has 1 function for characters and one for "special keys". We map the characters in the 0..255 range and the keys above.
    io.KeyMap[ImGuiKey_Tab]         = '\t'; // == 9 == CTRL+I
    io.KeyMap[ImGuiKey_LeftArrow]   = 256 + GLUT_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow]  = 256 + GLUT_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]     = 256 + GLUT_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]   = 256 + GLUT_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp]      = 256 + GLUT_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown]    = 256 + GLUT_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home]        = 256 + GLUT_KEY_HOME;
    io.KeyMap[ImGuiKey_End]         = 256 + GLUT_KEY_END;
    io.KeyMap[ImGuiKey_Insert]      = 256 + GLUT_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete]      = 127;
    io.KeyMap[ImGuiKey_Backspace]   = 8;  // == CTRL+H
    io.KeyMap[ImGuiKey_Space]       = ' ';
    io.KeyMap[ImGuiKey_Enter]       = 13; // == CTRL+M
    io.KeyMap[ImGuiKey_Escape]      = 27;
    io.KeyMap[ImGuiKey_KeyPadEnter] = 13; // == CTRL+M
    io.KeyMap[ImGuiKey_A]           = 'A';
    io.KeyMap[ImGuiKey_C]           = 'C';
    io.KeyMap[ImGuiKey_V]           = 'V';
    io.KeyMap[ImGuiKey_X]           = 'X';
    io.KeyMap[ImGuiKey_Y]           = 'Y';
    io.KeyMap[ImGuiKey_Z]           = 'Z';
   

    return true;
}

void ImGui_ImplGLUT_InstallFuncs()
{
    glutReshapeFunc(ImGui_ImplGLUT_ReshapeFunc);
    glutMotionFunc(ImGui_ImplGLUT_MotionFunc);
    glutPassiveMotionFunc(ImGui_ImplGLUT_MotionFunc);
    glutMouseFunc(ImGui_ImplGLUT_MouseFunc);
#ifdef __FREEGLUT_EXT_H__
    glutMouseWheelFunc(ImGui_ImplGLUT_MouseWheelFunc);
#endif
    glutKeyboardFunc(ImGui_ImplGLUT_KeyboardFunc);
    glutKeyboardUpFunc(ImGui_ImplGLUT_KeyboardUpFunc);
    glutSpecialFunc(ImGui_ImplGLUT_SpecialFunc);
    glutSpecialUpFunc(ImGui_ImplGLUT_SpecialUpFunc);
}

void ImGui_ImplGLUT_Shutdown()
{
}

void ImGui_ImplGLUT_NewFrame()
{
    // Setup time step
    ImGuiIO& io = ImGui::GetIO();
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    int delta_time_ms = (current_time - g_Time);
    if (delta_time_ms <= 0)
        delta_time_ms = 1;
    io.DeltaTime = delta_time_ms / 1000.0f;
    g_Time = current_time;
    // Start the frame
    ImGui::NewFrame();
}

static void ImGui_ImplGLUT_UpdateKeyboardMods()
{
    ImGuiIO& io = ImGui::GetIO();
    int mods = glutGetModifiers();
    io.KeyCtrl = (mods & GLUT_ACTIVE_CTRL) != 0;
    io.KeyShift = (mods & GLUT_ACTIVE_SHIFT) != 0;
    io.KeyAlt = (mods & GLUT_ACTIVE_ALT) != 0;
}

void ImGui_ImplGLUT_KeyboardFunc(unsigned char c, int x, int y)
{
    // Send character to imgui
    //printf("char_down_func %d '%c'\n", c, c);
    ImGuiIO& io = ImGui::GetIO();
    if (c >= 32)
        io.AddInputCharacter((unsigned int)c);

    // Store letters in KeysDown[] array as both uppercase and lowercase + Handle GLUT translating CTRL+A..CTRL+Z as 1..26.
    // This is a hacky mess but GLUT is unable to distinguish e.g. a TAB key from CTRL+I so this is probably the best we can do here.
    if (c >= 1 && c <= 26)
        io.KeysDown[c] = io.KeysDown[c - 1 + 'a'] = io.KeysDown[c - 1 + 'A'] = true;
    else if (c >= 'a' && c <= 'z')
        io.KeysDown[c] = io.KeysDown[c - 'a' + 'A'] = true;
    else if (c >= 'A' && c <= 'Z')
        io.KeysDown[c] = io.KeysDown[c - 'A' + 'a'] = true;
    else
        io.KeysDown[c] = true;
    glm::vec3 view = glm::normalize(m_camera.GetViewDir());
    if (c == 'w') {
        m_camera.SetCameraView(m_camera.GetEye() + view, m_camera.GetLookAt(), m_camera.GetUpVector());
    }
    if (c == 's') {
        m_camera.SetCameraView(m_camera.GetEye() - view, m_camera.GetLookAt(), m_camera.GetUpVector());
    }
    if (c == 'r' || c == 'R') {
        m_camera.SetCameraView(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glutPostRedisplay();
    ImGui_ImplGLUT_UpdateKeyboardMods();
    (void)x; (void)y; // Unused
}

void ImGui_ImplGLUT_KeyboardUpFunc(unsigned char c, int x, int y)
{
   
    ImGuiIO& io = ImGui::GetIO();
    if (c >= 1 && c <= 26)
        io.KeysDown[c] = io.KeysDown[c - 1 + 'a'] = io.KeysDown[c - 1 + 'A'] = false;
    else if (c >= 'a' && c <= 'z')
        io.KeysDown[c] = io.KeysDown[c - 'a' + 'A'] = false;
    else if (c >= 'A' && c <= 'Z')
        io.KeysDown[c] = io.KeysDown[c - 'A' + 'a'] = false;
    else
        io.KeysDown[c] = false;
    /*if (c == 'x') {

        glRotatef(15, 1.0, 0, 0);

    }
    else if (c == 'y') {

        glRotatef(15, 0.0, 1.0, 0.0);
    }
    else if (c == 'z') {

        glRotatef(15, 0, 0, 1.0);
    }
    glutPostRedisplay();
    ImGui_ImplGLUT_UpdateKeyboardMods();*/
    (void)x; (void)y; // Unused
}

void ImGui_ImplGLUT_SpecialFunc(int key, int x, int y)
{
    //printf("key_down_func %d\n", key);
    ImGuiIO& io = ImGui::GetIO();
    if (key + 256 < IM_ARRAYSIZE(io.KeysDown))
        io.KeysDown[key + 256] = true;
    ImGui_ImplGLUT_UpdateKeyboardMods();
    (void)x; (void)y; // Unused
}

void ImGui_ImplGLUT_SpecialUpFunc(int key, int x, int y)
{
    //printf("key_up_func %d\n", key);
    ImGuiIO& io = ImGui::GetIO();
    if (key + 256 < IM_ARRAYSIZE(io.KeysDown))
        io.KeysDown[key + 256] = false;
    ImGui_ImplGLUT_UpdateKeyboardMods();
    (void)x; (void)y; // Unused
}

void ImGui_ImplGLUT_MouseFunc(int glut_button, int state, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    int button = -1;
    if (glut_button == GLUT_LEFT_BUTTON) button = 0;
    if (glut_button == GLUT_RIGHT_BUTTON) button = 1;
    if (glut_button == GLUT_MIDDLE_BUTTON) button = 2;
    if (button != -1 && state == GLUT_DOWN)
        io.MouseDown[button] = true;
    if (button != -1 && state == GLUT_UP)
        io.MouseDown[button] = false;
    //ImGui::IsMouseDragging(io.MouseDown[1]);
    //ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Right
    if (io.MouseDown[1]) {
        arcball_on = true;
        //glm::vec4 position(m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z, 1);
        //glm::vec4 pivot(m_camera.GetLookAt().x, m_camera.GetLookAt().y, m_camera.GetLookAt().z, 1);

        //// step 1 : Calculate the amount of rotation given the mouse movement.
        //float deltaAngleX = (2 * M_PI / io.DisplaySize.x); // a movement from left to right = 2*PI = 360 deg
        //float deltaAngleY = (M_PI / io.DisplaySize.y);  // a movement from top to bottom = PI = 180 deg
        //float xAngle = (io.MouseClickedPos[1].x - io.MousePos.x) * deltaAngleX;
        //float yAngle = (io.MouseClickedPos[1].y - io.MousePos.y) * deltaAngleY;

        //// Extra step to handle the problem when the camera direction is the same as the up vector
        //float cosAngle = glm::dot(m_camera.GetViewDir(), m_upVector);
        ///*if (cosAngle * sgn(yDeltaAngle) > 0.99f)
        //    yDeltaAngle = 0;*/

        //// step 2: Rotate the camera around the pivot point on the first axis.
        //glm::mat4x4 rotationMatrixX(1.0f);
        //rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, m_upVector);
        //position = (rotationMatrixX * (position - pivot)) + pivot;

        //// step 3: Rotate the camera around the pivot point on the second axis.
        //glm::mat4x4 rotationMatrixY(1.0f);
        //rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, m_camera.GetRightVector());
        //glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;
        //m_camera.SetCameraView(finalPosition, m_camera.GetLookAt(), m_upVector);

        // Update the mouse position for the next rotation
        //m_lastMousePosX = io.MouseClickedPos[1].x;
        //m_lastMousePosY = io.MouseClickedPos[1].y;
    }
    else {
        arcball_on = false;
    }
    if (io.MouseDown[2]) {
        pan_on = true;
        pan_x = x;
        pan_y = y;
    }
    else {
        pan_on = false;
    }
    m_lastMousePosX = io.MousePos.x;
    m_lastMousePosY = io.MousePos.y;

  
  
    glutPostRedisplay();
}

#ifdef __FREEGLUT_EXT_H__
void ImGui_ImplGLUT_MouseWheelFunc(int button, int dir, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    if (dir > 0)
        io.MouseWheel += 1.0;
    else if (dir < 0)
        io.MouseWheel -= 1.0;
    (void)button; // Unused
}
#endif

void ImGui_ImplGLUT_ReshapeFunc(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
}

void ImGui_ImplGLUT_MotionFunc(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    if (arcball_on) {  // if left button is pressed
        glm::vec4 position(m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z, 1);
        glm::vec4 pivot(m_camera.GetLookAt().x, m_camera.GetLookAt().y, m_camera.GetLookAt().z, 1);

        // step 1 : Calculate the amount of rotation given the mouse movement.
        float deltaAngleX = (2 * M_PI / io.DisplaySize.x); // a movement from left to right = 2*PI = 360 deg
        float deltaAngleY = (M_PI / io.DisplaySize.y);  // a movement from top to bottom = PI = 180 deg
        float xAngle = (m_lastMousePosX - io.MousePos.x) * deltaAngleX;
        float yAngle = (m_lastMousePosY - io.MousePos.y) * deltaAngleY;

   
        // Extra step to handle the problem when the camera direction is the same as the up vector
        float cosAngle = glm::dot(m_camera.GetViewDir(), m_upVector);
        if (cosAngle * ((deltaAngleY > 0) ? 1 : ((deltaAngleY < 0) ? -1 : 0)) > 0.99f)
            deltaAngleY = 0;
   
        

            // step 2: Rotate the camera around the pivot point on the first axis.
        glm::mat4x4 rotationMatrixX(1.0f);
        rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, m_upVector);
        position = (rotationMatrixX * (position - pivot)) + pivot;

        // step 3: Rotate the camera around the pivot point on the second axis.
        glm::mat4x4 rotationMatrixY(1.0f);
        rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, m_camera.GetRightVector());
        glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;
        m_camera.SetCameraView(finalPosition, m_camera.GetLookAt(), m_upVector);
        m_lastMousePosX = io.MousePos.x;
        m_lastMousePosY = io.MousePos.y; 
        io.MouseClickedPos[2];
        //printf("upVec x:%f, y:%f, z:%f\n", m_upVector.x, m_upVector.y, m_upVector.z);
    }

    if (pan_on) {
        //const float cameraSpeed = 50*io.DeltaTime;
        float cameraSpeed = 0.2;
        if (io.DeltaTime < 0.0085) {
            cameraSpeed = 0.2;
        }
        else {
            cameraSpeed = io.DeltaTime * 20;
        }
        glm::vec3 targetToEye;
        glm::vec3 finalLookx;
        glm::vec3 finalLooky;
        glm::vec3 upVec(1, 0, 0);
        glm::vec3 finalLook;
        //printf("cx :%f\n", io.MouseClickedPos[2].x);
        //printf("cy: %f\n", io.MouseClickedPos[2].y);
        //printf("dx1 :%f dx2: %f\n", diffx1, diffx2);
        //printf("x :%f\n", io.MousePos.x);
        targetToEye = m_camera.GetEye() - m_camera.GetLookAt();
        targetToEye = glm::normalize(targetToEye);
        
        if (io.KeyShift) {
            if ((io.MouseClickedPos[2].x - io.MousePos.x) <= 0) { //Moving Left
                if (diffx < io.MousePos.x) {
                    finalLookx = (glm::cross(targetToEye, -m_camera.GetUpVector())) * cameraSpeed;
                    //printf("upVec x:%f, y:%f, z:%f\n", m_camera.GetUpVector().x, m_camera.GetUpVector().y, m_camera.GetUpVector().z);
                }
                else {
                    finalLookx = (glm::cross(targetToEye, m_camera.GetUpVector())) * cameraSpeed;
                }
            }
            else if ((io.MouseClickedPos[2].x - io.MousePos.x) > 0) {   //Moving right
                if (diffx < io.MousePos.x) {
                    finalLookx = (glm::cross(targetToEye, -m_camera.GetUpVector())) * cameraSpeed;
                }
                else {
                    finalLookx = (glm::cross(targetToEye, m_camera.GetUpVector())) * cameraSpeed;
                }

            }
            /*printf("tar x:%f, y:%f, z:%f\n", targetToEye.x, targetToEye.y, targetToEye.z);
            printf("up x:%f, y:%f, z:%f\n", m_camera.GetUpVector().x, m_camera.GetUpVector().y, m_camera.GetUpVector().z);
            printf("final x:%f, y:%f, z:%f\n", finalLookx.x, finalLookx.y, finalLookx.z);*/
            m_camera.SetCameraView(m_camera.GetEye() - finalLookx, m_camera.GetLookAt() - finalLookx, m_upVector);
        }
        glm::vec3 right = glm::normalize(m_camera.GetRightVector());
        if (io.KeyCtrl) {
            if ((io.MouseClickedPos[2].y - io.MousePos.y) <= 0) { //Down
                if (diffy < io.MousePos.y) {
                   //upVec.x = 1;
                    finalLooky = (glm::cross(targetToEye, -right)) * cameraSpeed;

                }
                else {
                    //upVec.x = -1;
                    finalLooky = (glm::cross(targetToEye, right)) * cameraSpeed;

                }

            }
            else if ((io.MouseClickedPos[2].y - io.MousePos.y) > 0) {
                if (diffy < io.MousePos.y) {
                    //upVec.x = 1;
                    finalLooky = (glm::cross(targetToEye, -right)) * cameraSpeed;

                }
                else {
                    //upVec.x = -1;
                    finalLooky = (glm::cross(targetToEye, right)) * cameraSpeed;
                }
                
            }

            glm::vec3 eye(m_camera.GetEye().x - finalLooky.x, m_camera.GetEye().y - finalLooky.y, m_camera.GetEye().z - finalLooky.z);
            glm::vec3 look(m_camera.GetLookAt().x - finalLooky.x, m_camera.GetLookAt().y - finalLooky.y, m_camera.GetLookAt().z - finalLooky.z);
            m_camera.SetCameraView(eye, look, m_upVector);
        }
     
    }
   
    diffx = io.MousePos.x;
    diffy = io.MousePos.y;
}

void myLookAt() {
    glm::vec3 cameraTarget = cameraFront;
    //gluLookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    gluLookAt(m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z, m_camera.GetLookAt().x, m_camera.GetLookAt().y, m_camera.GetLookAt().z, m_camera.GetUpVector().x, m_camera.GetUpVector().y, m_camera.GetUpVector().z);
}

