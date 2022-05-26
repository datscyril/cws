#include "events/event-loop.hh"

#include "events/register.hh"

namespace http
{
    static void sigint_cb(struct ev_loop *loop, ev_signal *, int)
    {
        std::cout << "\n=== Gracefull Shutdown ===\n";
        ev_break(loop, EVBREAK_ALL);
        auto registry = http::event_register.event_get();
        for (auto event : registry)
        {
            http::event_register.unregister_ew(event.first);
        }
    }

    EventLoop::EventLoop()
        : loop(EV_DEFAULT)
    {}

    EventLoop::EventLoop(struct ev_loop *lo)
        : loop(lo)
    {}

    EventLoop::~EventLoop()
    {
        ev_loop_destroy(loop);
    }

    void EventLoop::register_watcher(EventWatcher *ew)
    {
        ev_io_start(loop, &(ew->watcher_get()));
    }

    void EventLoop::unregister_watcher(EventWatcher *ew)
    {
        ev_io_stop(loop, &(ew->watcher_get()));
    }

    void EventLoop::register_sigint_watcher(ev_signal *es) const
    {
        ev_signal_init(es, sigint_cb, SIGINT);
        ev_signal_start(loop, es);
    }

    void EventLoop::operator()() const
    {
        ev_run(loop, 0);
    }

} // namespace http
