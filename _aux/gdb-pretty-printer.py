#! /usr/bin/env python

import re

class YaalHCoreHStringPrinter:
	"Print a yaal::hcore::HString"

	def __init__( self, val ):
		self._val = val

	def to_string( self ):
		return self._val['_buffer']

	def display_hint( self ):
		return 'string'

def yaal_lookup_function( val ):
	lookup_tag = val.type.tag
	regex = re.compile( "^yaal::hcore::HString$" )
	if lookup_tag == None:
		return None
	if regex.match( lookup_tag ):
		return YaalHCoreHStringPrinter( val )
	regex = re.compile( "^yaal::hcore::HArray<.*>$" )
	if regex.match( lookup_tag ):
		return YaalHCoreHArrayPrinter( val )

	return None

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
		return ( 'yaal::hcore::HArray of %s of length %d, capacity %d' % ( self.val.type.template_argument( 0 ), self.val['_size'], self.capacity() ) )

	def display_hint( self ):
		return 'array'

gdb.pretty_printers.append( yaal_lookup_function )

