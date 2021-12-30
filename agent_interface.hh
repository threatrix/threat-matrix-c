#ifndef AKARMI_FLOW_AGENT_INTERFACE_HH_INCLUDED
#define AKARMI_FLOW_AGENT_INTERFACE_HH_INCLUDED

#include <akarmi_flow/proxy_pull_consumer.hh>
#include <akarmi_flow/proxy_push_supplier.hh>

namespace akarmi
{
	namespace flow
	{
		template <
			typename IMPL,
			typename AGENT_DESCRIPTOR = typename IMPL::agent_descriptor >
		class agent_interface :
			public virtual proxy_pull_consumer<
				IMPL,
				typename AGENT_DESCRIPTOR::pull_supplier_channel_t,
				typename AGENT_DESCRIPTOR::pull_supplier_channel_id_t >,
			public virtual proxy_push_supplier<
				IMPL,
				typename AGENT_DESCRIPTOR::push_consumer_channel_t,
				typename AGENT_DESCRIPTOR::push_consumer_channel_id_t >
		{
		public:
			typedef typename AGENT_DESCRIPTOR::pull_supplier_channel_t pull_supplier_channel_t_;
			typedef typename AGENT_DESCRIPTOR::pull_supplier_channel_id_t pull_supplier_channel_id_t_;
			typedef typename AGENT_DESCRIPTOR::push_consumer_channel_t push_consumer_channel_t_;
			typedef typename AGENT_DESCRIPTOR::push_consumer_channel_id_t push_consumer_channel_id_t_;

			inline agent_interface() :
				proxy_pull_consumer< IMPL,
					pull_supplier_channel_t_,
					pull_supplier_channel_id_t_ > (),
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > () {}

			template <typename ARG1>
			inline agent_interface(ARG1 arg1) :
				proxy_pull_consumer< IMPL,
					pull_supplier_channel_t_,
					pull_supplier_channel_id_t_ > (arg1),
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1) {}

			template <typename ARG1,typename ARG2>
			inline agent_interface(ARG1 arg1,ARG2 arg2) :
				proxy_pull_consumer< IMPL,
					pull_supplier_channel_t_,
					pull_supplier_channel_id_t_ > (arg1,arg2),
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2) {}

			template <typename ARG1,typename ARG2,typename ARG3>
			inline agent_interface(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
				proxy_pull_consumer< IMPL,
					pull_supplier_channel_t_,
					pull_supplier_channel_id_t_ > (arg1,arg2,arg3),
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2,arg3) {}

			template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
			inline agent_interface(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
				proxy_pull_consumer< IMPL,
					pull_supplier_channel_t_,
					pull_supplier_channel_id_t_ > (arg1,arg2,arg3,arg4),
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2,arg3,arg4) {}
		};
	}
}

#endif // AKARMI_FLOW_AGENT_INTERFACE_HH_INCLUDED

// $Id: agent_interface.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
