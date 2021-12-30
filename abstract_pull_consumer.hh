#ifndef AKARMI_FLOW_ABSTRACT_PULL_CONSUMER_HH_INCLUDED
#define AKARMI_FLOW_ABSTRACT_PULL_CONSUMER_HH_INCLUDED

#include <akarmi_flow/abstract_pull_channel.hh>
#include <akarmi_flow/typecheck.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename PULL_CHANNEL,typename ID>
		class abstract_pull_consumer
		{
			public:
				typedef PULL_CHANNEL pull_channel_t;

			private:
				typedef typename PULL_CHANNEL::retval_t ch_retval_t;
				typedef typename PULL_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PULL_CHANNEL,
					abstract_pull_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_abstract_channel;

			public:
				virtual void connect_pull_channel(
					PULL_CHANNEL & ln,
					const ID & id) = 0;

				virtual inline ~abstract_pull_consumer() {}
		};

		template <typename PULL_CHANNEL>
		class abstract_pull_consumer<PULL_CHANNEL,void>
		{
			public:
				typedef PULL_CHANNEL pull_channel_t;

			private:
				typedef typename PULL_CHANNEL::retval_t ch_retval_t;
				typedef typename PULL_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PULL_CHANNEL,
					abstract_pull_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_abstract_channel;

			public:
				virtual void connect_pull_channel(
					PULL_CHANNEL & ln) = 0;

				virtual inline ~abstract_pull_consumer() {}
		};
	};
};

#endif // AKARMI_FLOW_ABSTRACT_PULL_CONSUMER_HH_INCLUDED

// $Id: abstract_pull_consumer.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
