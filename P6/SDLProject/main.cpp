#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>
#include "Util.h"
#include "Entity.h"


 
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
bool generate = false;
bool up = false;
bool down = false;
bool left = false;
bool right = false;
int power = 0;
int shoot = false;
int lives =3;
int seconds = 120;
int win = false;
int lose = false;
int count = 0;
#define OBJECT_COUNT 5
#define ENEMY_COUNT 4
#define WEAPON_COUNT 1
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;
struct GameState {
    Entity *player;
    Entity *objects;
    Entity *enemies;
    Entity *weapons;
};
Mix_Music *music;
Mix_Chunk *hit;
GameState state;
GLuint heartTextureID;
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Beat Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 2560, 1440);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
    
    
        
    
    
    state.objects = new Entity[OBJECT_COUNT];
    GLuint floorTextureID = Util::LoadTexture("floor2.png");
    Mesh *cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj",3);
    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0,-0.25f,0);
    state.objects[0].rotation = glm::vec3(0,0,-90);
    state.objects[0].velocity = glm::vec3(0,0,0.5);
    state.objects[0].acceleration = glm::vec3(0,0,0);
    state.objects[0].entityType = FLOOR;
    state.objects[0].scale = glm::vec3(1,1.5f,125);
    state.objects[0].depth = 0.5;
    
    GLuint crateTextureID = Util::LoadTexture("crate.jpeg");
       Mesh *crateMesh = new Mesh();
       crateMesh->LoadOBJ("cube.obj", 1);
    state.objects[1].textureID = crateTextureID;
    state.objects[1].mesh = crateMesh;
    state.objects[1].position = glm::vec3(-0.1,0.5,-0.5);
    state.objects[1].entityType = CRATE;
    state.objects[1].scale = glm::vec3(0.15f,0.25f,0.5f);
    state.objects[1].width = 0.2;
    

    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("up.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
    }
    state.enemies[0].position = glm::vec3(-0.45, 1.8, -5);
    state.enemies[0].rotation = glm::vec3(0,0, -90.0f);
    state.enemies[0].scale = glm::vec3(0.5,0.8f,0.1);
    
    state.enemies[1].position = glm::vec3(-0.15, 1.8, -10);
    state.enemies[1].rotation = glm::vec3(0,0,-180);
    state.enemies[1].scale = glm::vec3(0.4,0.7f,0.1);
    
    state.enemies[2].position = glm::vec3(0.15, 1.8, -15);
    state.enemies[2].rotation = glm::vec3(0,0, 0);
    state.enemies[2].scale = glm::vec3(0.4,0.7f,0.1);
    
    state.enemies[3].position = glm::vec3(0.45, 1.8, -20);
    state.enemies[3].rotation = glm::vec3(0,0,-270);
    state.enemies[3].scale = glm::vec3(0.5,0.8f,0.1);
    
    
    
    

    
    

 
    GLuint raichuTextureID = Util::LoadTexture("raichubody.png");
    Mesh *raichuMesh = new Mesh();
    raichuMesh->LoadOBJ("Raichu.obj",1);
    for (int i=2;i<OBJECT_COUNT;i++){
        state.objects[i].textureID = raichuTextureID;
        state.objects[i].mesh = raichuMesh;
        state.objects[i].position = glm::vec3(0,0.5,-12-3*i);
        state.objects[i].scale = glm::vec3(0.05f,0.05f,0.05f);
        state.objects[i].width = 0.07;
        state.objects[i].depth = 0.05;
        state.objects[i].entityType = RAICHU;
        
    }
    
    state.objects[2].velocity = glm::vec3(0,0,2);
    state.objects[3].velocity = glm::vec3(0.2,0,0.5);
    state.objects[4].velocity = glm::vec3(0.3,0,0.8);
    
    state.weapons = new Entity();
   
        state.weapons->textureID = crateTextureID;
        state.weapons->mesh = crateMesh;
        state.weapons->entityType = WEAPON;
        state.weapons->rotation = glm::vec3(0,0,0);
        state.weapons->scale = glm::vec3(0.01,0.01,0.01);
        state.weapons->width = 0.05;
       state.weapons->position = glm::vec3(state.objects[1].position.x+0.05, 0.63f, -0.35);
        state.weapons->acceleration = glm::vec3(0, 0, 0);

    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("font1.png");
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("balloon.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    hit = Mix_LoadWAV("hit.wav");
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if(power>=10){
                            state.weapons->velocity = glm::vec3(0,0,-1);
                            power = power-10;
                            shoot = false;
                        }
                    
                        
                        break;
                    case SDLK_RETURN:
                        if(start == false){
                           start = true;
                        }
                        break;
                    case SDLK_UP:
                        up = true;
                        down = false;
                        left = false;
                        right = false;
                        break;
                    case SDLK_DOWN:
                        up = false;
                        down = true;
                        left = false;
                        right = false;
                        break;
                    case SDLK_LEFT:
                        up = false;
                        down = false;
                        left = true;
                        right = false;
                        break;
                    case SDLK_RIGHT:
                        up = false;
                        down = false;
                        left = false;
                        right = true;
                        break;
                }
                break;
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
       
        if(state.objects[1].position.x>=-0.4){
            state.objects[1].position.x -=0.03;
        }
        
    }
    else if (keys[SDL_SCANCODE_D]) {
        
        if(state.objects[1].position.x<=0.4){
            state.objects[1].position.x +=0.03;
        }
        
    }

    state.player->velocity.x = 0;
    state.player->velocity.z = 0;

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
        state.player->Update(FIXED_TIMESTEP, state.player,state.objects, OBJECT_COUNT);
        
        for (int i = 0; i<OBJECT_COUNT; i++){
            state.objects[i].Update(FIXED_TIMESTEP,state.player,state.objects, OBJECT_COUNT);
        }
        
        int result = rand()%4;
        
        for (int i = 0; i<ENEMY_COUNT; i++){
            if(state.enemies[i].position.z>=-1){
                state.enemies[i].position.x = 0.3*result-0.45;
                if((state.enemies[i].rotation.z==-90 && left)||(state.enemies[i].rotation.z==-180 && down)||(state.enemies[i].rotation.z==0 && up)||(state.enemies[i].rotation.z==-270 && right)){
                    power+=1;
                }
                
                state.enemies[i].rotation.z =-90*result;
                state.enemies[i].position.z = -20;
            }
            state.enemies[i].Update(FIXED_TIMESTEP,state.player,state.objects, OBJECT_COUNT);
            
        }
        if(power>=10){
            shoot = true;
        }
        
        if(state.objects[3].position.x<=-0.5){
            state.objects[3].velocity = glm::vec3(0.2,0,0.5);
        }
        if(state.objects[3].position.x>=0.5){
            state.objects[3].velocity = glm::vec3(-0.2,0,0.5);
        }
        
        if(state.objects[4].position.x<=-0.5){
            state.objects[4].velocity = glm::vec3(0.3,0,0.8);
        }
        if(state.objects[4].position.x>=0.5){
            state.objects[4].velocity = glm::vec3(-0.3,0,0.8);
        }
        for (int i=2;i<OBJECT_COUNT;i++){
            if(state.objects[i].position.z>=-0.1){
                state.objects[i].position=glm::vec3(-0.49+rand()%10*0.1,0.5, -1*rand()%10-10);
            }
            if(state.objects[i].killed) {
                state.objects[i].position=glm::vec3(-0.49+rand()%10*0.1,0.5, -1*rand()%10-10);
                state.weapons->position.x = state.objects[1].position.x+0.1;
                state.weapons->position.z = -0.35;
                state.weapons->velocity = glm::vec3(0);
                state.objects[i].killed = false;
            }
            
            
            
            
        }
        if(state.objects[1].minus){
            if(!lose && !win && start){
                Mix_PlayChannel(-1, hit, 0);
            
        
                lives--;
                state.objects[1].minus = false;
            }
        }
    
        if (!state.weapons->velocity.z){
            state.weapons->position.x = state.objects[1].position.x;
        }
        if (state.weapons->position.z < -8){
            state.weapons->position.z = -0.35;
            state.weapons->velocity = glm::vec3(0);
        }
        
        
        state.weapons->Update(FIXED_TIMESTEP,state.player,state.objects,OBJECT_COUNT);
        if(start){
            if(count<=58){
                count++;
            }
            else{
                seconds--;
                count = 0;
            }
            
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -state.player->position);
}


