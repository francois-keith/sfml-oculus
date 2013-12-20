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

#include "OVRWrapper.h"
#include "PLYMesh.h"
#include "SFMLScreen.h"

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

void render(sf::RenderWindow & app, SFMLScreen & sfmlScreen, glm::mat4 & model_screen, PLYMesh & box, glm::mat4& model_box, glm::mat4 & anim_box, glm::mat4 & projection, glm::mat4 & view, OVRWrapper & oculus, OVR::Util::Render::StereoEye eye)
{
    oculus.setEye(eye);
    glm::mat4 viewAdjust = oculus.getViewAdjust();
    glm::mat4 mvp_box = projection*viewAdjust*view*model_box*anim_box;
    glm::mat4 mvp_screen = viewAdjust*model_screen;
    app.clear();
    sfmlScreen.render(mvp_screen);
    glClear(GL_DEPTH_BUFFER_BIT);
    box.render(mvp_box);
}

int main(int argc, char * argv[])
{
    bool render_for_oculus = true;
    if(argc > 1)
    {
        std::stringstream ss;
        ss << argv[1]; ss >> render_for_oculus;
    }
    unsigned int width = 1280; unsigned int height = 800;
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
    sf::RenderWindow app(sf::VideoMode(width, height), "OpenGL 2.0", sf::Style::Close, contextSettings);
    if(app.getSize().x != width || app.getSize().y != height)
    {
        std::cerr << "[Warning] Requested " << width << "x" << height <<  " window." << std::endl;
        width = app.getSize().x; height = app.getSize().y;
        std::cerr << "[Warning] Got " << width << "x" << height <<  " window." << std::endl;
        std::cerr << "[Warning] Continue as if nothing happened but be aware of that in case of weird behaviours" << std::endl;
    }
    app.setActive();
//    app.setVerticalSyncEnabled(true);

    OVRWrapper oculus(width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    sf::Clock clock;
    sf::Clock anim_clock;

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    sf::Text text;
    text.setString("FPS: 0");
    text.setFont(font);
    text.setPosition(10,10);
    text.setCharacterSize(24);
    text.setColor(sf::Color::Red);

    SFMLScreen sfmlScreen;
    if(render_for_oculus)
    {
        sfmlScreen.init(width/2, 480, width/2, height);
    }
    else
    {
        sfmlScreen.init(width/2, 480, width, height);
    }

    PLYMesh box;
    box.loadFromFile("models/can.ply");

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
        }

        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(60.0f, 1.0f*(width)/height, 0.1f, 10.0f);

        static float angle = 0;
        glm::mat4 anim_box = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
        angle += 90*anim_clock.getElapsedTime().asMicroseconds()/1e6;
        anim_clock.restart();
        glm::mat4 model_box = glm::translate(glm::mat4(1.0f), glm::vec3(0., 0.0, 2.));
        if(render_for_oculus)
        {
            projection = glm::perspective(60.0f, 1.0f*(width/2)/height, 0.1f, 10.0f);
        }
        glm::mat4 mvp_box = projection*view*model_box*anim_box;

        glm::mat4 model_screen = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
        glm::mat4 mvp_screen = model_screen;

        /* Draw stuff to the SFML inner-screen */
        /* SFML drawings from here */
        sfmlScreen.clear(sf::Color::White);
        sf::RectangleShape border(sf::Vector2f(sfmlScreen.getSize().x - 10, sfmlScreen.getSize().y - 10));
        border.setPosition(5,5);
        border.setFillColor(sf::Color(255,255,255,0));
        border.setOutlineThickness(5);
        border.setOutlineColor(sf::Color(0,0,255,255));
        sfmlScreen.draw(border);
        sf::RectangleShape center(sf::Vector2f(10,10));
        center.setPosition(sfmlScreen.getSize().x/2, sfmlScreen.getSize().y/2),
        center.setFillColor(sf::Color(255,0,0,255));
        sfmlScreen.draw(center);
        sfmlScreen.draw(text);
        sfmlScreen.display();
        /* End of SFML drawings */
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        app.clear();
        glClear(GL_DEPTH_BUFFER_BIT);

        if(!render_for_oculus)
        {
            sfmlScreen.render(mvp_screen);
            glClear(GL_DEPTH_BUFFER_BIT);
            box.render(mvp_box);
        }
        else
        {
            /* Render left eye */
            {
                render(app, sfmlScreen, model_screen, box, model_box, anim_box, projection, view, oculus, OVR::Util::Render::StereoEye_Left);
            }
            /* Render right eye */
            {
                render(app, sfmlScreen, model_screen, box, model_box, anim_box, projection, view, oculus, OVR::Util::Render::StereoEye_Right);
            }
        }

        app.display();
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
