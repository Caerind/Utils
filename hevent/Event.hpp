#ifndef EVENT_EVENT_HPP
#define EVENT_EVENT_HPP

#include "VEvent.hpp"

namespace event
{
    /**
     * \brief A class that represent an Event.
     * T is the parent class template parameter.
     *
     * \code
     * class TestEvent : public event::Event<TestEvent>
     * {
     *  // some stuf ...
     * };
     * \endcode
     */
    template<typename T>
    class Event : public VEvent
    {
        public:
            Event(const Event<T>&) = default;
            Event& operator=(const Event<T>&) = default;

            Event();
            virtual ~Event();

            /**
             * \brief internal class unique id
             * \return the unique id of the event class
             */
            static unsigned int family();

        private:
            
            virtual bool _dispatch(const VEmitter& emitter) const override; //< called by VEmitter to get te real type on the event
    };
}

#include "Event.tpl"

#endif
