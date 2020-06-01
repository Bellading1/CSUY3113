#define GL_SILENCE_DEPRECATION
#include <iostream>
using namespace std;
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool goRight = true;
ShaderProgram program;
glm::mat4 viewMatrix, celesteMatrix,starMatrix, projectionMatrix;

float player_x = 0;
float rotate_star = 0;
float star_x = 0;
float star_y = 0;


GLuint celesteTextureID;
GLuint starTextureID;


GLuint LoadTexture(const char* filePath) {
    int w, h, n;
unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
if (image == NULL) {
std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
}
GLuint textureID;
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Meteor Shower!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 1280, 960 );
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    celesteMatrix = glm::mat4(1.0f);
    celesteMatrix = glm::translate(celesteMatrix,glm::vec3(-3.0f,-3.25f,0.0f));
    starMatrix = glm::mat4(1.0f);
    starMatrix = glm::translate(starMatrix,glm::vec3(3.0f,2.0f,0.0f));
   
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    
    
    glEnable(GL_BLEND);
    //Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    celesteTextureID = LoadTexture("Acnl-celeste.png");
    starTextureID = LoadTexture("Star-AC.png");
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float deltaTime = ticks-lastTicks;
    lastTicks = ticks;
    float x = celesteMatrix[3][0];
    
    if (x>5 && goRight){
        goRight = false;
       
    }
    else if(x<-5 && goRight==false){
        goRight = true;
       
    }
    
    if (goRight){
        player_x +=1.0f*deltaTime;
    }
    else{
        player_x -=1.0f*deltaTime;
    }
    celesteMatrix = glm::mat4(1.0f);
    celesteMatrix = glm::translate(celesteMatrix,glm::vec3(-3.0f,-3.25f,0.0f));
    celesteMatrix= glm::translate(celesteMatrix, glm::vec3(player_x, 0.0f, 0.0f));
   
    star_x-=5.0f*deltaTime;
    star_y = star_x/4;
    rotate_star+=90.0f*deltaTime;
    starMatrix = glm::mat4(1.0f);
    starMatrix = glm::translate(starMatrix,glm::vec3(4.5f,2.5f,0.0f));
    starMatrix=glm::translate(starMatrix,glm::vec3(star_x, star_y, 0.0f));
    if(starMatrix[3][0]<-5){
        star_x=0;
        star_y=0;
    }
    starMatrix = glm::rotate(starMatrix,glm::radians(rotate_star), glm::vec3(0.0f,0.0f,1.0f));
    
}
void DrawCeleste(){
    program.SetModelMatrix(celesteMatrix);
    glBindTexture(GL_TEXTURE_2D, celesteTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Drawstar(){
    program.SetModelMatrix(starMatrix);
    glBindTexture(GL_TEXTURE_2D, starTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void Render() {
    
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); glEnableVertexAttribArray(program.positionAttribute); glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords); glEnableVertexAttribArray(program.texCoordAttribute);
    DrawCeleste();
    Drawstar();
    glDisableVertexAttribArray(program.positionAttribute); glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
   
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
