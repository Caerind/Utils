#ifndef EVENT_PRIV_VEVENT_HPP
#define EVENT_PRIV_VEVENT_HPP

/**
 * \brief The namespace that contain all the event functionalities
 */
namespace event
{
    class VEmitter;
    /**
     * \brief A common virtual class for all Event.
     * Use only for the id, and for default callbacks
     */
    class VEvent
    {
        public:
            VEvent(const VEvent&) = default;
            VEvent& operator=(const VEvent&) = default;

            virtual ~VEvent() = default;

        protected:
            static unsigned int _familyCounter; //< used in subclass to generate unique ID

            VEvent() = default;

        private:
            friend class VEmitter;

            virtual bool _dispatch(const VEmitter& emitter) const = 0; //< called by VEmitter to get te real type on the event
    };

}

#endif
