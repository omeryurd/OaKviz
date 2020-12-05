#pragma once

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include "OBJLoader/OBJ_Loader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include "FileBrowser/Dirent/dirent.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "OBJLoader/OBJ_Loader.h"

void init_window(int argc, char** argv);
void init_other();
void showMainMenu();
void loadObj(const char* path);
void my_display_code(void);
void glut_display_func(void);
void normalizeCoordinates(void);
void addToScene(std::vector<objl::Loader*>& loadedObjs, const char* path);
void glut_keyboard_func(unsigned char c, int x, int y);
objl::Loader* loadObject(const char* path, std::string objectName);
