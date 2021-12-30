#ifndef AKARMI_FLOW_SPECIALIZED_PULL_SUPPLIER_HH_INCLUDED
#define AKARMI_FLOW_SPECIALIZED_PULL_SUPPLIER_HH_INCLUDED

#include <akarmi_flow/abstract_pull_supplier.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename IMPL, typename PULL_CHANNEL,typename ID>
		class proxy_pull_supplier :
			virtual public IMPL,
			virtual public abstract_pull_supplier<PULL_CHANNEL,ID>
		{
			public:
				typedef PULL_CHANNEL pull_channel_t;

			private:
				typedef typename PULL_CHANNEL::retval_t ch_retval_t;
				typedef typename PULL_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PULL_CHANNEL,
					abstract_pull_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_generic_channel;

			public:
				inline PULL_CHANNEL & obtain_pull_channel(const ID & id)
				{
					return IMPL::obtain_pull_channel(id);
				}

				virtual inline ~proxy_pull_supplier() {}

				inline proxy_pull_supplier() :
					IMPL(), abstract_pull_supplier<PULL_CHANNEL,ID>() {}

				template <typename ARG1>
				inline proxy_pull_supplier(ARG1 arg1) :
					IMPL(arg1), abstract_pull_supplier<PULL_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2) :
					IMPL(arg1,arg2), abstract_pull_supplier<PULL_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2,typename ARG3>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
					IMPL(arg1,arg2,arg3), abstract_pull_supplier<PULL_CHANNEL,ID>() {}

				template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
					IMPL(arg1,arg2,arg3,arg4), abstract_pull_supplier<PULL_CHANNEL,ID>() {}
		};

		template <typename IMPL, typename PULL_CHANNEL>
		class proxy_pull_supplier<IMPL,PULL_CHANNEL,void> :
			virtual public IMPL,
			virtual public abstract_pull_supplier<PULL_CHANNEL,void>
		{
			public:
				typedef PULL_CHANNEL pull_channel_t;

			private:
				typedef typename PULL_CHANNEL::retval_t ch_retval_t;
				typedef typename PULL_CHANNEL::event_t ch_event_t;

				conversion_must_exist<
					PULL_CHANNEL,
					abstract_pull_channel<ch_retval_t,ch_event_t> >
						argument_cannot_be_converted_to_generic_channel;

			public:
				inline PULL_CHANNEL & obtain_pull_channel(void)
				{
					return IMPL::obtain_pull_channel();
				}

				virtual inline ~proxy_pull_supplier() {}

				inline proxy_pull_supplier() :
					IMPL(), abstract_pull_supplier<PULL_CHANNEL,void>() {}

				template <typename ARG1>
				inline proxy_pull_supplier(ARG1 arg1) :
					IMPL(arg1), abstract_pull_supplier<PULL_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2) :
					IMPL(arg1,arg2), abstract_pull_supplier<PULL_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2,typename ARG3>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
					IMPL(arg1,arg2,arg3), abstract_pull_supplier<PULL_CHANNEL,void>() {}

				template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
				inline proxy_pull_supplier(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
					IMPL(arg1,arg2,arg3,arg4), abstract_pull_supplier<PULL_CHANNEL,void>() {}
		};
	};
};

#endif // AKARMI_FLOW_SPECIALIZED_PULL_SUPPLIER_HH_INCLUDED

// $Id: proxy_pull_supplier.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
