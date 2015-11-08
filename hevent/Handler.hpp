#ifndef EVENT_VHANDLER_HPP
#define EVENT_VHANDLER_HPP

#include <unordered_map>
#include <list>
#include <memory>

namespace event
{
	class VEmitter;
	class VEvent;

	/**
	 * \brief This class receive event from Emitter
	 */
	class Handler
	{
		public:
			Handler(const Handler&) = delete;
			Handler& operator=(const Handler&) = delete;

			/**
			 * \brief Default constructor
			 */
			Handler();

			/**
			 * \param The default callback to use whene an event is receive
			 * */
			Handler(const std::function<void(const VEvent& event)>& defaultCallback);

			virtual ~Handler();

			/**
			 * \brief set the callback to use whene an event of type T is receive
			 * \param callback the callback to call
			 * */
			template <typename T>
			void bind(const std::function<void(const T&)>& callback);

			/**
			 * \brief the Handler will receive event from the emitter
			 * \param emitter to connect with
			 * The callback use is the one set with bind() or the default one if avalible
			 * */
			void connect(VEmitter& emitter);

			/**
			 * \brief set a call callback to use for a specifique event from one emitter
			 * \param T the event type to use
			 * \param emitter the emitter to connect with
			 * \param callback the callback to use when emitter send a event of type T
			 * */
			template <typename T>
			void connect(VEmitter& emitter,const std::function<void(const T&)>& callback);

			/**
			 * \brief forget the emitter
			 * */
			void disconnect(VEmitter& emitter);

		private:
			friend class VEmitter;

			/**
			 * \brief polymorphique callback
			 * */
			class VFunction
			{
				public:
					VFunction();
					virtual ~VFunction();
					virtual void exec(const VEvent& event)const = 0;  
			};

			template <typename T>
			class Function : public VFunction
			{
				public :
					Function(const std::function<void(const T&)>& callback);
					virtual ~Function();

					virtual void exec(const VEvent& event)const override;
				private:
					std::function<void(const T&)> _callback;
			};


			/**
			 * \brief used by Emitter to trigger an event
			 * */
			template <typename T>
			void _receive(const VEmitter& emitter,const T& event) const;

			/**
			 * \brief call on the destructor of Emitter
			 * */
			void _unregister(const VEmitter* emitter);

			std::function<void(const VEvent&)> _defaultCallback;
			std::unordered_map<unsigned int,std::unique_ptr<VFunction>> _callbacks;
			std::unordered_map<VEmitter*,std::unordered_map<unsigned int,std::unique_ptr<VFunction>>> _callbacksByEmitter;

			std::list<VEmitter*> _emitters;
	};
}
	
#include "Handler.tpl"
#endif
