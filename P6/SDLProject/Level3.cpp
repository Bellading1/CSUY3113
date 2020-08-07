#include "Level3.h"
#define LEVEL3_WIDTH 21
#define LEVEL3_HEIGHT 8
#include <iostream>
#include <string>
#define LEVEL3_ENEMY_COUNT 2
bool killed3 = false;
bool minus3 = true;
bool restart3 = true;
unsigned int level3_data[] = {
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    20, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 2, 20,
    20, 0, 0, 0, 0, 2, 2, 0, 3, 0, 0, 3, 2, 2, 2, 2, 2, 3, 3, 3, 20,
    20, 0, 0, 0, 0, 3, 3, 0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 20,
    20, 2, 2, 2, 2, 3, 3, 0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 20,
};
GLuint fontTextureID3;
void Level3::Initialize() {
    livest = 3;
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 14, 7);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.2f,-6.1f,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2;
    state.player->textureID = Util::LoadTexture("player.png");
    
    state.player->animLeft = new int[4] {4,5,6,7};
    state.player->animRight = new int[4] {8,9,10,11};
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.2f;
    
    state.player->jumpPower = 7.0f;
    

    
    
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("Goblin.png");
    for(int i = 0; i<LEVEL3_ENEMY_COUNT;i++){
        state.enemies[i].animLeft = new int[5] {5,6,7,8,9};
        state.enemies[i].animRight = new int[5] {10,11,12,13,14};
        state.enemies[i].animFrames = 5;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 5;
        state.enemies[i].animRows = 4;
        state.enemies[i].height = 0.8f;
        state.enemies[i].width = 0.2f;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 1;
        state.enemies[i].aiType = WAITANDGO;
        state.enemies[i].aiState = IDLE;
        
    }
    state.enemies[0].animIndices = state.enemies[0].animRight;
    state.enemies[1].animIndices = state.enemies[1].animLeft;
    state.enemies[0].position = glm::vec3(4.0f,-6,0);
    state.enemies[1].position = glm::vec3(13.7, -4,0);
    fontTextureID3 = Util::LoadTexture("font1.png");
    
}
void Level3::Update(float deltaTime) {
    if(restart3){
        state.player->position = glm::vec3(1.3f,-5,0);

        state.enemies[0].position = glm::vec3(4.0f,-6,0);
        state.enemies[1].position =glm::vec3(13.7,-4,0);
        minus3 = true;
        restart3 = false;
        for(int i = 0; i<LEVEL3_ENEMY_COUNT;i++){
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
    }
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    for(int i = 0; i<LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.player,LEVEL3_ENEMY_COUNT, state.map);
       
        if(state.player->CheckCollision(&state.enemies[i])||state.player->position.y<-9){
            if(minus3 && livest>0){
                livest--;
                minus3 = false;
                restart3 = true;
            }
            
        }
    }
    std::cout << "playerx" << state.player->position.x << std::endl;
    std::cout << "playery" << state.player->position.y << std::endl;
    if(state.enemies[1].position.x<12){
        state.enemies[1]. movement = glm::vec3(1,0,0);
    }
    if(state.player->position.x>=20){
        state.nextScene  = 3 ;
    }
}
void Level3::Render(ShaderProgram *program) {
    
    state.map->Render(program);
    state.player->Render(program);
    if(start == false && livest>0){
        Util::DrawText(program, fontTextureID3, "Level3", 1.0f, -0.5f, glm::vec3(state.player->position.x, -1, 0));
    }
    Util::DrawText(program, fontTextureID3, "Lives:"+std::to_string(livest), 0.75f, -0.5f, glm::vec3(state.player->position.x-1, state.player->position.y+1, 0));
    state.enemies[0].Render(program);
    state.enemies[1].Render(program);
}
