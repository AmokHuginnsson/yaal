/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/hxml.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "stream.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace xml {

typedef yaal::hcore::HPointer<yaal::tools::HXml> xml_t;

namespace {

HHuginn::class_t make_node_class( HRuntime* runtime_, HHuginn::HClass const* origin_, char const* name_, char const* doc_ ) {
	M_PROLOG
	HHuginn::class_t c( runtime_->create_class( name_, doc_, HHuginn::ACCESS::PRIVATE ) );
	c->set_origin( origin_ );
	c->redefine( runtime_->object_factory()->string_class(), {} );
	runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::PACKAGE );
	return ( c );
	M_EPILOG
}

}

class HElement : public HHuginn::HIterable {
public:
	typedef yaal::hcore::HPointer<yaal::tools::HXml> xml_t;
private:
	xml_t _xml;
	yaal::tools::HXml::HNodeProxy _node;
public:
	HElement( HHuginn::HClass const* class_, xml_t xml_, yaal::tools::HXml::HNodeProxy const& node_ )
		: HIterable( class_ )
		, _xml( xml_ )
		, _node( node_ ) {
		return;
	}
	HHuginn::value_t name( HThread* thread_ ) const {
		return ( thread_->object_factory().create_string( _node.get_name() ) );
	}
protected:
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override {
		return ( distance( _node.begin(), _node.end() ) );
	}
};

class HDocumentClass;

class HElementClass : public HHuginn::HClass {
public:
	typedef HElementClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::HClass const* _documentClass;
	HHuginn::class_t const& _exceptionClass;
public:
	HElementClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::HClass const* documentClass_,
		HHuginn::class_t const& exceptionClass_,
		HHuginn::HClass const* origin_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "Element" ),
			"The `Element` class represents a single `XML` `Document` `Element` reference.",
			HHuginn::ACCESS::PRIVATE
		)
		, _documentClass( documentClass_ )
		, _exceptionClass( exceptionClass_ ) {
		HHuginn::field_definitions_t fd{
			{ "name", runtime_->create_method( &HElementClass::element_name ), "get the name of this `Element`" }
		};
		set_origin( origin_ );
		redefine( nullptr, fd );
		return;
	}
	static HHuginn::value_t element_name( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Element.name", values_, 0, 0, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		return ( element.name( thread_ ) );
		M_EPILOG
	}
	HDocumentClass const* document_class( void ) const;
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::HClass const* documentClass_, HHuginn::class_t const& exceptionClass_ , HHuginn::HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				HRuntime::class_constructor_t(
					[&runtime_, &documentClass_, &exceptionClass_, &origin_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
						return (
							make_pointer<HElementClass>(
								runtime_,
								typeId_,
								documentClass_,
								exceptionClass_,
								origin_
							)
						);
					}
				)
			)
		);
		runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::PACKAGE );
		return ( c );
		M_EPILOG
	}
private:
	HElementClass( HElementClass const& ) = delete;
	HElementClass& operator = ( HElementClass const& ) = delete;
};

class HElementIterator : public HIteratorInterface {
	HHuginn::HClass const* _class;
	xml_t _xml;
	yaal::tools::HXml::HNodeProxy _node;
	yaal::tools::HXml::HIterator _it;
public:
	HElementIterator( HHuginn::HClass const* class_, xml_t xml_, yaal::tools::HXml::HNodeProxy const& node_ )
		: _class( class_ )
		, _xml( xml_ )
		, _node( node_ )
		, _it( _node.begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int ) override;
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _it != _node.end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HElementIterator( HElementIterator const& ) = delete;
	HElementIterator& operator = ( HElementIterator const& ) = delete;
};

HHuginn::HIterable::iterator_t HElement::do_iterator( HThread*, int ) {
	return ( make_pointer<HElementIterator>( get_class(), _xml, _node ) );
}

class HDocument : public HHuginn::HValue {
private:
	xml_t _xml;
public:
	HDocument( HHuginn::HClass const* class_, yaal::hcore::HString const& name_ )
		: HValue( class_ )
		, _xml( make_pointer<yaal::tools::HXml>() ) {
		_xml->create_root( name_ );
		return;
	}
	HDocument( HHuginn::HClass const* class_, xml_t xml_ )
		: HValue( class_ )
		, _xml( xml_ ) {
		return;
	}
	HHuginn::value_t root( HThread* thread_, HHuginn::HClass const* class_ ) {
		return ( thread_->object_factory().create<HElement>( class_, _xml, _xml->get_root() ) );
	}
};

