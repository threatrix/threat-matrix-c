#ifndef AKARMI_FLOW_ABSTRACT_PULL_SUPPLIER_HH_INCLUDED
#define AKARMI_FLOW_ABSTRACT_PULL_SUPPLIER_HH_INCLUDED

#include <akarmi_flow/abstract_pull_channel.hh>
#include <akarmi_flow/typecheck.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename PULL_CHANNEL,typename ID>
		class abstract_pull_supplier
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
				virtual PULL_CHANNEL & obtain_pull_channel(const ID & id) = 0;
				virtual inline ~abstract_pull_supplier() {}
		};

		template <typename PULL_CHANNEL>
		class abstract_pull_supplier<PULL_CHANNEL,void>
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
				virtual PULL_CHANNEL & obtain_pull_channel(void) = 0;
				virtual ~abstract_pull_supplier() {}
		};
	};
};

#endif // AKARMI_FLOW_ABSTRACT_PULL_SUPPLIER_HH_INCLUDED

// $Id: abstract_pull_supplier.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
