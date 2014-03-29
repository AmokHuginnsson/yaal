#! /usr/bin/env python

import re

def yaal_lookup_function( val_ ):
	lookup_tag = val_.type.strip_typedefs().tag
	if lookup_tag == None:
		return None
	regex = re.compile( "^yaal::hcore::HPointer<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHPointerPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HResource<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHResourcePrinter( val_ )
	regex = re.compile( "^yaal::hcore::HString$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHStringPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HPair<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHPairPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HArray<.*>::HIterator<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHArrayHIteratorPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HArray<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHArrayPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HDeque<.*>::HIterator<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHDequeHIteratorPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HDeque<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHDequePrinter( val_ )
	regex = re.compile( "^yaal::hcore::HList<.*>::HIterator<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHListHIteratorPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HList<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHListPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HMap<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHMapPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HSet<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHSetPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HHashMap<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHHashMapPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HHashSet<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHHashSetPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HNumber$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHNumberPrinter( val_ )
	regex = re.compile( "^yaal::hcore::HVariant<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHVariantPrinter( val_ )
	regex = re.compile( "^yaal::tools::HRing<.*>$" )
	if regex.match( lookup_tag ):
		return YaalToolsHRingPrinter( val_ )

	return None

class YaalHCoreHPointerPrinter:
	"Print a yaal::hcore::HPointer"

	def __init__( self, val_ ):
		self._val = val_
		self._ptr = 0
		shared = self._val['_shared']
		if shared != 0:
			self._ptr = self._val['_object']
			if self._ptr != 0:
				self._innerPrinter = yaal_lookup_function( self._ptr.dereference() )
				if self._innerPrinter != None:
					if hasattr( self._innerPrinter, 'children' ):
						def children( self ):
							return self._innerPrinter.children()
						setattr( self.__class__, 'children', children )

	def to_string( self ):
		s = "NULL"
		if self._ptr != 0:
			if self._innerPrinter != None:
				s = self._innerPrinter.to_string()
			else:
				s = self._ptr.dereference()
		return s

	def display_hint( self ):
		s = "string";
		if ( self._ptr != 0 ) and ( self._innerPrinter != None ):
			s = self._innerPrinter.display_hint()
		return s

class YaalHCoreHResourcePrinter:
	"Print a yaal::hcore::HResource"

	def __init__( self, val_ ):
		self._val = val_
		self._ptr = self._val['_resource']
		if self._ptr != 0:
			self._innerPrinter = yaal_lookup_function( self._ptr.dereference() )
			if self._innerPrinter != None:
				if hasattr( self._innerPrinter, 'children' ):
					def children( self ):
						return self._innerPrinter.children()
					setattr( self.__class__, 'children', children )

	def to_string( self ):
		s = "NULL"
		if self._ptr != 0:
			if self._innerPrinter != None:
				s = self._innerPrinter.to_string()
			else:
				s = self._ptr.dereference()
		return s

	def display_hint( self ):
		s = "string";
		if ( self._ptr != 0 ) and ( self._innerPrinter != None ):
			s = self._innerPrinter.display_hint()
		return s

class YaalHCoreHStringPrinter:
	"Print a yaal::hcore::HString"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		inplace = not ( self._val['_mem'][self._val['_mem'].type.sizeof - 1] & 128 )
		s = ""
		if inplace:
			s = self._val['_mem']
		else:
			s = self._val['_mem'].cast( gdb.lookup_type( 'char' ).pointer().pointer() ).dereference()
		return s

	def display_hint( self ):
		return 'string'

class YaalHCoreHPairPrinter:
	"Print a yaal::hcore::HPair"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		s = str( self._val['first'] ) + "," + str( self._val['second'] ) + ">"
		return "<" + s.strip()

	def display_hint( self ):
		return 'string'

class YaalHCoreHArrayHIteratorPrinter:
	"Print a yaal::hcore::HArray::HIterator"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		ptr = self._val['_owner']['_buf']
		index = self._val['_index']
		return "{0x%x,%d,%d,%s}" % ( ptr, index, self._val['_owner']['_size'], ( ptr + index ).dereference() )

	def display_hint( self ):
		return 'string'

class YaalHCoreHArrayPrinter:
	"Print a yaal::hcore::HArray"

	class Iterator:
		def __init__ (self, start, size):
			self._item = start
			self._size = size
			self._count = 0

		def __iter__(self):
			return self

		def __next__( self ):
			return ( self.next() )

		def next(self):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			self._count = self._count + 1
			elt = self._item.dereference()
			self._item = self._item + 1
			return ( '[%d]' % count, elt )

	def __init__(self, val_):
		self._val = val_
	
	def sizeof_elem( self ):
		return self._val.type.template_argument( 0 ).sizeof

	def children( self ):
		return self.Iterator(self._val['_buf'], self._val['_size'])

	def to_string( self ):
		return ( "yaal::hcore::HArray of `%s' of length %d, capacity %d" % ( self._val.type.template_argument( 0 ), self._val['_size'], self._val['_capacity'] ) )

	def display_hint( self ):
		return 'array'

class YaalHCoreHDequeHIteratorPrinter:
	"Print a yaal::hcore::HDeque::HIterator"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		VPC = self._val['_owner']['VALUES_PER_CHUNK']
		ptr = self._val['_owner']
		chunks = ptr['_chunks']['_data'].cast( self._val.type.template_argument( 0 ).pointer().pointer() )
		index = self._val['_index']
		start = ptr['_start']
		physical = index + start
		chunk = chunks[physical / VPC]
		if chunk != 0:
			return "{0x%x,%d,%d,%s}" % ( ptr, index, self._val['_owner']['_size'], chunk[physical % VPC] )
		return "{0x%x,%d,%d,(NIL)}" % ( ptr, index, self._val['_owner']['_size'] )

	def display_hint( self ):
		return 'string'

class YaalHCoreHDequePrinter:
	"Print a yaal::hcore::HDeque"

	class Iterator:
		def __init__( self, chunks_, VPC_, index_, size_ ):
			self._VPC = VPC_
			self._chunks = chunks_
			self._index = index_
			self._size = size_
			self._count = 0

		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			self._count = self._count + 1
			chunkIdx = self._index / self._VPC
			chunk = self._chunks[ chunkIdx ]
			elt = chunk[ self._index % self._VPC ] if chunk != 0 else "{(NIL) %d chunk}" % chunkIdx
			self._index = self._index + 1
			return ( '[%d]' % count, elt )

	def __init__( self, val_ ):
		self._val = val_
		self._VPC = abs( self._val['VALUES_PER_CHUNK'] )
	
	def children( self ):
		return self.Iterator( self._val['_chunks']['_data'].cast( self._val.type.template_argument( 0 ).pointer().pointer() ), self._VPC, self._val['_start'], self._val['_size'] )

	def to_string( self ):
		return ( "yaal::hcore::HDeque of `%s' of length %d of VPC %d" % ( self._val.type.template_argument( 0 ), self._val['_size'], self._VPC ) )

	def display_hint( self ):
		return 'array'

class YaalHCoreHListHIteratorPrinter:
	"Print a yaal::hcore::HList::HIterator"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		ptr = self._val['_owner']
		current = self._val['_current']
		if ptr != 0:
			if current != 0:
				return "{0x%x,%d,%s}" % ( ptr, ptr['_size'], current['_value'] )
			else:
				return "{0x%x,%d,(NIL)}" % ( ptr, ptr['_size'] )
		return "{0x0,0,(NIL)}"

	def display_hint( self ):
		return 'string'

class YaalHCoreHListPrinter:
	"Print a yaal::hcore::HList"

	class Iterator:
		def __init__( self, start_, size_ ):
			self._item = start_
			self._size = size_
			self._count = 0

		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			self._count = self._count + 1
			elt = self._item['_value']
			self._item = self._item['_next']
			return ('[%d]' % count, elt)

	def __init__( self, val_ ):
		self._val = val_
	
	def children( self ):
		return self.Iterator(self._val['_hook'], self._val['_size'])

	def to_string( self ):
		return ( "yaal::hcore::HList of `%s' of length %d" % ( self._val.type.template_argument( 0 ), self._val['_size'] ) )

	def display_hint( self ):
		return 'array'

class BinaryTreeIterator( object ):
	def __init__( self, owner_, start_, size_ ):
		self._owner = owner_
		self._item = start_
		self._size = size_
		self._count = 0
	def do_next( self, it_ ):
		lastNode = it_
		while it_ != 0:
			if ( it_['_right'] != 0 ) and ( it_['_right'] != lastNode ):
				it_ = it_['_right']
				while ( it_['_left'] ):
					it_ = it_['_left']
				break
			else:
				lastNode = it_
				it_ = it_['_parent']
				if ( it_ != 0 ) and ( lastNode == it_['_left'] ):
					break
		return it_

class YaalHCoreHMapPrinter:
	"Print a yaal::hcore::HMap"

	class Iterator( BinaryTreeIterator ):

		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == ( self._size * 2 ):
				raise StopIteration
			count = self._count
			valueType = gdb.lookup_type( "yaal::hcore::HPair<%s, %s>" % ( self._owner.type.template_argument( 0 ).const(), self._owner.type.template_argument( 1 ) ) )
			compareType = self._owner.type.template_argument( 2 )
			helperType = "yaal::hcore::map_helper<%s,%s>" % ( self._owner.type.template_argument( 0 ), self._owner.type.template_argument( 1 ) )
			allocatorType = self._owner.type.template_argument( 3 )
			nodeType = gdb.lookup_type( "yaal::hcore::HSBBSTree<%s,%s,%s,%s>::HNode" % ( valueType, compareType, helperType, allocatorType ) ).pointer()
			if ( count % 2 ) == 0:
				elt = self._item.cast( nodeType )['_key']['first']
			else:
				elt = self._item.cast( nodeType )['_key']['second']
				self._item = self.do_next( self._item )
			self._count = self._count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val_ ):
		self._val = val_

	def begin( self ):
		node = self._val['_engine']['_root']
		if self._val['_engine']['_size'] > 0:
			while node['_left'] != 0:
				node = node['_left']
		return node

	def children( self ):
		return self.Iterator( self._val, self.begin(), self._val['_engine']['_size'] )

	def to_string( self ):
		return ( "yaal::hcore::HMap of `%s' to `%s' of length %d" % ( self._val.type.template_argument( 0 ), self._val.type.template_argument( 1 ), self._val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'map'

class YaalHCoreHSetPrinter:
	"Print a yaal::hcore::HSet"

	class Iterator( BinaryTreeIterator ):

		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			valueType = self._owner.type.template_argument( 0 )
			compareType = self._owner.type.template_argument( 1 )
			allocatorType = self._owner.type.template_argument( 2 )
			nodeType = gdb.lookup_type( "yaal::hcore::HSBBSTree<%s,%s,yaal::hcore::set_helper<%s>,%s>::HNode" % ( valueType, compareType, valueType, allocatorType ) ).pointer()
			elt = self._item.cast( nodeType )['_key']
			self._item = self.do_next( self._item )
			self._count = self._count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val_ ):
		self._val = val_

	def begin( self ):
		node = self._val['_engine']['_root']
		if self._val['_engine']['_size'] > 0:
			while node['_left'] != 0:
				node = node['_left']
		return node

	def children( self ):
		return self.Iterator( self._val, self.begin(), self._val['_engine']['_size'] )

	def to_string( self ):
		return ( "yaal::hcore::HSet of `%s' of length %d" % ( self._val.type.template_argument( 0 ), self._val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'array'

class HashContainterIterator( object ):
	def __init__( self, owner_, atom_, index_, size_ ):
		self._owner = owner_
		self._atom = atom_
		self._index = index_
		self._size = size_
		self._count = 0

	def do_next( self ):
		if self._atom != 0:
			self._atom = self._atom['_next']
			if self._atom == 0:
				self._index = self._index + 1
		if self._atom == 0:
			buckets = self._owner.buckets()
			while ( self._index < self._owner._val['_engine']['_prime'] ) and ( buckets[ self._index ] == 0 ):
				self._index = self._index + 1
			if self._index < self._owner._val['_engine']['_prime']:
				self._atom = buckets[ self._index ]
		if self._atom == 0:
			self._index = self._owner._val['_engine']['_prime']


class YaalHCoreHHashMapPrinter:
	"Print a yaal::hcore::HHashMap"

	class Iterator( HashContainterIterator ):
		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == ( self._size * 2 ):
				raise StopIteration
			count = self._count
			nodeType = self._owner.nodeType()
			if ( count % 2 ) == 0:
				elt = self._atom.cast( nodeType )['_value']['first']
			else:
				elt = self._atom.cast( nodeType )['_value']['second']
				self.do_next()
			self._count = self._count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val_ ):
		self._val = val_

	def nodeType( self ):
		keyType = self._val.type.template_argument( 0 )
		dataType = self._val.type.template_argument( 1 )
		valueType = gdb.lookup_type( "yaal::hcore::HPair<%s,%s>" % ( keyType.const(), dataType ) )
		hasherType = self._val.type.template_argument( 2 );
		allocatorType = self._val.type.template_argument( 3 );
		nodeType = gdb.lookup_type( "yaal::hcore::HHashContainer<%s,%s,yaal::hcore::hashmap_helper<%s,%s>,%s>::HAtom" % ( valueType, hasherType, keyType, dataType, allocatorType ) ).pointer()
		return nodeType

	def buckets( self ):
		buckets = self._val['_engine']['_buckets']['_data']
		return buckets.cast( self.nodeType().pointer() )

	def children( self ):
		it = self.Iterator( self, 0, 0, self._val['_engine']['_size'] )
		it.do_next()
		return it

	def to_string( self ):
		return ( "yaal::hcore::HHashMap of `%s' to `%s' of length %d" % ( self._val.type.template_argument( 0 ), self._val.type.template_argument( 1 ), self._val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'map'

class YaalHCoreHHashSetPrinter:
	"Print a yaal::hcore::HHashSet"

	class Iterator( HashContainterIterator ):
		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			nodeType = self._owner.nodeType()
			elt = self._atom.cast( nodeType )['_value']
			self.do_next()
			self._count = self._count + 1
			return ('[%d]' % count, elt)

	def __init__( self, val_ ):
		self._val = val_

	def nodeType( self ):
		valueType = self._val.type.template_argument( 0 );
		hasherType = self._val.type.template_argument( 1 );
		allocatorType = self._val.type.template_argument( 2 );
		nodeType = gdb.lookup_type( "yaal::hcore::HHashContainer<%s,%s,yaal::hcore::hashset_helper<%s>,%s>::HAtom" % ( valueType, hasherType, valueType, allocatorType ) ).pointer()
		return nodeType

	def buckets( self ):
		buckets = self._val['_engine']['_buckets']['_data']
		return buckets.cast( self.nodeType().pointer() )

	def children( self ):
		it = self.Iterator( self, 0, 0, self._val['_engine']['_size'] )
		it.do_next()
		return it

	def to_string( self ):
		return ( "yaal::hcore::HHashSet of `%s' of length %d" % ( self._val.type.template_argument( 0 ), self._val['_engine']['_size'] ) )

	def display_hint( self ):
		return 'array'

class YaalHCoreHNumberPrinter:
	"Print a yaal::hcore::HNumber"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		s = ""
		DDIL = gdb.parse_and_eval( "yaal::hcore::HNumber::DECIMAL_DIGITS_IN_LEAF" )
		if self._val['_leafCount'] > 0:
			if self._val['_negative']:
				s = s + "-"
			digit = 0
			data = self._val['_canonical']['_data'].cast( gdb.lookup_type( 'yaal::i32_t' ).pointer() )
			while digit < self._val['_integralPartSize']:
				s = s + ( str( data[digit] ).zfill( DDIL ) if ( digit > 0 ) else str( data[digit] ) )
				digit = digit + 1
			if self._val['_leafCount'] > self._val['_integralPartSize']:
				if self._val['_integralPartSize'] == 0:
					s = s + "0"
				s = s + "."
			while digit < self._val['_leafCount']:
				s = s + str( data[digit] ).zfill( DDIL )
				digit = digit + 1
			if self._val['_leafCount'] > self._val['_integralPartSize']:
				s = s.rstrip( "0" )
		else:
			s = "0"
		return s

	def display_hint( self ):
		return 'string'

class YaalHCoreHVariantPrinter:
	"Print a yaal::hcore::HVariant"

	def __init__( self, val_ ):
		self._val = val_

	def to_string( self ):
		val = "<uninitialized variant>"
		typeid = self._val['_type']
		if typeid != -1:
			val = self._val['_mem'].cast( self._val.type.template_argument( typeid ).pointer() ).dereference()
		return val

	def display_hint( self ):
		return 'string'

class YaalToolsHRingPrinter:
	"Print a yaal::tools::HRing"

	class Iterator:
		def __init__( self, data_, index_, size_, capacity_ ):
			self._data = data_
			self._index = index_
			self._size = size_
			self._capacity = capacity_ 
			self._count = 0

		def __iter__( self ):
			return self

		def __next__( self ):
			return ( self.next() )

		def next( self ):
			if self._count == self._size:
				raise StopIteration
			count = self._count
			self._count = self._count + 1
			elt = self._data[ self._index % self._capacity ]
			self._index = self._index + 1
			return ( '[%d]' % count, elt )

	def __init__( self, val_ ):
		self._val = val_
	
	def sizeof_elem( self ):
		return self._val.type.template_argument( 0 ).sizeof

	def capacity( self ):
		return self._val['_buf']['_size'] / self.sizeof_elem()

	def children( self ):
		return self.Iterator( self._val['_buf']['_data'].cast( self._val.type.template_argument( 0 ).pointer() ), self._val['_start'], self._val['_size'], self.capacity() )

	def to_string( self ):
		return ( "yaal::tools::HRing of `%s' of length %d, capacity %d" % ( self._val.type.template_argument( 0 ), self._val['_size'], self.capacity() ) )

	def display_hint( self ):
		return 'array'

gdb.pretty_printers.append( yaal_lookup_function )

