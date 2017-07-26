#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <memory>
#include <set>
#include <mutex>
#include <atomic>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename TNotif>
class Pigeon;

//----------------------------------------------------------------------------------------------------------------------
namespace notifier
{
struct ISubscription
{
    virtual ~ISubscription() = 0;
    virtual void Mute() = 0;
    virtual void UnMute() = 0;
    virtual bool IsMuted() const = 0;
};

inline ISubscription::~ISubscription() {}

//----------------------------------------------------------------------------------------------------------------------
template <typename TNotif>
struct Cage;

//----------------------------------------------------------------------------------------------------------------------
template <typename TNotif>
struct Typo
{
    using notifi_t = typename std::decay<TNotif>::type;
    using const_ref_notif_t = const notifi_t&;
    using const_ref_handler_t = const std::function<void(const notifi_t&)>&;
    using const_handler_t = const std::function<void(const notifi_t&)>;
};

//----------------------------------------------------------------------------------------------------------------------
template <typename TNotif>
class Subscription : public ISubscription
{
    using notif_t = typename Typo<TNotif>::notifi_t;
    using const_ref_notif_t = typename Typo<TNotif>::const_ref_notif_t;
    using handler_t = typename Typo<TNotif>::const_ref_handler_t;

    friend class Cage<notif_t>;

public:
    Subscription(handler_t i_handler)
        : m_handler(i_handler)
        , m_mute(false)
    {
    }

    ~Subscription()
    {
        Cage<notif_t>::GetInstance().Remove(this);
    }

    void Mute() override
    {
        m_mute = true;
    }

    void UnMute() override
    {
        m_mute = false;
    }

    bool IsMuted() const override
    {
        return m_mute;
    }

private:
    void operator()(const_ref_notif_t i_notification) const
    {
        m_handler(i_notification);
    }

private:
    Subscription() = delete;
    Subscription(const Subscription&) = delete;
    Subscription(Subscription&&) = delete;
    const Subscription& operator=(const Subscription&) = delete;
    const Subscription& operator=(Subscription&&) = delete;

private:
    const std::function<void(const_ref_notif_t)> m_handler;
    std::atomic<bool> m_mute;
};

//----------------------------------------------------------------------------------------------------------------------
template <typename TNotif>
class Cage
{
    using notif_t = typename Typo<TNotif>::notifi_t;
    using const_ref_notif_t = typename Typo<TNotif>::const_ref_notif_t;
    using const_ref_handler_t = typename Typo<TNotif>::const_ref_handler_t;

    friend Subscription<notif_t>;
    friend Pigeon<notif_t>;

private:
    static Cage& GetInstance()
    {
        static Cage cage;
        return cage;
    }

    std::unique_ptr<ISubscription> Subscribe(const_ref_handler_t i_handler)
    {
        auto p_subscription = std::make_unique<Subscription<notif_t>>(i_handler);

        std::lock_guard<std::mutex> lock(m_mtx);
        m_subscriptions.insert(p_subscription.get());
        return p_subscription;
    }

    void Notify(const_ref_notif_t i_notification)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        for (auto p_subscription : m_subscriptions)
            if (!p_subscription->IsMuted())
                (*static_cast<const Subscription<notif_t>*>(p_subscription))(i_notification);
    }

    void Remove(ISubscription* ip_subscription)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_subscriptions.erase(ip_subscription);
    }

    Cage() = default;
    Cage(const Cage&) = delete;
    Cage(Cage&&) = delete;
    const Cage& operator=(const Cage&) = delete;
    const Cage& operator=(Cage&&) = delete;

private:
    std::set<const ISubscription*> m_subscriptions;
    std::mutex m_mtx;
};
} // namespace notifier

#endif // NOTIFIER_H
