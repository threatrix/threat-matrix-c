#ifndef AKARMI_FLOW_ABSTRACT_PUSH_SUPPLIER_HH_INCLUDED
#define AKARMI_FLOW_ABSTRACT_PUSH_SUPPLIER_HH_INCLUDED

#include <akarmi_flow/abstract_push_channel.hh>
#include <akarmi_flow/typecheck.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename PUSH_CHANNEL,typename ID>
		class abstract_push_supplier
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
				virtual void connect_push_channel(
					PUSH_CHANNEL & ln,
					const ID & id) = 0;

				virtual inline ~abstract_push_supplier() {}
		};

		template <typename PUSH_CHANNEL>
		class abstract_push_supplier<PUSH_CHANNEL,void>
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
				virtual void connect_push_channel(PUSH_CHANNEL & ln) = 0;
				virtual inline ~abstract_push_supplier() {}
		};
	};
};

#endif // AKARMI_FLOW_ABSTRACT_PUSH_SUPPLIER_HH_INCLUDED

// $Id: abstract_push_supplier.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
