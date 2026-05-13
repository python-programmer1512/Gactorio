// Gactorio - Energy Drink Factory Simulator
// SDL2/OpenGL/ImGui 부트스트랩은 Application 클래스 안에 캡슐화되어 있다.
// main은 MVC 객체를 조립하고 run() 호출만 한다.

#include "models/Factory.h"
#include "controllers/FactoryController.h"
#include "views/Application.h"
#include "views/FactoryPanel.h"

#include <memory>

int main(int /*argc*/, char** /*argv*/) {
    Application app("Gactorio - Energy Drink Factory", 1280, 720);

    Factory           factory;
    FactoryController ctrl(factory);

    app.addPanel(std::make_unique<FactoryPanel>(ctrl));

    app.run();
    return 0;
}
