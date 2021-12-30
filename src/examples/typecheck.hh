#ifndef AKARMI_FLOW_TYPECHECK_HH_INCLUDED
#define AKARMI_FLOW_TYPECHECK_HH_INCLUDED

namespace akarmi
{
	namespace flow
	{
		template <typename T1,typename T2>
		struct sametype
		{
			enum { result = false };
		};

		template <typename T>
		struct sametype<T,T>
		{
			enum { result = true };
		};

		template <int>
		struct type_error
		{
		};

		template <>
		struct type_error<true>
		{
		};

		template <typename DERIVED_T,typename BASE_T>
		class conversion_must_exist
		{
			public:
				~conversion_must_exist()
				{
					void (*p)(DERIVED_T *d,BASE_T *b) = constraint;
				};
			private:
				static void constraint(DERIVED_T* derived,BASE_T *base)
				{
					base = derived;
				}
		};
	};
};

#endif // AKARMI_FLOW_GENERIC_PUSH_CHANNEL_HH_INCLUDED

// $Id: typecheck.hh,v 1.2 2005/04/08 17:51:38 dbeck Exp $