void Render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.SetProjectionMatrix(projectionMatrix);
    
    program.SetViewMatrix(viewMatrix);
    if(start == false ){
        state.objects[0].Render(&program);
        program.SetProjectionMatrix(uiProjectionMatrix);
        program.SetViewMatrix(uiViewMatrix);
        Util::DrawText(&program, fontTextureID, "Beat Game", 0.8, -0.3f, glm::vec3(-2, 2.2, 0));
        Util::DrawText(&program, fontTextureID, "Press ENTER to start", 0.8, -0.3f, glm::vec3(-5, 1.2, 0));
        
    }
   
    
    else{
        if(lives<=0){
            state.objects[0].Render(&program);
            program.SetProjectionMatrix(uiProjectionMatrix);
            program.SetViewMatrix(uiViewMatrix);
            Util::DrawText(&program, fontTextureID, "You lose", 0.8, -0.3f, glm::vec3(-2, 2.2, 0));
            lose= true;
        }
        else if(seconds<=0){
            state.objects[0].Render(&program);
            program.SetProjectionMatrix(uiProjectionMatrix);
            program.SetViewMatrix(uiViewMatrix);
            Util::DrawText(&program, fontTextureID, "You Win", 0.8, -0.3f, glm::vec3(-2, 2.2, 0));
            win = true;
        }
        else{
             state.weapons->Render(&program);
            for (int i = 0; i<OBJECT_COUNT; i++){
                state.objects[i].Render(&program);
            }
            for (int i = 0; i<ENEMY_COUNT; i++){
                state.enemies[i].Render(&program);
            }
            
           
            
            program.SetProjectionMatrix(uiProjectionMatrix);
            program.SetViewMatrix(uiViewMatrix);
            if(shoot){
                Util::DrawText(&program, fontTextureID, "Press SPACE to shoot", 0.7, -0.3f, glm::vec3(-6, 1.2, 0));
            }
                Util::DrawText(&program, fontTextureID, "Power: "+std::to_string(power), 0.6, -0.3f, glm::vec3(-6, 3.2, 0));
            Util::DrawText(&program, fontTextureID, "Lives: "+std::to_string(lives), 0.6, -0.3f, glm::vec3(-6, 2.7, 0));
                Util::DrawText(&program, fontTextureID, "Time: "+std::to_string(seconds), 0.6, -0.3f, glm::vec3(-6, 2.2, 0));
            
            
            
           
            
            
            

        }
        program.SetProjectionMatrix(uiProjectionMatrix);
        program.SetViewMatrix(uiViewMatrix);
        
    }
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