class HDocumentClass : public HHuginn::HClass {
public:
	typedef HDocumentClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _elementClass;
	HHuginn::class_t _textClass;
	HHuginn::class_t _commentClass;
	HHuginn::class_t _entityClass;
	HHuginn::class_t const& _exceptionClass;
public:
	HDocumentClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_,
		HHuginn::HClass* origin_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "Document" ),
			"The `Document` class represents a single `XML` `Document` instance.",
			HHuginn::ACCESS::PRIVATE
		)
		, _elementClass(
			add_class_as_member(
				origin_,
				HElementClass::get_class( runtime_, this, exceptionClass_, origin_ ),
				"An `XML` `Element` node type."
			)
		)
		, _textClass(
			add_class_as_member(
				origin_,
				make_node_class( runtime_, origin_, "Text", "The `Text` class represents TEXT node in an `XML` `Document`." ),
				"An `XML` `TEXT` node type."
			)
		)
		, _commentClass(
			add_class_as_member(
				origin_,
				make_node_class( runtime_, origin_, "Comment", "The `Comment` class represents a comment string in an `XML` `Document`." ),
				"An `XML` `Comment` node type."
			)
		)
		, _entityClass(
			add_class_as_member(
				origin_,
				make_node_class( runtime_, origin_, "Entity", "The `Entity` class represents an `Entity` reference in an `XML` `Document`." ),
				"An `XML` `Entity` node type."
			)
		)
		, _exceptionClass( exceptionClass_ ) {
		HHuginn::field_definitions_t fd{
			{ "root", runtime_->create_method( &HDocumentClass::root ), "get root element of an `XML` `Document`" }
		};
		set_origin( origin_ );
		redefine( nullptr, fd );
		return;
	}
	static HHuginn::value_t root( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Document.root", values_, 0, 0, thread_, position_ );
		HDocument& document( *static_cast<HDocument*>( object_->raw() ) );
		HDocumentClass const& documentClass( *static_cast<HDocumentClass const*>( document.get_class() ) );
		return ( document.root( thread_, documentClass.element_class() ) );
		M_EPILOG
	}
	HHuginn::HClass const* element_class( void ) const {
		return ( _elementClass.raw() );
	}
	HHuginn::HClass const* text_class( void ) const {
		return ( _textClass.raw() );
	}
	HHuginn::HClass const* comment_class( void ) const {
		return ( _commentClass.raw() );
	}
	HHuginn::HClass const* entity_class( void ) const {
		return ( _entityClass.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::class_t const& exceptionClass_, HHuginn::HClass* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				HRuntime::class_constructor_t(
					[&runtime_, &exceptionClass_, &origin_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
						return (
							make_pointer<HDocumentClass>(
								runtime_,
								typeId_,
								exceptionClass_,
								origin_
							)
						);
					}
				)
			)
		);
		runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::PACKAGE );
		return ( c );
		M_EPILOG
	}
private:
	HDocumentClass( HDocumentClass const& ) = delete;
	HDocumentClass& operator = ( HDocumentClass const& ) = delete;
};

HDocumentClass const* HElementClass::document_class( void ) const {
	return ( static_cast<HDocumentClass const *>( _documentClass ) );
}

HHuginn::value_t HElementIterator::do_value( HThread* thread_, int ) {
	HHuginn::value_t v;
	switch ( (*_it).get_type() ) {
		case ( yaal::tools::HXml::HNode::TYPE::NODE ): {
			v = thread_->object_factory().create<HElement>( _class, _xml, *_it );
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::CONTENT ): {
			v = thread_->object_factory().create<HHuginn::HString>(
				static_cast<HElementClass const*>( _class )->document_class()->text_class(),
				(*_it).get_value()
			);
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::COMMENT ): {
			v = thread_->object_factory().create<HHuginn::HString>(
				static_cast<HElementClass const*>( _class )->document_class()->comment_class(),
				(*_it).get_value()
			);
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::ENTITY ): {
			v = thread_->object_factory().create<HHuginn::HString>(
				static_cast<HElementClass const*>( _class )->document_class()->entity_class(),
				(*_it).get_value()
			);
		} break;
	}
	return ( v );
}

}

class HXml : public HHuginn::HValue {
	HHuginn::class_t _documentClass;
	HHuginn::class_t _streamClass;
	HHuginn::class_t _exceptionClass;
public:
	HXml( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _documentClass( xml::HDocumentClass::get_class( class_->runtime(), _exceptionClass, class_ ) )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t load( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HXml& xml( *static_cast<HXml*>( object_->raw() ) );
		verify_signature_by_class( "XML.load", values_, { xml._streamClass.raw() }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		HXml& XML( *static_cast<HXml*>( object_->raw() ) );
		try {
			xml::xml_t doc( make_pointer<yaal::tools::HXml>() );
			HStream& stream( *static_cast<HStream*>( values_[0].raw() ) );
			doc->load( stream.raw() );
			v = thread_->object_factory().create<xml::HDocument>( XML._documentClass.raw(), doc );
		} catch ( HException const& e ) {
			thread_->raise( XML._exceptionClass.raw(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t create( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "XML.create", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HXml& XML( *static_cast<HXml*>( object_->raw() ) );
		return ( thread_->object_factory().create<xml::HDocument>( XML._documentClass.raw(), get_string( values_[0] ) ) );
		M_EPILOG
	}
};

namespace package_factory {

class HXmlCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} xmlCreator;

HHuginn::value_t HXmlCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"XML",
			"The `XML` package provides various text manipulation algorithms.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "load",   runtime_->create_method( &HXml::load ),   "( *stream* ) - load an `XML` document from given *stream*" },
		{ "create", runtime_->create_method( &HXml::create ), "( *name* ) - create new `XML` document with given root node *name*" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HXml>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "XML", &xmlCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

