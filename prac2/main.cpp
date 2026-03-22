#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "mat3.h"
#include "src/window.h"
#include "src/scene.h"
#include "src/game.h"

using namespace std;

int main(int argc, char* argv[]) {
////////////////////////////////////////////////////////////////////////////////////////
//Command-line arguments - used to import from a specific json file
////////////////////////////////////////////////////////////////////////////////////////
    string importFile = "";
    for (int i = 1; i < argc - 1; i++)
        if (string(argv[i]) == "--import")
            importFile = argv[i + 1];


    GLFWwindow* window = setUp();

    GLuint shaderID  = LoadShaders("vertex.glsl", "fragment.glsl");
    GLuint modelLoc  = glGetUniformLocation(shaderID, "model");
    GLuint projLoc   = glGetUniformLocation(shaderID, "projection");
    GLuint colourLoc = glGetUniformLocation(shaderID, "colour");

    float projData[9];
    flattenMatrix(identity3(), projData);

    LineFont font;
    font.init(shaderID, colourLoc, modelLoc, projLoc);


    Scene sc;
    sc.init();

    GameState gs;
    gs.initBridgeConstants(sc.rampTilt);
////////////////////////////////////////////////////////////////////////////////////////
//actualy does the importing calling relevant functions
////////////////////////////////////////////////////////////////////////////////////////
    if (!importFile.empty()) {
        auto spawnTri  = [&](float x, float y){ return sc.spawnTri(x, y);  };
        auto spawnRect = [&](float x, float y){ return sc.spawnRect(x, y); };
        if (importLayout(importFile.c_str(), sc.ball, sc.hole,
                         sc.triObs, sc.rectObs, gs.bridgeOpen,
                         spawnTri, spawnRect))
            cout << "Layout imported from: " << importFile << "\n";
        else
            cerr << "Failed to import layout from: " << importFile << "\n";
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

////////////////////////////////////////////////////////////////////////////////////////
//game loop running infinitely 
////////////////////////////////////////////////////////////////////////////////////////

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

        gs.handleKeys(window, sc, now, dt);
        gs.updateBridge(sc, dt);
        gs.updateCollision(sc);
        gs.syncSpots(sc);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderID);
        glUniformMatrix3fv(projLoc, 1, GL_TRUE, projData);

        for (RenderShape* s : sc.buildDrawList())
            s->draw(modelLoc, colourLoc, gs.wireframeMode);

        gs.drawHUD(font, projData);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
