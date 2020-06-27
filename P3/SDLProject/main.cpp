#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include<iostream>
#include<vector>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define PLATFORM_COUNT 9
#define FENCE_COUNT 7
#define LAND_COUNT 2
#define CHARACTER_COUNT 256
struct GameState {
    Entity *balloon;
    Entity *sea;
    Entity *land;
    Entity *fenceLeft;
    Entity *fenceRight;
    Entity *fence;
   
};

GameState state;
GLuint fontTextureID;
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
bool win = false;
bool pressed =false;
float direction = 0;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
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
    
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize balloon
    state.balloon = new Entity();
    state.balloon->position = glm::vec3(-2,3.25,0);
    state.balloon->movement = glm::vec3(0);
    state.balloon->acceleration = glm::vec3(0,-0.1f,0);
    state.balloon->speed = 1.4f;
    state.balloon->textureID = LoadTexture("balloon.png");
    
    fontTextureID = LoadTexture("font1.png");
  
    
//    int *mission = new int[7]{77,105,125,125,109,111,110};
//    state.characters->animRight = new int[4] {109, 37, 111, 125};
//    state.balloon->animLeft = new int[4] {1, 5, 9, 13};
//    state.balloon->animUp = new int[4] {2, 6, 10, 14};
//    state.balloon->animDown = new int[4] {0, 4, 8, 12};

//    state.characters->animIndices = mission;
//    state.characters->animFrames = 16;
//    state.characters->animIndex = 1;
//    state.characters->animTime = 0;
//    state.characters->animCols = 16;
//    state.characters->animRows = 16;
    
    state.balloon->height = 0.2f;
    state.balloon->width = 0.2f;
    
//    state.balloon->jumpPower = 5.0f;
    
    state.sea = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("sea.png");
    state.sea[0].textureID = platformTextureID;
    state.sea[0].position = glm::vec3(-5,-3.25f,0);
    
    state.sea[1].textureID = platformTextureID;
    state.sea[1].position = glm::vec3(-4,-3.25f,0);
    
    state.sea[2].textureID = platformTextureID;
    state.sea[2].position = glm::vec3(-1,-3.25f,0);
    
    state.sea[3].textureID = platformTextureID;
    state.sea[3].position = glm::vec3(0,-3.25f,0);
    
    state.sea[4].textureID = platformTextureID;
    state.sea[4].position = glm::vec3(1,-3.25f,0);
    
    state.sea[5].textureID = platformTextureID;
    state.sea[5].position = glm::vec3(2,-3.25f,0);
    
    state.sea[6].textureID = platformTextureID;
    state.sea[6].position = glm::vec3(3,-3.25f,0);
    
    state.sea[7].textureID = platformTextureID;
    state.sea[7].position = glm::vec3(4,-3.25f,0);
    
    state.sea[8].textureID = platformTextureID;
    state.sea[8].position = glm::vec3(5,-3.25f,0);


    
    state.land = new Entity[LAND_COUNT];
    GLuint landTextureID = LoadTexture("land.png");
    state.land[0].textureID = landTextureID;
    state.land[0].position = glm::vec3(-3,-3.25f,0);
    
    state.land[1].textureID = landTextureID;
    state.land[1].position = glm::vec3(-2,-3.25f,0);
    
    state.fenceLeft= new Entity[FENCE_COUNT];
    GLuint fenceTextureID = LoadTexture("fence.png");
    state.fenceLeft[0].textureID = fenceTextureID;
    state.fenceLeft[0].position = glm::vec3(4.7,3.25f,0);
    
    state.fenceLeft[1].textureID = fenceTextureID;
    state.fenceLeft[1].position = glm::vec3(4.7,2.25f,0);
    
    state.fenceLeft[2].textureID = fenceTextureID;
    state.fenceLeft[2].position = glm::vec3(4.7,1.25f,0);
    
    state.fenceLeft[3].textureID = fenceTextureID;
    state.fenceLeft[3].position = glm::vec3(4.7,0.25f,0);
    
    state.fenceLeft[4].textureID = fenceTextureID;
    state.fenceLeft[4].position = glm::vec3(4.7,-0.75f,0);
    
    state.fenceLeft[5].textureID = fenceTextureID;
    state.fenceLeft[5].position = glm::vec3(4.7,-1.75f,0);
    
    state.fenceLeft[6].textureID = fenceTextureID;
    state.fenceLeft[6].position = glm::vec3(4.7,-2.75f,0);
    
    state.fenceRight= new Entity[FENCE_COUNT];
    GLuint fence1TextureID = LoadTexture("fence2.png");
    state.fenceRight[0].textureID = fence1TextureID;
    state.fenceRight[0].position = glm::vec3(-4.7,3.25f,0);
    
    state.fenceRight[1].textureID = fence1TextureID;
    state.fenceRight[1].position = glm::vec3(-4.7,2.25f,0);
    
    state.fenceRight[2].textureID = fence1TextureID;
    state.fenceRight[2].position = glm::vec3(-4.7,1.25f,0);
    
    state.fenceRight[3].textureID = fence1TextureID;
    state.fenceRight[3].position = glm::vec3(-4.7,0.25f,0);
    
    state.fenceRight[4].textureID = fence1TextureID;
    state.fenceRight[4].position = glm::vec3(-4.7,-0.75f,0);
    
    state.fenceRight[5].textureID = fence1TextureID;
    state.fenceRight[5].position = glm::vec3(-4.7,-1.75f,0);
    
    state.fenceRight[6].textureID = fence1TextureID;
    state.fenceRight[6].position = glm::vec3(-4.7,-2.75f,0);
    
    state.fence= new Entity[FENCE_COUNT];
    GLuint fence2TextureID = LoadTexture("fence1.png");
    state.fence[0].textureID = fence2TextureID;
    state.fence[0].position = glm::vec3(1.6,2.2,0);
    
    state.fence[1].textureID = fence2TextureID;
    state.fence[1].position = glm::vec3(2.4,2.2,0);
    
    state.fence[2].textureID = fence2TextureID;
    state.fence[2].position = glm::vec3(3.2,2.2,0);
    
    state.fence[3].textureID = fence2TextureID;
    state.fence[3].position = glm::vec3(4,2.2,0);
    
    state.fence[4].textureID = fence2TextureID;
    state.fence[4].position = glm::vec3(-4,-0.5,0);
    
    state.fence[5].textureID = fence2TextureID;
    state.fence[5].position = glm::vec3(-3.2,-0.5,0);
    
    state.fence[6].textureID = fence2TextureID;
    state.fence[6].position = glm::vec3(-2.4,-0.5,0);
    
    
    for(int i = 0; i<PLATFORM_COUNT;i++){
        state.sea[i].Update(0, NULL,0);
    }
    for(int i = 0; i<LAND_COUNT;i++){
        state.land[i].Update(0, NULL,0);
    }
    for(int i = 0; i<FENCE_COUNT;i++){
        state.fenceLeft[i].Update(0, NULL,0);
    }
    for(int i = 0; i<FENCE_COUNT;i++){
        state.fenceRight[i].Update(0, NULL,0);
    }
    for(int i = 0; i<FENCE_COUNT;i++){
        state.fence[i].Update(0, NULL,0);
    }
    
 
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
    
    state.balloon->movement = glm::vec3(0);
    
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
                        // Move the balloon left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the balloon right
                        break;
                        
                    case SDLK_SPACE:
                       start = true;
                        pressed = true;
                        
                        
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        
        // state.balloon->movement.x = -1.0f;
        state.balloon->acceleration.x -= 1.0f;
        if(state.balloon->velocity.x > 0){
            state.balloon->acceleration.x -= 2.0f;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        // state.balloon->movement.x = 1.0f;
        state.balloon->acceleration.x += 1.0f;
        if(state.balloon->velocity.x < 0){
            state.balloon->acceleration.x += 2.0f;
        }
        //state.balloon->animIndices = state.balloon->animRight;
    }
    else {
        if(state.balloon->velocity.x > 0){
            state.balloon->acceleration.x -= 2.0f;
        }
        else if(state.balloon->velocity.x < 0){
            state.balloon->acceleration.x += 2.0f;
        }
    }
    

    if (glm::length(state.balloon->movement) > 1.0f) {
        state.balloon->movement = glm::normalize(state.balloon->movement);
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
        if(start){
            
            state.balloon->collidedTop = false;
            state.balloon->collidedBottom = false;
            state.balloon->collidedRight = false;
            state.balloon->collidedLeft = false;
            
            state.balloon->Update(FIXED_TIMESTEP, state.fenceLeft, FENCE_COUNT);
            state.balloon->Update(FIXED_TIMESTEP, state.fenceRight, FENCE_COUNT);
            
            state.balloon->Update(FIXED_TIMESTEP, state.fence, FENCE_COUNT);
            state.balloon->Update(FIXED_TIMESTEP, state.sea, PLATFORM_COUNT);
            if(state.balloon->collidedLeft
               ||state.balloon->collidedRight
               ||state.balloon->collidedBottom
               ||state.balloon->collidedTop){
                std::cout << "collided\n";
//                    state.balloon->acceleration = glm::vec3(0);
//                    state.balloon->velocity = glm::vec3(0);
//                    state.balloon->speed = 0;
                    win = false;
                    start = false;
                }
            state.balloon->collidedBottom = false;
            state.balloon->Update(FIXED_TIMESTEP, state.land, LAND_COUNT);
            if(state.balloon->collidedBottom){
//                state.balloon->acceleration = glm::vec3(0);
//                state.balloon->velocity = glm::vec3(0);
//                state.balloon->speed = 0;
                win = true;
                start = false;
            }
               
        }
        
        deltaTime -= FIXED_TIMESTEP;
        
    }
    accumulator = deltaTime;
    
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.sea[i].Render(&program);
    }
    for(int i = 0; i < LAND_COUNT; i++){
        state.land[i].Render(&program);
    }
    for(int i = 0; i < FENCE_COUNT; i++){
        state.fenceLeft[i].Render(&program);
    }
    for(int i = 0; i < FENCE_COUNT; i++){
        state.fenceRight[i].Render(&program);
    }
    for(int i = 0; i < FENCE_COUNT; i++){
        state.fence[i].Render(&program);
    }
    if(!start && !pressed){
         DrawText(&program,fontTextureID, "Press SPACE to start" , 0.5f, -0.25f, glm::vec3(-2.75f, -2.5, 0));
    }
    if(!start && pressed){
        if(win){
            DrawText(&program,fontTextureID, "Mission Successful" , 0.5f, -0.25f, glm::vec3(-1.75f, 0.9, 0));
        }
        else{
            DrawText(&program,fontTextureID, "Mission Failed" , 0.5f, -0.25f, glm::vec3(-1.75f, 0.9, 0));
        }
       
    }
    
    
    
   
    
    state.balloon->Render(&program);
   
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
