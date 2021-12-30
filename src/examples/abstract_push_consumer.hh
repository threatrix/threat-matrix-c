#ifndef AKARMI_FLOW_ABSTRACT_PUSH_CONSUMER_HH_INCLUDED
#define AKARMI_FLOW_ABSTRACT_PUSH_CONSUMER_HH_INCLUDED

#include <akarmi_flow/abstract_push_channel.hh>
#include <akarmi_flow/typecheck.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename PUSH_CHANNEL,typename ID>
		class abstract_push_consumer
		{
			public:
				typedef PUSH_CHANNEL push_channel_t;

			private:
				typedef typename PUSH_CHANNEL::retval_t ch_retval_t;
				typedef typename PUSH_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PUSH_CHANNEL,
					abstract_push_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_abstract_channel;

			public:
				virtual PUSH_CHANNEL & obtain_push_channel(const ID & id) = 0;
				virtual inline ~abstract_push_consumer() {}
		};

		template <typename PUSH_CHANNEL>
		class abstract_push_consumer<PUSH_CHANNEL,void>
		{
			public:
				typedef PUSH_CHANNEL push_channel_t;

			private:
				typedef typename PUSH_CHANNEL::retval_t ch_retval_t;
				typedef typename PUSH_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PUSH_CHANNEL,
					abstract_push_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_abstract_channel;

			public:
				virtual PUSH_CHANNEL & obtain_push_channel(void) = 0;
				virtual inline ~abstract_push_consumer() {}
		};
	};
};

#endif // AKARMI_FLOW_ABSTRACT_PUSH_CONSUMER_HH_INCLUDED

// $Id: abstract_push_consumer.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
