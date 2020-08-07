#include "Menu.h"
GLuint fontTextureIDm;

void Menu::Initialize() {
    state.nextScene = -1;
    fontTextureIDm = Util::LoadTexture("font1.png");
    livest = 3;
   
   
    
}
void Menu::Update(float deltaTime) {
   }
void Menu::Render(ShaderProgram *program) {
    Util::DrawText(program, fontTextureIDm, "Beat Game", 1.5f, -0.5f, glm::vec3(-4, 2, 0));
    Util::DrawText(program, fontTextureIDm, "Press enter to start", 0.9f, -0.5f, glm::vec3(-4, 0, 0));
   
}
