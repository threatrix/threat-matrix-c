#ifndef AKARMI_FLOW_LOCALCALL_PULL_CHANNEL_HH_INCLUDED
#define AKARMI_FLOW_LOCALCALL_PULL_CHANNEL_HH_INCLUDED

#include <akarmi_flow/abstract_pull_channel.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename IMPL,typename RETVAL,typename EVENT>
		class localcall_pull_channel :
			public abstract_pull_channel<RETVAL,EVENT>
		{
			public:
				typedef RETVAL (IMPL::*FUN)(EVENT & e);

				inline RETVAL pull(EVENT & e)
				{
					return (impl_->*fun_)(e);
				}

				inline localcall_pull_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_pull_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_pull_channel() {}
		};

		template <typename IMPL>
		class localcall_pull_channel<IMPL,void,void> :
			public abstract_pull_channel<void,void>
		{
			public:
				typedef void (IMPL::*FUN)(void);

				inline void pull(void)
				{
					(impl_->*fun_)();
				}

				inline localcall_pull_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_pull_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_pull_channel() {}
		};

		template <typename IMPL,typename RETVAL>
		class localcall_pull_channel<IMPL,RETVAL,void> :
			public abstract_pull_channel<RETVAL,void>
		{
			public:
				typedef RETVAL (IMPL::*FUN)(void);

				inline RETVAL pull(void)
				{
					return (impl_->*fun_)();
				}

				inline localcall_pull_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_pull_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_pull_channel() {}
		};

		template <typename IMPL,typename EVENT>
		class localcall_pull_channel<IMPL,void,EVENT> :
			public abstract_pull_channel<void,EVENT>
		{
			public:
				typedef void (IMPL::*FUN)(EVENT & e);

				inline void pull(EVENT & e)
				{
					(impl_->*fun_)(e);
				}

				inline localcall_pull_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_pull_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_pull_channel() {}
		};
	};
};

#endif // AKARMI_FLOW_LOCALCALL_PULL_CHANNEL_HH_INCLUDED

// $Id: localcall_pull_channel.hh,v 1.4 2005/04/08 17:51:38 dbeck Exp $
