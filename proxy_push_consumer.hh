#ifndef AKARMI_FLOW_PROXY_PUSH_CONSUMER_HH_INCLUDED
#define AKARMI_FLOW_PROXY_PUSH_CONSUMER_HH_INCLUDED

#include <akarmi_flow/abstract_push_consumer.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename IMPL, typename PUSH_CHANNEL,typename ID>
		class proxy_push_consumer :
			virtual public IMPL,
			virtual public abstract_push_consumer<PUSH_CHANNEL,ID>
		{
			public:
				typedef PUSH_CHANNEL push_channel_t;

			private:
				typedef typename PUSH_CHANNEL::retval_t ch_retval_t;
				typedef typename PUSH_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PUSH_CHANNEL,
					abstract_push_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_generic_channel;

			public:
				inline PUSH_CHANNEL & obtain_push_channel(const ID & id)
				{
					return IMPL::obtain_push_channel(id);
				}

				virtual inline ~proxy_push_consumer() {}

				inline proxy_push_consumer() :
					IMPL(), abstract_push_consumer<PUSH_CHANNEL,ID>() {}

				template <typename ARG1>
				inline proxy_push_consumer(ARG1 arg1) :
					IMPL(arg1), abstract_push_consumer<PUSH_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2) :
					IMPL(arg1,arg2), abstract_push_consumer<PUSH_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2,typename ARG3>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
					IMPL(arg1,arg2,arg3), abstract_push_consumer<PUSH_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
					IMPL(arg1,arg2,arg3,arg4), abstract_push_consumer<PUSH_CHANNEL,ID>() {}
		};

		template <typename IMPL, typename PUSH_CHANNEL>
		class proxy_push_consumer<IMPL,PUSH_CHANNEL,void> :
			virtual public IMPL,
			virtual public abstract_push_consumer<PUSH_CHANNEL,void>
		{
			public:
				typedef PUSH_CHANNEL push_channel_t;

			private:
				typedef typename PUSH_CHANNEL::retval_t ch_retval_t;
				typedef typename PUSH_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PUSH_CHANNEL,
					abstract_push_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_generic_channel;

			public:
				inline PUSH_CHANNEL & obtain_push_channel(void)
				{
					return IMPL::obtain_push_channel();
				}

				virtual inline ~proxy_push_consumer() {}

				inline proxy_push_consumer() :
					IMPL(), abstract_push_consumer<PUSH_CHANNEL,void>() {}

				template <typename ARG1>
				inline proxy_push_consumer(ARG1 arg1) :
					IMPL(arg1), abstract_push_consumer<PUSH_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2) :
					IMPL(arg1,arg2), abstract_push_consumer<PUSH_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2,typename ARG3>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
					IMPL(arg1,arg2,arg3), abstract_push_consumer<PUSH_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
				inline proxy_push_consumer(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
					IMPL(arg1,arg2,arg3,arg4), abstract_push_consumer<PUSH_CHANNEL,void>() {}
		};
	};
};

#endif // AKARMI_FLOW_PROXY_PUSH_CONSUMER_HH_INCLUDED

// $Id: proxy_push_consumer.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $

