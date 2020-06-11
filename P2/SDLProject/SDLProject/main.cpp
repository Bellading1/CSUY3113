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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameBegin = false;
bool end = false;
bool restart = true;
ShaderProgram program;
glm::mat4 viewMatrix, player1Matrix, player2Matrix, ballMatrix, projectionMatrix;

float ball_x = 0;
float ball_y = 0;
float ball_x_sign = 1.0f;
float ball_y_sign = -1.0f;


glm::vec3 player1_position = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 player1_movement = glm::vec3(0, 0, 0);

glm::vec3 player2_position = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 player2_movement = glm::vec3(0, 0, 0);


float player_speed = 2.0f;




void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
   
    
}

void ProcessInput() {
    player1_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);
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
                            // Some sort of action
                            if(end){
                                end = false;
                                restart = true;
                            }
                            else{
                                gameBegin = true;
                            }
                }
                break;//SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP] && player2_position.y<3.35) {
        player2_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && player2_position.y>-3.35) {
        player2_movement.y = -1.0f;
    }
    if(keys[SDL_SCANCODE_W] && player1_position.y<3.35){
        player1_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]&& player1_position.y>-3.35) {
        player1_movement.y = -1.0f;
    }

}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    // Add (direction * units per second * elapsed time)
    if(restart){
        player1Matrix = glm::mat4(1.0f);
        player2Matrix = glm::mat4(1.0f);
        ballMatrix = glm::mat4(1.0f);
        ball_x = 0;
        ball_y = 0;
        player1_movement = glm::vec3(0);
        player2_movement = glm::vec3(0);
        player1_position = glm::vec3(0);
        player2_position = glm::vec3(0);
        ball_x_sign = 1.0f;
        ball_y_sign = -1.0f;
        
    
    }
    if(gameBegin){
        restart = false;
        player1_position += player1_movement * player_speed * deltaTime;
        player1Matrix = glm::mat4(1.0f);
        player1Matrix = glm::translate(player1Matrix, player1_position);
        player2_position += player2_movement * player_speed * deltaTime;
        player2Matrix = glm::mat4(1.0f);
        player2Matrix = glm::translate(player2Matrix, player2_position);
        ballMatrix = glm::mat4(1.0f);
        ball_x+=ball_x_sign*player_speed*deltaTime;
        ball_y+=ball_y_sign*player_speed*deltaTime;
        float xdist1 = fabs(ball_x-player1_position.x+4.8)-((0.2+0.4)/2.0f);
        float ydist1 = fabs(ball_y+1.8-player1_position.y)-((0.8+0.4)/2.0f);
        float xdist2 = fabs(ball_x-player2_position.x-4.8)-((0.2+0.4)/2.0f);
        float ydist2 = fabs(ball_y+1.8-player2_position.y)-((0.8+0.4)/2.0f);
        if((xdist1<0 && ydist1<0)){
            
            ball_x_sign = 1.0f;
            
        }
        if(xdist2<0 && ydist2<0){
            ball_x_sign = -1.0f;
        }
        if(ball_y+1.8+0.2>3.75){
            ball_y_sign = -1.0f;
        }
        if(ball_y+1.8-0.2<-3.75){
            ball_y_sign = 1.0f;
        }
        if(ball_x+0.2>5||ball_x-0.2<-5){
            end = true;
            gameBegin = false;
        }
        ballMatrix = glm::translate(ballMatrix,glm::vec3(ball_x,ball_y,0.0f));
    }
   


}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(player1Matrix);
    float vertices[] = { -4.9f, 0.4f, -4.9f, -0.4f, -4.7f, 0.4f,-4.7f,-0.4f};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    program.SetModelMatrix(player2Matrix);
    float vertices2[] = { 4.9f, 0.4f, 4.9f, -0.4f, 4.7f, 0.4f,4.7f,-0.4f};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    program.SetModelMatrix(ballMatrix);
    float vertices3[] = { 0.2f, 2.0f, 0.2f, 1.6f, -0.2f, 2.0f, -0.2f,1.6f};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(program.positionAttribute);
    
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
