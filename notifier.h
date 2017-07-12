#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <type_traits>

namespace noti
{
    class IWraphandler
    {
    public:
        virtual ~IWraphandler() = 0;
    };

    inline IWraphandler::~IWraphandler() {}

    template <typename TNotif>
    class WrapHandler : public IWraphandler
    {
        using type = typename std::decay<TNotif>::type;

    public:
        WrapHandler() = default;
        WrapHandler(const std::function<void(const type&)>& i_notif_handler)
            : m_notif_handler(i_notif_handler)
        {
        }

        void Do(const type& i_notification)
        {
            m_notif_handler(i_notification);
        }

    private:
        const std::function<void(const type&)> m_notif_handler;
    };

    template <typename TNotif>
    class NotifHandlerContainer;

    template <typename TNotif>
    class Subscription
    {
        using type = typename std::decay<TNotif>::type;

    public:
        Subscription() = default;
        ~Subscription()
        {
            NotifHandlerContainer<type>::GetInstance().RemoveSubscriber(this);
        }
    };

    template <typename TNotif>
    class NotifHandlerContainer
    {
        using type = typename std::decay<TNotif>::type;

        friend Subscription<type>;

    public:
        using subscription_t = Subscription<type>;

    public:
        static NotifHandlerContainer& GetInstance()
        {
            static NotifHandlerContainer noti;
            return noti;
        }

        std::unique_ptr<subscription_t> Subscribe(const std::function<void(const type&)>& i_notif_handler)
        {
            auto p_subscription = std::make_unique<subscription_t>();
            m_handlers[p_subscription.get()] = std::make_unique<WrapHandler<type>>(i_notif_handler);
            return p_subscription;
        }

        void Notify(const type& i_notification)
        {
            for (const auto& pair : m_handlers)
            {
                static_cast<WrapHandler<type>*>(pair.second.get())->Do(i_notification);
            }
        }

    private:
        void RemoveSubscriber(subscription_t* ip_notif_handler)
        {
            m_handlers.erase(ip_notif_handler);
        }

    private:
        NotifHandlerContainer() = default;
        NotifHandlerContainer(const NotifHandlerContainer&) = delete;
        NotifHandlerContainer(NotifHandlerContainer&&) = delete;
        const NotifHandlerContainer& operator =(const NotifHandlerContainer&) = delete;
        const NotifHandlerContainer& operator =(NotifHandlerContainer&&) = delete;

    private:
        std::map<subscription_t*, std::unique_ptr<IWraphandler>> m_handlers;
    };

    template <typename TNotif>
    auto Subscribe(const std::function<void(const TNotif&)>& i_notif_handler) -> std::unique_ptr<Subscription<TNotif>>
    {
        return NotifHandlerContainer<TNotif>::GetInstance().Subscribe(i_notif_handler);
    }

    template <typename TNotif>
    void Notify(const TNotif& i_notif)
    {
        NotifHandlerContainer<TNotif>::GetInstance().Notify(i_notif);
    }
}

#endif // NOTIFIER_H
