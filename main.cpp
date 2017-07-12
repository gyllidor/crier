#include <iostream>

//#include <crier.h>
#include <notifier.h>

#include <unistd.h>

struct Notification
{
    Notification() = default;

    std::string mem;
};

int main(int argc, char** argv)
{
    const auto subscription = noti::Subscribe<Notification>([](const Notification& i_notif)
    {
        std::cout << i_notif.mem << std::endl;
    });

    if (argc != 2)
    {
        std::cout << "pass a number" << std::endl;
        return 0;
    }

    for (int i = 0; i < std::stoi(argv[1]); ++i)
    {
        Notification notif;
        notif.mem = "woohoo_" + std::to_string(i);
        noti::Notify(notif);
    }

    return 0;
}
