#ifndef AKARMI_FLOW_ABSTRACT_PUSH_CHANNEL_HH_INCLUDED
#define AKARMI_FLOW_ABSTRACT_PUSH_CHANNEL_HH_INCLUDED

namespace akarmi
{
	namespace flow
	{
		template <typename RETVAL,typename EVENT>
		class abstract_push_channel
		{
			public:
				typedef RETVAL retval_t;
				typedef EVENT event_t;

				virtual RETVAL push(const EVENT & e) = 0;
				virtual inline ~abstract_push_channel() {}
		};

		template <typename EVENT>
		class abstract_push_channel<void,EVENT>
		{
			public:
				typedef void retval_t;
				typedef EVENT event_t;

				virtual void push(const EVENT & e) = 0;
				virtual inline ~abstract_push_channel() {}
		};

		template <typename RETVAL>
		class abstract_push_channel<RETVAL,void>
		{
			public:
				typedef RETVAL retval_t;
				typedef void event_t;

				virtual RETVAL push(void) = 0;
				virtual inline ~abstract_push_channel() {}
		};

		template <>
		class abstract_push_channel<void,void>
		{
			public:
				typedef void retval_t;
				typedef void event_t;

				virtual void push(void) = 0;
				virtual inline ~abstract_push_channel() {}
		};
	};
};

#endif // AKARMI_FLOW_ABSTRACT_PUSH_CHANNEL_HH_INCLUDED

// $Id: abstract_push_channel.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
