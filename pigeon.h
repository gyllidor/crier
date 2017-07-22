#ifndef PIGEON_H
#define PIGEON_H

#include "notifier.h"

#include <type_traits>

template<typename TNotif>
class Pigeon
{
#define ENABLER template <typename T = typename std::enable_if<std::is_same<TNotif, notif_t>::value>::type>

public:
    using notif_t = typename notifier::Typo<TNotif>::notifi_t;
    using const_ref_notif_t = typename notifier::Typo<TNotif>::const_ref_notif_t;
    using const_ref_handler_t = typename notifier::Typo<TNotif>::const_ref_handler_t;
    using const_handler_t = typename notifier::Typo<TNotif>::const_handler_t;

public:
    ENABLER static std::unique_ptr<notifier::ISubscription> Subscribe(const_ref_handler_t i_handler)
    {
        return notifier::Cage<notif_t>::GetInstance().Subscribe(i_handler);
    }

    ENABLER static void Notify(const_ref_notif_t i_notification)
    {
        notifier::Cage<notif_t>::GetInstance().Notify(i_notification);
    }
};

#endif // PIGEON_H
