#include "Application.hpp"
#include "BaseState.hpp"
#include "DebugView.hpp"

#include "States/GameState.hpp"
#include "States/MenuState.hpp"

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Window/Event.hpp>
#include <gsl/gsl_util>

#include <chrono>
#include <thread>

Application::Application()
    : m_total{}
{
    sf::ContextSettings ctx;
    ctx.antialiasingLevel = 1;
    m_window.create(sf::VideoMode(1366, 768), "LD45", sf::Style::Default, ctx);
    // m_window.setVerticalSyncEnabled(true);
    m_defaultView = m_window.getDefaultView();

    m_resourceManager.addResource<sf::Shader>("Atmosphere", "data/Atmosphere.frag", sf::Shader::Fragment);
    m_resourceManager.addResource<sf::Shader>("Corona", "data/Corona.frag", sf::Shader::Fragment);
    m_resourceManager.addResource<sf::Shader>("Orbit", "data/Orbit.frag", sf::Shader::Fragment);
}

Application::~Application()
{
}

void Application::run()
{
    m_stateManager.init(this);

    m_stateManager.pushState(std::make_unique<States::MenuState>());

    using clock = std::chrono::high_resolution_clock;
    constexpr std::chrono::nanoseconds ticklength(1000000000 / kTickRate);
    constexpr std::chrono::duration<float> ticklength_s(ticklength);
    constexpr std::chrono::milliseconds targetDt(kTargetDt);

    std::chrono::nanoseconds accumulator{};
    auto lastFrame = clock::now();
    sf::Event ev;

    DebugView debug;
    debug.init(*this);

    while (m_window.isOpen())
    {
        debug.startFrame();

        auto dt = clock::now() - lastFrame;
        lastFrame = clock::now();
        m_total += dt;

        std::chrono::duration<float> dt_s(dt);

        accumulator += std::chrono::nanoseconds(dt);

        auto* curState = m_stateManager.getCurrent();
        while (m_window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case sf::Event::Closed:
                m_window.close(); break;

            case sf::Event::Resized:
                {
                    sf::Vector2f size(ev.size.width, ev.size.height);

                    m_defaultView.setSize(size);
                    m_defaultView.setCenter(size / 2.f);
                } break;

            default:
                break;
            }

            if (GSL_LIKELY(curState))
                curState->handleEvent(ev);
        }

        // Dynamic update - every frame
        if (GSL_LIKELY(curState))
            curState->update(dt_s.count());

        while (accumulator >= ticklength)
        {
            debug.startUpdate();

            // Fixed update - N times per second
            if (GSL_LIKELY(curState))
                curState->fixedUpdate(ticklength_s.count());

            accumulator -= ticklength;

            debug.endUpdate();
        }

        const auto alpha = (float)accumulator.count() / ticklength.count();

        curState = m_stateManager.getCurrent();
        m_window.clear();
        m_window.setView(m_defaultView);

        if (GSL_LIKELY(curState))
            curState->render(alpha);

        m_window.draw(debug);

        m_window.display();

        m_stateManager.update();
        debug.endFrame();

        // Sleep before next frame
        if (GSL_LIKELY(dt < targetDt))
            std::this_thread::sleep_for(targetDt - dt);
    }
}

void Application::stop()
{
    m_window.close();
}
