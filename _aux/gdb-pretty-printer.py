#! /usr/bin/env python

import re

def yaal_lookup_function( val ):
	lookup_tag = val.type.strip_typedefs().tag
	if lookup_tag == None:
		return None
	regex = re.compile( "^yaal::hcore::HString$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHStringPrinter( val )
	regex = re.compile( "^yaal::hcore::HArray<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHArrayPrinter( val )
	regex = re.compile( "^yaal::hcore::HList<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHListPrinter( val )
	regex = re.compile( "^yaal::hcore::HMap<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHMapPrinter( val )
	regex = re.compile( "^yaal::hcore::HSet<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHSetPrinter( val )
	regex = re.compile( "^yaal::hcore::HHashMap<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHHashMapPrinter( val )
	regex = re.compile( "^yaal::hcore::HHashSet<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHHashSetPrinter( val )
	regex = re.compile( "^yaal::hcore::HNumber$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHNumberPrinter( val )
	regex = re.compile( "^yaal::hcore::HVariant<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHVariantPrinter( val )

	return None

class YaalHCoreHStringPrinter:
	"Print a yaal::hcore::HString"

	def __init__( self, val ):
		self._val = val

	def to_string( self ):
		return self._val['_buffer']

	def display_hint( self ):
		return 'string'

class YaalHCoreHArrayPrinter:
	"Print a yaal::hcore::HArray"

	class _iterator:
		def __init__ (self, start, size):
			self.item = start
			self.size = size
			self.count = 0

		def __iter__(self):
			return self

		def next(self):
			if self.count == self.size:
				raise StopIteration
			count = self.count
			self.count = self.count + 1
			elt = self.item.dereference()
			self.item = self.item + 1
			return ('[%d]' % count, elt)

	def __init__(self, val):
		self.val = val
	
	def sizeof_elem( self ):
		return self.val.type.template_argument( 0 ).sizeof

	def capacity( self ):
		return self.val['_buf']['_size'] / self.sizeof_elem()

	def children( self ):
		return self._iterator(self.val['_buf']['_data'].cast(self.val.type.template_argument( 0 ).pointer()), self.val['_size'])

	def to_string( self ):
		return ( "yaal::hcore::HArray of `%s' of length %d, capacity %d" % ( self.val.type.template_argument( 0 ), self.val['_size'], self.capacity() ) )

	def display_hint( self ):
		return 'array'

class YaalHCoreHListPrinter:
	"Print a yaal::hcore::HArray"

	class _iterator:
		def __init__ (self, start, size):
			self.item = start
			self.size = size
			self.count = 0

		def __iter__(self):
			return self

		def next(self):
			if self.count == self.size:
				raise StopIteration
			count = self.count
			self.count = self.count + 1
			elt = self.item['_value']
			self.item = self.item['_next']
			return ('[%d]' % count, elt)

	def __init__(self, val):
		self.val = val
	
	def children( self ):
		return self._iterator(self.val['_hook'], self.val['_size'])

	def to_string( self ):
		return ( "yaal::hcore::HList of `%s' of length %d" % ( self.val.type.template_argument( 0 ), self.val['_size'] ) )

	def display_hint( self ):
		return 'array'

class BinaryTreeIterator( object ):
	def __init__( self, owner, start, size ):
		self._owner = owner
		self.item = start
		self.size = size
		self.count = 0
	def do_next( self, it ):
		lastNode = it;
		while it != 0:
			if ( it['_right'] != 0 ) and ( it['_right'] != lastNode ):
				it = it['_right']
				while ( it['_left'] ):
					it = it['_left']
				break
			else:
				lastNode = it
				it = it['_parent']
				if ( it != 0 ) and ( lastNode == it['_left'] ):
					break
		return it

class YaalHCoreHMapPrinter:
	"Print a yaal::hcore::HMap"

	class _iterator( BinaryTreeIterator ):

		def __iter__(self):
			return self

		def next(self):
			if self.count == ( self.size * 2 ):
				raise StopIteration
			count = self.count
			valuetype = gdb.lookup_type( "yaal::hcore::HPair<%s, %s>" % ( self._owner.type.template_argument( 0 ).const(), self._owner.type.template_argument( 1 ) ) )
			nodetype = gdb.lookup_type( "yaal::hcore::HSBBSTree::HNode<%s>" % ( valuetype ) ).pointer()
			if ( count % 2 ) == 0:
				elt = self.item.cast( nodetype )['_key']['first']
			else:
				elt = self.item.cast( nodetype )['_key']['second']
				self.item = self.do_next( self.item )
			self.count = self.count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val ):
		self.val = val

	def begin( self ):
		node = self.val['_engine']['_root']
		while node['_left'] != 0:
			node = node['_left']
		return node

	def children( self ):
		return self._iterator( self.val, self.begin(), self.val['_engine']['_size'] )

	def to_string( self ):
		return ( "yaal::hcore::HMap of `%s' to `%s' of length %d" % ( self.val.type.template_argument( 0 ), self.val.type.template_argument( 1 ), self.val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'map'

class YaalHCoreHSetPrinter:
	"Print a yaal::hcore::HSet"

	class _iterator( BinaryTreeIterator ):

		def __iter__(self):
			return self

		def next(self):
			if self.count == self.size:
				raise StopIteration
			count = self.count
			nodetype = gdb.lookup_type( "yaal::hcore::HSBBSTree::HNode<%s>" % ( self._owner.type.template_argument( 0 ) ) ).pointer()
			elt = self.item.cast( nodetype )['_key']
			self.item = self.do_next( self.item )
			self.count = self.count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val ):
		self.val = val

	def begin( self ):
		node = self.val['_engine']['_root']
		while node['_left'] != 0:
			node = node['_left']
		return node

	def children( self ):
		return self._iterator( self.val, self.begin(), self.val['_engine']['_size'] )

	def to_string( self ):
		return ( "yaal::hcore::HSet of `%s' of length %d" % ( self.val.type.template_argument( 0 ), self.val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'array'

class HashContainterIterator( object ):
	def __init__( self, owner, atom, index, size ):
		self._owner = owner
		self._atom = atom
		self._index = index
		self.size = size
		self.count = 0

	def do_next( self ):
		if self._atom != 0:
			self._atom = self._atom['_next'];
			if self._atom == 0:
				self._index = self._index + 1;
		if self._atom == 0:
			buckets = self._owner.buckets()
			while ( self._index < self._owner.val['_engine']['_prime'] ) and ( buckets[ self._index ] == 0 ):
				self._index = self._index + 1
			if self._index < self._owner.val['_engine']['_prime']:
				self._atom = buckets[ self._index ];
		if self._atom == 0:
			self._index = self._owner.val['_engine']['_prime'];


class YaalHCoreHHashMapPrinter:
	"Print a yaal::hcore::HHashMap"

	class _iterator( HashContainterIterator ):
		def __iter__(self):
			return self

		def next(self):
			if self.count == ( self.size * 2 ):
				raise StopIteration
			count = self.count
			nodetype = self._owner.nodetype()
			if ( count % 2 ) == 0:
				elt = self._atom.cast( nodetype )['_value']['first']
			else:
				elt = self._atom.cast( nodetype )['_value']['second']
				self.do_next()
			self.count = self.count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val ):
		self.val = val

	def nodetype( self ):
		valuetype = gdb.lookup_type( "yaal::hcore::HPair<%s, %s>" % ( self.val.type.template_argument( 0 ).const(), self.val.type.template_argument( 1 ) ) )
		nodetype = gdb.lookup_type( "yaal::hcore::HHashContainer::HAtom<%s>" % ( valuetype ) ).pointer()
		return nodetype

	def buckets( self ):
		buckets = self.val['_engine']['_buckets']['_data']
		return buckets.cast( self.nodetype().pointer() )

	def children( self ):
		it = self._iterator( self, 0, 0, self.val['_engine']['_size'] )
		it.do_next()
		return it

	def to_string( self ):
		return ( "yaal::hcore::HHashMap of `%s' to `%s' of length %d" % ( self.val.type.template_argument( 0 ), self.val.type.template_argument( 1 ), self.val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'map'

class YaalHCoreHHashSetPrinter:
	"Print a yaal::hcore::HHashSet"

	class _iterator( HashContainterIterator ):
		def __iter__(self):
			return self

		def next(self):
			if self.count == self.size:
				raise StopIteration
			count = self.count
			nodetype = self._owner.nodetype()
			elt = self._atom.cast( nodetype )['_value']
			self.do_next()
			self.count = self.count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val ):
		self.val = val

	def nodetype( self ):
		nodetype = gdb.lookup_type( "yaal::hcore::HHashContainer::HAtom<%s>" % ( self.val.type.template_argument( 0 ) ) ).pointer()
		return nodetype

	def buckets( self ):
		buckets = self.val['_engine']['_buckets']['_data']
		return buckets.cast( self.nodetype().pointer() )

	def children( self ):
		it = self._iterator( self, 0, 0, self.val['_engine']['_size'] )
		it.do_next()
		return it

	def to_string( self ):
		return ( "yaal::hcore::HHashSet of `%s' of length %d" % ( self.val.type.template_argument( 0 ), self.val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'array'

class YaalHCoreHNumberPrinter:
	"Print a yaal::hcore::HNumber"

	def __init__( self, val ):
		self._val = val

	def to_string( self ):
		s = ""
		if self._val['_digitCount'] > 0:
			if self._val['_negative']:
				s = s + "-"
			digit = 0
			data = self._val['_canonical']['_data'].cast( gdb.lookup_type( "char" ).pointer() ).string( 'ascii', 'ignore', self._val['_digitCount'] )
			while digit < self._val['_integralPartSize']:
				s = s + chr( ord( data[digit] ) + ord( '0' ) )
				digit = digit + 1
			if self._val['_digitCount'] > self._val['_integralPartSize']:
				s = s + "."
			while digit < self._val['_digitCount']:
				s = s + chr( ord( data[digit] ) + ord( '0' ) )
				digit = digit + 1
		else:
			s = "0"
		return s

	def display_hint( self ):
		return 'string'

class YaalHCoreHVariantPrinter:
	"Print a yaal::hcore::HVariant"

	def __init__( self, val ):
		self._val = val

	def to_string( self ):
		val = "<uninitialized variant>"
		typeid = self._val['_type']
		if typeid != -1:
			val = self._val['_mem'].cast( self._val.type.template_argument( typeid ).pointer() ).dereference()
		return val

	def display_hint( self ):
		return 'string'

gdb.pretty_printers.append( yaal_lookup_function )

