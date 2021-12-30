#ifndef AKARMI_FLOW_BEGIN_INTERFACE_HH_INCLUDED
#define AKARMI_FLOW_BEGIN_INTERFACE_HH_INCLUDED

#include <akarmi_flow/proxy_push_supplier.hh>

namespace akarmi
{
	namespace flow
	{
		template <
			typename IMPL,
			typename BEGIN_DESCRIPTOR = typename IMPL::begin_descriptor >
		class begin_interface :
			public virtual proxy_push_supplier<
				IMPL,
				typename BEGIN_DESCRIPTOR::push_consumer_channel_t,
				typename BEGIN_DESCRIPTOR::push_consumer_channel_id_t >
		{
		public:
			typedef typename BEGIN_DESCRIPTOR::push_consumer_channel_t push_consumer_channel_t_;
			typedef typename BEGIN_DESCRIPTOR::push_consumer_channel_id_t push_consumer_channel_id_t_;

			inline begin_interface() :
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > () {}

			template <typename ARG1>
			inline begin_interface(ARG1 arg1) :
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1) {}

			template <typename ARG1,typename ARG2>
			inline begin_interface(ARG1 arg1,ARG2 arg2) :
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2) {}

			template <typename ARG1,typename ARG2,typename ARG3>
			inline begin_interface(ARG1 arg1,ARG2 arg2,ARG3 arg3) :
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2,arg3) {}

			template <typename ARG1,typename ARG2,typename ARG3,typename ARG4>
			inline begin_interface(ARG1 arg1,ARG2 arg2,ARG3 arg3,ARG4 arg4) :
				proxy_push_supplier< IMPL,
					push_consumer_channel_t_,
					push_consumer_channel_id_t_ > (arg1,arg2,arg3,arg4) {}
		};
	}
}

#endif // AKARMI_FLOW_BEGIN_INTERFACE_HH_INCLUDED

// $Id: begin_interface.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
