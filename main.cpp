#include <iostream>

#include <pigeon.h>

#include <thread>

struct Notification
{
    Notification() = default;

    std::string mem;
};

struct Notif
{
    Notif() = default;

    std::string mem;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    Pigeon<Notification>::const_handler_t func = [](const Notification& i_notif)
    {
        std::cout << i_notif.mem /*<< std::endl*/;
    };

    const auto p_subscription = Pigeon<Notification>::Subscribe(func);
    const auto p_sub = Pigeon<Notif>::Subscribe([](const Notif& i_notif)
    {
        std::cout << i_notif.mem /*<< std::endl*/;
    });

    //    argc = 2;
    if (argc != 2)
    {
        std::cout << "pass a number" << std::endl;
        return 0;
    }

    //    const std::string count = "5";// argv[1];
    const std::string count = argv[1];
    int num = 0;
    try
    {
        num = std::stoi(count);
    }
    catch(const std::invalid_argument& i_invalid)
    {
        std::cerr << "invalid argument " << i_invalid.what() << std::endl;
    }

    std::thread thread([argv, count]()
    {
        const int num = std::stoi(count);
        for (int i = 0; i < num; ++i)
        {
            Notif notif;
            notif.mem = "pewpew_" + std::to_string(i);
            notif.mem.append("\n");
            Pigeon<Notif>::Notify(notif);
        }
    });

    for (int i = 0; i < num; ++i)
    {
        Notification notif;
        notif.mem = "woohoo_" + std::to_string(i);
        notif.mem.append("\n");
        Pigeon<Notification>::Notify(notif);
    }

    if (thread.joinable())
        thread.join();

    return 0;
}
