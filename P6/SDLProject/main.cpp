#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#include<GL/glut.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Menu.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool end = false;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];
GLuint fontTextureID;
Mix_Music *music;
Mix_Chunk *jump;
int lives;

//GLuint fontTextureID;

void SwitchToScene(Scene *scene)
{
    currentScene = scene;
    currentScene->Initialize();
}




void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Game!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("take-me-out-to-the-ballgame-by-kevin-macleod-from-filmmusic-io.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    jump = Mix_LoadWAV("jump.wav");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
  
   // Initialize the levels and start at the first one
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
    
    fontTextureID = Util::LoadTexture("font1.png");
}

void ProcessInput() {
    if(currentScene!=sceneList[0]){
        currentScene->state.player->movement = glm::vec3(0);
    }
    
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if(end==false && currentScene->start== false){
                            currentScene->start = true;
                        }
                        break;
                        
                    case SDLK_RIGHT:
                        if(end==false && currentScene->start== false){
                            currentScene->start = true;
                        }
                        break;
                        
                    case SDLK_SPACE:
                        if(currentScene!=sceneList[0] && currentScene->state.player->collidedBottom && !end){
                            currentScene->state.player->jump = true;
                            Mix_PlayChannel(-1, jump, 0);
                        }
                        
                        
                        break;
                    case SDLK_RETURN:
                        if(currentScene == sceneList[0]){
                            currentScene->state.nextScene = 1;
                        }
                       
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (end==false && currentScene!=sceneList[0] && keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (end==false && currentScene!=sceneList[0] && keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    

    if (currentScene!=sceneList[0] && glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
        
    }
    while (deltaTime >= FIXED_TIMESTEP) {
    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
        
    }
    accumulator = deltaTime;
    viewMatrix = glm::mat4(1.0f);
    if(currentScene!=sceneList[0]){
        if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x <15) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else if(currentScene->state.player->position.x >15){
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-15, 3.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
    }
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    

    if (currentScene!=sceneList[0]&&currentScene->livest == 0){
        end = true;
        Util::DrawText(&program, fontTextureID, "You Lose!", 1.0f, -0.5f, glm::vec3(currentScene->state.player->position.x, -1, 0));
    }
   
    if(currentScene == sceneList[3] && currentScene->state.player->position.x>19.8 && currentScene->livest>0){
        Util::DrawText(&program, fontTextureID, "You Win!", 1.0f, -0.5f, glm::vec3(14, -1, 0));
        end = true;
    }
    
    
    currentScene->Render(&program);
    
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
        
        if(currentScene->state.nextScene >=0 && currentScene!=sceneList[3]){
            lives = currentScene->livest;
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            currentScene->livest = lives;
        }
                          
        Render();
    }
    
    Shutdown();
    return 0;
}
