#include <iostream>
#include <deque>
#include <boost\asio.hpp>
#include <Windows.h>
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\Window\WindowHandle.hpp>

// #define BORDERLESSMODE

int main(int argc, char *argv[])
{
    FreeConsole();
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    auto window = sf::RenderWindow(sf::VideoMode{{650, 400}}, "Twitch Chat");
    window.setPosition({screenWidth - 400, 0});
    HWND hwnd = window.getNativeHandle();

#ifdef BORDERLESSMODE
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~(WS_THICKFRAME | WS_BORDER);
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    window.setSize({screenWidth, screenHeight});
    window.setPosition({0, 0});
#endif

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED | WS_EX_TOPMOST;
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    SetLayeredWindowAttributes(
        hwnd, RGB(0, 255, 0),
        255, LWA_ALPHA | LWA_COLORKEY);

    SetWindowPos(
        hwnd, HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE);

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("irc.chat.twitch.tv", "6667");
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    boost::asio::streambuf buf;

    auto send = [&](const std::string &msg)
    {
        boost::asio::write(socket, boost::asio::buffer(msg + "\r\n"));
    };

    std::string textStr = "!Hello Chat!\n";
    textStr.reserve(8192);
    std::string msg;
    msg.reserve(512);
    std::string line;
    line.reserve(64);
    std::deque<std::string> lines;
    lines.push_back("!Hello Chat!\n");

    sf::Font font("./FiraCode-Regular.ttf");
    sf::Text text(font);

    bool logFull = 0;
    std::function<void()> readLoop;
    readLoop = [&]()
    {
        boost::asio::async_read_until(
            socket, buf, "\r\n",
            [&](boost::system::error_code ec, std::size_t)
            {
                if (ec)
                    throw boost::system::system_error(ec);

                textStr.clear();
                msg.clear();
                line.clear();

                std::istream is(&buf);
                std::getline(is, msg);

                if (msg.starts_with("PING"))
                {
                    send("PONG :tmi.twitch.tv");
                    readLoop();
                    return;
                }

                auto mpos = msg.find("PRIVMSG");
                if (mpos == std::string::npos)
                {
                    readLoop();
                    return;
                }

                auto dpos = msg.find("display-name=");
                if (dpos == std::string::npos)
                {
                    readLoop();
                    return;
                }
                dpos += 13;
                auto dend = msg.find(';', dpos);
                line.append(msg.data() + dpos, dend - dpos);

                auto msgPos = msg.find(" :", mpos + 7);
                if (msgPos == std::string::npos)
                {
                    readLoop();
                    return;
                }

                std::string_view chat(msg.c_str() + msgPos + 2);
                size_t pos = 0;
                while (pos < chat.size())
                {
                    size_t next = chat.find(' ', pos);
                    if (next == std::string_view::npos) next = chat.size();

                    std::string_view word = chat.substr(pos, next - pos);

                    if (line.size() + word.size() + 1 > 60)
                    {
                        line.push_back('\n');
                        lines.push_back(line);
                        line.clear();
                    }

                    if (!line.empty()) line.push_back(' ');
                    line.append(word);

                    pos = next + 1;
                }
                line.push_back('\n');
                lines.push_back(line);
                while (lines.size() > 20) lines.pop_front();

                window.clear(sf::Color::Green);
                for (auto& str : lines) textStr.append(str);
                text.setString(textStr);
                window.draw(text);
                window.display();
                readLoop();
            });
    };
    readLoop();

    send("CAP REQ :twitch.tv/tags twitch.tv/commands twitch.tv/membership");
    send("NICK justinfan4347");
    send("JOIN #" + std::string(argv[1]));

    text.setString(textStr);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::Magenta);
    window.clear(sf::Color::Green);
    window.draw(text);
    window.display();
    window.setFramerateLimit(5);
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) // Exits loop
                window.close();
        }
        io_context.poll();
    }

    return 0;
}