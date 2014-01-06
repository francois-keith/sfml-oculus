#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OculusWindow.h"

#include "PLYMesh.h"

#include <OVR.h>

inline std::ostream & operator<<(std::ostream & os, const OVR::Util::Render::StereoEye & eye)
{
    switch(eye)
    {
        case OVR::Util::Render::StereoEye_Center:
            os << "center eye";
            break;
        case OVR::Util::Render::StereoEye_Left:
            os << "left eye";
            break;
        case OVR::Util::Render::StereoEye_Right:
            os << "right eye";
            break;
    }
    return os;
}

inline std::ostream & operator<<(std::ostream & os, const glm::mat4 & m)
{
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            os << m[i][j] << ", ";
        }
        os << std::endl;
    }
    return os;
}

inline std::ostream & operator<<(std::ostream & os, const OVR::Util::Render::DistortionConfig & d)
{
    os << "K " << d.K[0] << ", " << d.K[1] << ", " << d.K[2] << ", " << d.K[3] << std::endl;
    os << "Offset (X,Y): " << d.XCenterOffset << ", " << d.YCenterOffset << std::endl;
    os << "Scale: " << d.Scale << std::endl;
    return os;
}

int main(int argc, char * argv[])
{
    unsigned int width = 1280; unsigned int height = 800;
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
    OculusWindow window(sf::VideoMode(width, height), "Oculus window for SFML", sf::Style::Close, contextSettings);

    sf::Clock clock;

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    sf::Text text;
    text.setString("FPS: 0");
    text.setFont(font);
    text.setPosition(10,10);
    text.setCharacterSize(40);
    text.setColor(sf::Color::Red);

    sf::Texture bgTexture; bgTexture.loadFromFile("bg/background.png");
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(window.getRenderScale(), window.getRenderScale());

    sf::Window & app = window.getApplication();
    sf::RenderTarget & target = window.getRenderTarget();

    bool quit = false;
    unsigned int frameC = 0;
    while(!quit)
    {
        sf::Event event;
        while(app.pollEvent(event))
        {
            if(event.type == sf::Event::Closed ||
                ( event.type == sf::Event::KeyPressed && ( event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q ) ))
            {
                quit = true;
            }
            if(event.type == sf::Event::Closed ||
                ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up ))
            {

                window.setScreenModel(glm::translate(window.getScreenModel(), glm::vec3(0,0,-0.1)));
            }
            if(event.type == sf::Event::Closed ||
                ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down ))
            {
                window.setScreenModel(glm::translate(window.getScreenModel(), glm::vec3(0,0,0.1)));
            }
        }

        /* Draw stuff to the SFML inner-screen */
        /* SFML drawings from here */
        target.clear(sf::Color::White);
        target.draw(bgSprite);
        sf::RectangleShape border(sf::Vector2f(target.getSize().x - 10, target.getSize().y - 10));
        border.setPosition(5,5);
        border.setFillColor(sf::Color(255,255,255,0));
        border.setOutlineThickness(5);
        border.setOutlineColor(sf::Color(0,0,255,255));
        target.draw(border);
        sf::RectangleShape center(sf::Vector2f(10,10));
        center.setPosition(target.getSize().x/2, target.getSize().y/2),
        center.setFillColor(sf::Color(255,0,0,255));
        target.draw(center);
        target.draw(text);
        /* End of SFML drawings */

        window.show();

        frameC++;
        if(frameC == 100)
        {
            std::stringstream ss;
            ss << "FPS: " << (100*1e6/clock.getElapsedTime().asMicroseconds());
            text.setString(ss.str());
            clock.restart();
            frameC = 0;
        }

    }

    return 0;
}