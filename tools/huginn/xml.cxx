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

class HElement : public HHuginn::HInvalidatingIterable {
public:
	typedef yaal::hcore::HPointer<yaal::tools::HXml> xml_t;
private:
	xml_t _xml;
	yaal::tools::HXml::HNodeProxy _node;
public:
	HElement( HHuginn::HClass const* class_, xml_t xml_, yaal::tools::HXml::HNodeProxy const& node_ )
		: HInvalidatingIterable( class_ )
		, _xml( xml_ )
		, _node( node_ ) {
		return;
	}
	HHuginn::value_t name( HThread* thread_ ) const {
		return ( !! _node ? thread_->object_factory().create_string( _node.get_name() ) : thread_->runtime().none_value() );
	}
	HHuginn::value_t nth_child( HThread*, int, int );
	HHuginn::value_t append( HThread*, HHuginn::values_t&, int );
	void remove( HThread*, HElement*, int );
	void remove_nth( HThread*, int, int );
	HHuginn::value_t parent( HThread* );
	HHuginn::value_t document( HThread* );
	yaal::tools::HXml::HNodeProxy const& node( void ) const {
		return ( _node );
	}
protected:
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override {
		return ( distance( _node.begin(), _node.end() ) );
	}
	HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const override {
		M_PROLOG
		throw HHuginn::HHuginnRuntimeException(
			"Copy semantics is not supported on `XML.Element`s.",
			thread_->current_frame()->file_id(),
			position_
		);
		M_EPILOG
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
			{ "name",       runtime_->create_method( &HElementClass::element_name ), "get the name of this `Element`" },
			{ "parent",     runtime_->create_method( &HElementClass::parent ),       "get parent node of this `Element`" },
			{ "subscript",  runtime_->create_method( &HElementClass::subscript ),    "( *index* ) get *index*'th child node of this `Element`" },
			{ "document",   runtime_->create_method( &HElementClass::document ),     "get an `XML` `Document` to which this `Element` belongs to" },
			{ "append",     runtime_->create_method( &HElementClass::append ),       "( *type*, *value* ) - append new node of type *type* and value *value* at the end of this `Element`" },
			{ "remove",     runtime_->create_method( &HElementClass::remove ),       "( *node* ) - remove given `Element` child node of this `Element`" },
			{ "remove_nth", runtime_->create_method( &HElementClass::remove_nth ),   "( *nth* ) - remove *nth* child node of this `Element`" }
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
	static HHuginn::value_t parent( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Element.parent", values_, 0, 0, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		return ( element.parent( thread_ ) );
		M_EPILOG
	}
	static HHuginn::value_t subscript( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Element.subscript", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		return ( element.nth_child( thread_, safe_int::cast<int>( get_integer( values_[0] ) ), position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t document( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Element.document", values_, 0, 0, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		return ( element.document( thread_ ) );
		M_EPILOG
	}
	static HHuginn::value_t append( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Element.append", values_, { HHuginn::TYPE::FUNCTION_REFERENCE, HHuginn::TYPE::STRING }, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		return ( element.append( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t remove( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		verify_signature_by_class( "Element.remove", values_, { element.get_class() }, thread_, position_ );
		element.remove( thread_, static_cast<HElement*>( values_[0].raw() ), position_ );
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t remove_nth( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Element.remove_nth", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		HElement& element( *static_cast<HElement*>( object_->raw() ) );
		element.remove_nth( thread_, safe_int::cast<int>( get_integer( values_[0] ) ), position_ );
		return ( *object_ );
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

class HElementIterator : public HNotifableIterator {
	HHuginn::HClass const* _class;
	xml_t _xml;
	yaal::tools::HXml::HNodeProxy _node;
	yaal::tools::HXml::HIterator _it;
public:
	HElementIterator( HHuginn::HClass const* class_, xml_t xml_, HElement* element_ )
		: HNotifableIterator( element_ )
		, _class( class_ )
		, _xml( xml_ )
		, _node( element_->node() )
		, _it( _node.begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int ) override;
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _it != _node.end() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _node.end();
	}
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HElementIterator( HElementIterator const& ) = delete;
	HElementIterator& operator = ( HElementIterator const& ) = delete;
};

HHuginn::HIterable::iterator_t HElement::do_iterator( HThread*, int ) {
	return ( make_pointer<HElementIterator>( get_class(), _xml, this ) );
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
	void save( HStream& stream_ ) {
		M_PROLOG
		_xml->save( stream_.raw(), yaal::tools::HXml::GENERATOR::INDENT );
		return;
		M_EPILOG
	}
	HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		M_PROLOG
		return ( thread_->object_factory().create<HDocument>( get_class(), make_pointer<yaal::tools::HXml>( *_xml ) ) );
		M_EPILOG
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
	HHuginn::class_t const& _streamClass;
	HHuginn::class_t const& _exceptionClass;
public:
	HDocumentClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& streamClass_,
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
		, _streamClass( streamClass_ )
		, _exceptionClass( exceptionClass_ ) {
		HHuginn::field_definitions_t fd{
			{ "root", runtime_->create_method( &HDocumentClass::root ), "get root element of an `XML` `Document`" },
			{ "save", runtime_->create_method( &HDocumentClass::save ), "( *stream* ) - save this `Document` to given *stream*" }
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
	static HHuginn::value_t save( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HDocument& document( *static_cast<HDocument*>( object_->raw() ) );
		HDocumentClass const& documentClass( *static_cast<HDocumentClass const*>( document.get_class() ) );
		verify_signature_by_class( "XML.save", values_, { documentClass._streamClass.raw() }, thread_, position_ );
		document.save( *static_cast<HStream*>( values_[0].raw() ) );
		return ( *object_ );
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
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::class_t const& streamClass_, HHuginn::class_t const& exceptionClass_, HHuginn::HClass* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				HRuntime::class_constructor_t(
					[&runtime_, &streamClass_, &exceptionClass_, &origin_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
						return (
							make_pointer<HDocumentClass>(
								runtime_,
								typeId_,
								streamClass_,
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

namespace {

HHuginn::value_t make_node_ref( HObjectFactory& of_, HDocumentClass const* dc_, xml_t const& xml_, yaal::tools::HXml::HNodeProxy const& n_ ) {
	M_PROLOG
	HHuginn::value_t v;
	switch ( n_.get_type() ) {
		case ( yaal::tools::HXml::HNode::TYPE::NODE ): {
			v = of_.create<HElement>( dc_->element_class(), xml_, n_ );
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::CONTENT ): {
			v = of_.create<HHuginn::HString>( dc_->text_class(), n_.get_value() );
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::COMMENT ): {
			v = of_.create<HHuginn::HString>( dc_->comment_class(), n_.get_value() );
		} break;
		case ( yaal::tools::HXml::HNode::TYPE::ENTITY ): {
			v = of_.create<HHuginn::HString>( dc_->entity_class(), n_.get_value() );
		} break;
	}
	return ( v );
	M_EPILOG
}

}

HHuginn::value_t HElement::append( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	if ( ! _node ) {
		throw HHuginn::HHuginnRuntimeException(
			"Referenced `XML.Element` was removed.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HElementClass const* ec( static_cast<HElementClass const*>( get_class() ) );
	HDocumentClass const* dc( ec->document_class() );
	HHuginn::HFunctionReference& fr( *static_cast<HHuginn::HFunctionReference*>( values_[0].raw() ) );
	void const* id( fr.function().id() );
	HRuntime& r( thread_->runtime() );
	HString const& s( get_string( values_[1] ) );
	yaal::tools::HXml::HNode::TYPE t( yaal::tools::HXml::HNode::TYPE::NODE );
	if ( id == dc->element_class() ) {
		/* node type already set */
	} else if ( id == dc->text_class() ) {
		t = yaal::tools::HXml::HNode::TYPE::CONTENT;
	} else if ( id == dc->comment_class() ) {
		t = yaal::tools::HXml::HNode::TYPE::COMMENT;
	} else if ( id == dc->entity_class() ) {
		t = yaal::tools::HXml::HNode::TYPE::ENTITY;
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Node type must be one of: `XML.Element`, `XML.Text`, `XML.Comment` or `XML.Entity`, not `"_ys.append( r.identifier_name( fr.identifier_id() ) ).append( "`." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( make_node_ref( *r.object_factory(), dc, _xml, *_node.add_node( t, s ) ) );
	M_EPILOG
}

HHuginn::value_t HElement::parent( HThread* thread_ ) {
	M_PROLOG
	HElementClass const* ec( static_cast<HElementClass const*>( get_class() ) );
	HDocumentClass const* dc( ec->document_class() );
	yaal::tools::HXml::HNodeProxy n( _node.get_parent() );
	return (
		!! n
			? thread_->object_factory().create<HElement>( dc->element_class(), _xml, n )
			: thread_->runtime().none_value()
	);
	M_EPILOG
}

HHuginn::value_t HElement::nth_child( HThread* thread_, int index_, int position_ ) {
	M_PROLOG
	HElementClass const* ec( static_cast<HElementClass const*>( get_class() ) );
	HDocumentClass const* dc( ec->document_class() );
	if ( ( index_ < 0 ) || ( index_ >= _node.child_count() ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid `XML.Element` child index: "_ys.append( index_ ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	yaal::tools::HXml::HIterator it( _node.begin() );
	advance( it, index_ );
	return ( make_node_ref( thread_->object_factory(), dc, _xml, *it ) );
	M_EPILOG
}

void HElement::remove( HThread* thread_, HElement* element_, int position_ ) {
	M_PROLOG
	yaal::tools::HXml::HIterator it(
		yaal::find_if(
			_node.begin(),
			_node.end(),
			[&element_]( yaal::tools::HXml::HConstNodeProxy const& node_ ) {
				return ( node_ == element_->_node );
			}
		)
	);
	if ( it == _node.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Given `XML.Element` in not a child node of this `Element`. ",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	invalidate( it.node_id() );
	_node.remove_node( it );
	return;
	M_EPILOG
}

void HElement::remove_nth( HThread* thread_, int nth_, int position_ ) {
	M_PROLOG
	if ( ( nth_ < 0 ) || ( nth_ >= _node.child_count() ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid `XML.Element` child index: "_ys.append( nth_ ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	yaal::tools::HXml::HIterator it( _node.begin() );
	advance( it, nth_ );
	invalidate( it.node_id() );
	_node.remove_node( it );
	return;
	M_EPILOG
}

HHuginn::value_t HElement::document( HThread* thread_ ) {
	M_PROLOG
	HElementClass const* ec( static_cast<HElementClass const*>( get_class() ) );
	HDocumentClass const* dc( ec->document_class() );
	return ( thread_->object_factory().create<xml::HDocument>( dc, _xml ) );
	M_EPILOG
}

HHuginn::value_t HElementIterator::do_value( HThread* thread_, int ) {
	return ( make_node_ref( thread_->object_factory(), static_cast<HElementClass const*>( _class )->document_class(), _xml, *_it ) );
}

}

class HXml : public HHuginn::HValue {
	HHuginn::class_t _documentClass;
	HHuginn::class_t _streamClass;
	HHuginn::class_t _exceptionClass;
public:
	HXml( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _documentClass( xml::HDocumentClass::get_class( class_->runtime(), _streamClass, _exceptionClass, class_ ) )
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

