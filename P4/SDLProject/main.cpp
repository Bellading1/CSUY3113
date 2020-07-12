#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <vector>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define PLATFORM_COUNT 25
#define ENEMY_COUNT 3
#define WEAPON_COUNT 20

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
    Entity *bullet;
    Entity *weapon;
    
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
bool lose = false;
bool end = false;
int countWeapon = 0;
int direction = 1;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint fontTextureID;
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Tank!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4,-3,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 1.4f;
    state.player->textureID = LoadTexture("player.png");
    
    //state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {4,5,6,7};
    state.player->animRight = new int[4] {8,9,10,11};
   // state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.5f;
    
    state.player->jumpPower = 5.0f;
    
    state.weapon = new Entity[WEAPON_COUNT];
    for(int i = 0; i<WEAPON_COUNT;i++){
        state.weapon[i].textureID = LoadTexture("bullet.png");
        state.weapon[i].entityType =WEAPON;
        state.weapon[i].speed = 2;
        state.weapon[i].height = 0.1;
        state.weapon[i].width = 0.1;
        state.weapon[i].isActive = false;
    }
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("ground.png");
    
    for(int i = 0; i< 11;i++){
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5+i,-3.25f,0);
    }
    for(int i = 11; i< 22;i++){
        
            state.platforms[i].entityType = PLATFORM;
            state.platforms[i].textureID = platformTextureID;
        if(i!=19){
            state.platforms[i].position = glm::vec3(-5+i-11,-0.5f,0);
        }
        
    }
    
    
    
    
    
    
    for(int i =22 ; i< 25;i++){
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
    }
   
    state.platforms[23].position = glm::vec3(3.5f,2.3f,0);
    state.platforms[24].position = glm::vec3(4.5f,2.3f,0);
    state.platforms[22].position = glm::vec3(-5,2.3f,0);
    state.platforms[19].position = glm::vec3(-4,2.3f,0);
    for(int i = 0; i<PLATFORM_COUNT;i++){
        state.platforms[i].Update(0,NULL, NULL,NULL,0);
    }

    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint goblinTextureID = LoadTexture("Goblin.png");
    GLuint tankTextureID = LoadTexture("tank.png");
    
    state.enemies[0].animLeft = new int[5] {5,6,7,8,9};
    state.enemies[0].animRight = new int[5] {10,11,12,13,14};
    
    state.enemies[0].animFrames = 5;
    state.enemies[0].animIndex = 0;
    state.enemies[0].animTime = 0;
    state.enemies[0].animCols = 5;
    state.enemies[0].animRows = 4;
    state.enemies[0].height = 0.8f;
    state.enemies[0].width = 0.8f;
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = goblinTextureID;
    state.enemies[0].position = glm::vec3(2.5,-2.25f,0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].animIndices = state.enemies[0].animLeft;
    
    state.enemies[1].animLeft = new int[3] {3,4,5};
    state.enemies[1].animRight = new int[3] {6,7,8};
    
    state.enemies[1].animFrames = 3;
    state.enemies[1].animIndex = 0;
    state.enemies[1].animTime = 0;
    state.enemies[1].animCols = 3;
    state.enemies[1].animRows = 4;
    state.enemies[1].height = 0.8f;
    state.enemies[1].width = 0.8f;
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = tankTextureID;
    state.enemies[1].position = glm::vec3(0,0.4f,0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = PATROLLER;
    state.enemies[1].animIndices = state.enemies[1].animLeft;
    
   
    state.enemies[2].animLeft = new int[3] {3,4,5};
    state.enemies[2].animRight = new int[3] {6,7,8};
    
    state.enemies[2].animFrames = 3;
    state.enemies[2].animIndex = 0;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 3;
    state.enemies[2].animRows = 4;
    state.enemies[2].height = 0.8f;
    state.enemies[2].width = 0.8f;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = tankTextureID;
    state.enemies[2].position = glm::vec3(4,3.1f,0);
    state.enemies[2].speed = 0;
    state.enemies[2].aiType = WALKER;
    state.enemies[2].animIndices = state.enemies[2].animLeft;
    
    state.bullet = new Entity();
    state.bullet->textureID = LoadTexture("bullet.png");
    state.bullet->entityType = BULLET;
    state.bullet->speed = 2;
    state.bullet->height = 0.1;
    state.bullet->width = 0.1;
    state.bullet->position = glm::vec3(3.9,3.32f,0);
    

    fontTextureID = LoadTexture("font1.png");
}
void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position){
    float width = 1.0f/16.0f;
    float height = 1.0f/16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
        offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), { u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
     
}
void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
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
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                    case SDLK_s:
                        state.weapon[countWeapon].isActive = true;
                        
                        state.weapon[countWeapon].position=glm::vec3(state.player->position.x,state.player->position.y,0);
                        state.weapon[countWeapon].movement.x=direction;
                        if(countWeapon<WEAPON_COUNT){
                            countWeapon++;
                        }
                        else{
                            countWeapon = 0;
                        }
                            
                        break;
                    case SDLK_SPACE:
                        
                        if(state.player->collidedBottom && start){
                            state.player->jump = true;
                            if(state.player->collidedTop==false){
                                state.player->jumpPower=8;
                            }
                            
                            else{
                                state.player->jumpPower=5;
                            }
                            
                            
                        }
                        else{
                            start = true;
                            end = false;
                        }
                        
                        
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] && start) {
        direction = -1;
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && start) {
        direction = 1;
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
       
            state.player->Update(FIXED_TIMESTEP, state.player,state.platforms, NULL, PLATFORM_COUNT);
        if(state.player->position.y>3.5){
            state.player->position.y = 3.5;
        }
            if (!state.enemies[2].isActive){
                state.bullet->isActive = false;
            }
        if(state.player->position.y>0){
            state.enemies[0].speed = 0;
        }
        else{
            state.enemies[0].speed = 1;
        }
            if(!state.enemies[0].isActive&&!state.enemies[1].isActive&&!state.enemies[2].isActive){
                start=false;
                lose = false;
                end = true;
            }
            state.bullet->Update(FIXED_TIMESTEP, state.player,state.platforms, NULL, PLATFORM_COUNT);
            
            for (int i = 0; i<ENEMY_COUNT; i++){
                state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, NULL, PLATFORM_COUNT);
                if(state.player->CheckCollision(&state.enemies[i])){
                    state.player->killed = true;
                }
            
            }
            for (int i = 0; i<WEAPON_COUNT; i++){
                for (int j = 0; j<ENEMY_COUNT; j++){
                    state.weapon[i].Update(FIXED_TIMESTEP, state.player, state.platforms,&state.enemies[j],WEAPON_COUNT);
                }
            }
            if(!state.bullet->Shooting(state.player)||state.player->killed){
                start = false;
                end = true;
                lose = true;
                state.player->isActive = false;
            }
           
            deltaTime -= FIXED_TIMESTEP;
           
            
        }
        accumulator = deltaTime;
    
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    for(int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(&program);
    }
    for(int i = 0; i < WEAPON_COUNT; i++){
        state.weapon[i].Render(&program);
    }
    state.bullet->Render(&program);
    
    state.player->Render(&program);
    if(!start && !end){
        DrawText(&program,fontTextureID, "Press SPACE to start/jump" , 0.5f, -0.25f, glm::vec3(-2.85f, -2.4, 0));
        DrawText(&program,fontTextureID, "Use ARROW KEYS to control the direction" , 0.5f, -0.25f, glm::vec3(-4.8f, -3.4, 0));
        DrawText(&program,fontTextureID, "Get rid of all the enemies to win" , 0.5f, -0.25f, glm::vec3(-4.2f, 1.5, 0));
        DrawText(&program,fontTextureID, "Press S to shoot" , 0.5f, -0.25f, glm::vec3(-2.2f, 0.5, 0));
    }
    if(end){
        if(!lose){
            DrawText(&program,fontTextureID, "You Win" , 0.5f, -0.25f, glm::vec3(-1.75f, 0.9, 0));
        }
        else{
            DrawText(&program,fontTextureID, "You Lose" , 0.5f, -0.25f, glm::vec3(-1.75f, 0.9, 0));
        }
       
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
