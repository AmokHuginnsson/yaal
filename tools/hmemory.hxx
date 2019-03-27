/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HMEMORY_HXX_INCLUDED
#define YAAL_TOOLS_HMEMORY_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

class HMemoryHandlingStrategyInterface {
public:
	int long get_size( void ) const {
		return ( do_get_size() );
	}
	void* get_memory( void ) const {
		return ( do_get_memory() );
	}
	int long commit( int long valid_, int long read_, int long write_, int long size_ ) {
		return ( do_commit( valid_, read_, write_, size_ ) );
	}
	virtual ~HMemoryHandlingStrategyInterface( void ) {
		return;
	}
protected:
	virtual int long do_get_size( void ) const = 0;
	virtual void* do_get_memory( void ) const = 0;
	virtual int long do_commit( int long, int long, int long, int long ) = 0;
};

class HMemoryObserver : public HMemoryHandlingStrategyInterface {
	void* _block;
	int long const _size;
public:
	/*! \brief Create new memory accessor.
	 *
	 * \param ptr - pointer to memory block to be wrapped.
	 * \param size - size of memory block in octets.
	 */
	HMemoryObserver( void* ptr, int long size )
		: _block( ptr )
		, _size( size ) {
		M_ASSERT( size > 0 );
		M_ASSERT( ptr );
	}
protected:
	virtual int long do_get_size( void ) const override {
		return ( _size );
	}
	virtual void* do_get_memory( void ) const override {
		return ( _block );
	}
	virtual int long do_commit( int long, int long, int long, int long ) override;
private:
	HMemoryObserver( HMemoryObserver const& ) = delete;
	HMemoryObserver& operator = ( HMemoryObserver const& ) = delete;
};

class HMemoryProvider : public HMemoryHandlingStrategyInterface {
	yaal::hcore::HChunk& _chunk;
	int long _size;
public:
	HMemoryProvider( yaal::hcore::HChunk& chunk_, int long size_ )
		: _chunk( chunk_ )
		, _size( size_ ) {
	}
protected:
	virtual int long do_get_size( void ) const override {
		return ( _size );
	}
	virtual void* do_get_memory( void ) const override {
		return ( _chunk.raw() );
	}
	virtual int long do_commit( int long, int long, int long, int long ) override;
private:
	HMemoryProvider( HMemoryProvider const& ) = delete;
	HMemoryProvider& operator = ( HMemoryProvider const& ) = delete;
};

/*! \brief Stream based access to raw memory block.
 *
 * This stream based memory accessor wrapper does not own the memory block.
 */
class HMemory : public yaal::hcore::HStreamInterface {
public:
	typedef HMemory this_type;
	typedef yaal::hcore::HStreamInterface base_type;
	enum class INITIAL_STATE {
		AUTO,
		VALID,
		INVALID
	};
private:
	HMemoryHandlingStrategyInterface& _memory;
	int long _valid;
	int long _cursorRead;
	int long _cursorWrite;
public:
	HMemory( HMemoryHandlingStrategyInterface& memory_, INITIAL_STATE = INITIAL_STATE::AUTO );
	/*! \brief Destructor.
	 */
	virtual ~HMemory( void );
	/*! \brief Check if two blocks of memory have identical contents.
	 *
	 * \param other - check for equality with this block.
	 * \return true iff other has same contents as this.
	 */
	bool operator == ( HMemory const& other ) const;
	int long valid_octets( void ) const
		{ return ( _valid ); }
private:
	virtual int long do_write( void const*, int long ) override;
	virtual void do_flush( void ) override;
	virtual int long do_read( void*, int long ) override;
	virtual void do_seek( int long, SEEK ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	HMemory( HMemory const& src ) = delete;
	HMemory& operator = ( HMemory const& src ) = delete;
};

typedef yaal::hcore::HExceptionT<HMemory, yaal::hcore::HStreamInterfaceException> HMemoryException;

}

}

#endif /* #ifndef YAAL_TOOLS_HMEMORY_HXX_INCLUDED */

