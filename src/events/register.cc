#include "events/register.hh"

#include <iostream>

#include "events/event-loop.hh"

namespace http
{
    EventWatcherRegistry event_register;

    bool EventWatcherRegistry::unregister_ew(EventWatcher *ew)
    {
        for (auto i = events_.begin(); i != events_.end(); i++)
            if (i->first == ew)
            {
                loop_.unregister_watcher(ew);
                i = events_.erase(i);
                return true;
            }
        return false;
    }

    std::optional<std::shared_ptr<EventWatcher>>
    EventWatcherRegistry::at(EventWatcher *ew)
    {
        try
        {
            return events_.at(ew);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    EventLoop &EventWatcherRegistry::loop_get()
    {
        return loop_;
    }
    std::unordered_map<EventWatcher *, std::shared_ptr<EventWatcher>> &
    EventWatcherRegistry::event_get()
    {
        return events_;
    }
} // namespace http
