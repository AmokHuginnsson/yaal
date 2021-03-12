/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hmulticontainer.hxx
 * \brief Declaration of HMultiContainerStorage class.
 */

#ifndef YAAL_HCORE_HMULTICONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HMULTICONTAINER_HXX_INCLUDED 1

namespace yaal {

namespace hcore {

struct HMultiContainerStorage {
	template<typename key_type_t, typename value_type_t>
	struct HPacked {
		typedef value_type_t data_type;
		typedef data_type stored_type;
		typedef yaal::hcore::HPair<key_type_t, value_type_t> value_type;
		typedef yaal::hcore::HPair<key_type_t, reference<value_type_t> > accessor_elem_t;
		template<typename const_qual_t>
		struct const_aware_type {
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const,
							accessor_elem_t>::type accessor_t;
			typedef accessor_t accessor_ptr_t;
			template<typename key_provider_t, typename value_provider_t>
			static accessor_t accessor( key_provider_t const& key_, value_provider_t& val_ ) {
				return ( accessor_t( key_, ref( val_ ) ) );
			}
		};
		static data_type const& value( key_type_t const&, value_type_t const& val_ ) {
			return val_;
		}
		static data_type const& value( value_type const& elem_ ) {
			return ( elem_.second );
		}
		static data_type& value( value_type& elem_ ) {
			return ( elem_.second );
		}
	};
	template<typename key_type_t, typename value_type_t>
	struct HTransparent {
		typedef yaal::hcore::HPair<key_type_t, value_type_t> value_type;
		typedef value_type stored_type;
		template<typename const_qual_t>
		struct const_aware_type {
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const&,
							value_type&>::type accessor_t;
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const*,
							value_type*>::type accessor_ptr_t;
			template<typename key_provider_t, typename value_provider_t>
			static accessor_t accessor( key_provider_t const&, value_provider_t& val_ ) {
				return val_;
			}
		};
		static value_type value( key_type_t const& key_, value_type_t const& val_ ) {
			return ( value_type( key_, val_ ) );
		}
		static value_type const& value( value_type const& elem_ ) {
			return elem_;
		}
		static value_type& value( value_type& elem_ ) {
			return elem_;
		}
	};
};

}

}

#endif /* #ifndef YAAL_HCORE_HMULTICONTAINER_HXX_INCLUDED */

