#pragma once

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
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
