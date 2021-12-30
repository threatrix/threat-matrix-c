#ifndef AKARMI_FLOW_LOCALCALL_PUSH_CHANNEL_HH_INCLUDED
#define AKARMI_FLOW_LOCALCALL_PUSH_CHANNEL_HH_INCLUDED

#include <akarmi_flow/abstract_push_channel.hh>

namespace akarmi
{
	namespace flow
	{
		template <typename IMPL,typename RETVAL,typename EVENT>
		class localcall_push_channel : public abstract_push_channel<RETVAL,EVENT>
		{
			public:
				typedef RETVAL (IMPL::*FUN)(const EVENT & e);

				inline RETVAL push(const EVENT & e)
				{
					return (impl_->*fun_)(e);
				}

				inline localcall_push_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_push_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_push_channel() {}
		};

		template <typename IMPL>
		class localcall_push_channel<IMPL,void,void> :
			public abstract_push_channel<void,void>
		{
			public:
				typedef void (IMPL::*FUN)(void);

				inline void push(void)
				{
					(impl_->*fun_)();
				}

				inline localcall_push_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_push_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_push_channel() {}
		};

		template <typename IMPL,typename RETVAL>
		class localcall_push_channel<IMPL,RETVAL,void> :
			public abstract_push_channel<RETVAL,void>
		{
			public:
				typedef RETVAL (IMPL::*FUN)(void);

				inline RETVAL push(void)
				{
					return (impl_->*fun_)();
				}

				inline localcall_push_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_push_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_push_channel() {}
		};

		template <typename IMPL,typename EVENT>
		class localcall_push_channel<IMPL,void,EVENT> :
			public abstract_push_channel<void,EVENT>
		{
			public:
				typedef void (IMPL::*FUN)(const EVENT & e);

				inline void push(const EVENT & e)
				{
					(impl_->*fun_)(e);
				}

				inline localcall_push_channel(IMPL & i,FUN f) :
					impl_(&i), fun_(f) {}

				virtual inline ~localcall_push_channel() {}

			private:
				IMPL * impl_;
				FUN    fun_;
				localcall_push_channel() {}
		};
	};
};

#endif // AKARMI_FLOW_LOCALCALL_PUSH_CHANNEL_HH_INCLUDED

// $Id: localcall_push_channel.hh,v 1.4 2005/04/08 17:51:38 dbeck Exp $
