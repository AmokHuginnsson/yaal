/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/random.hxx
 * \brief Declaration of random namespace and HRandomNumberGenerator class.
 */

#ifndef YAAL_HCORE_RANDOM_HXX_INCLUDED
#define YAAL_HCORE_RANDOM_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

namespace yaal {

namespace random {

/*! \brief (Pseudo)Random number generator.
 *
 * Congruent pseudo random number generator class.
 */
class HRandomNumberGenerator final {
public:
	typedef HRandomNumberGenerator this_type;
	static int const STATE_SIZE = 312;
private:
	int _index;
	u64_t _state[STATE_SIZE];
public:
/*! \brief Construct new randomizer.
 *
 * \param seed - initialize pseudo-random number generator with seed.
 * \param range - upper limit for generated number.
 */
	HRandomNumberGenerator( u64_t seed = 5489ULL );

/*! \brief Construct new randomizer.
 *
 * \param stateFirst_ - beginning of pseudo-random number generator initial state range.
 * \param stateLast_ - one past the end of pseudo-random number generator initial state range.
 * \param range - upper limit for generated number.
 */
	HRandomNumberGenerator( u64_t const* stateFirst_, u64_t const* stateLast_ );

/*! \brief Re-initialize generator with new seed.
 */
	void set_seed( yaal::u64_t );

/*! \brief Generate next random number.
 *
 * \param range - upper limit for generated number.
 * \return next random number capped to randomizer instance default value.
 */
	u64_t operator()( void );

	void swap( HRandomNumberGenerator& );
private:
	void init( u64_t );
	void init( u64_t const*, int );
};

namespace distribution {

class HDistribution {
protected:
	HRandomNumberGenerator _rng;
public:
	HDistribution( void );
	virtual ~HDistribution( void ) {}
	void set_seed( yaal::u64_t );

/*! \brief Generate next random number from discrete distribution.
 *
 * \return next random number from this discrete distribution.
 */
	yaal::i64_t next_discrete( void ) {
		return ( do_next_discrete() );
	}
	double long next_continuous( void ) {
		return ( do_next_continuous() );
	}
protected:
	virtual yaal::i64_t do_next_discrete( void ) = 0;
	virtual double long do_next_continuous( void ) = 0;
};

class HDiscrete : public HDistribution {
private:
	yaal::i64_t _base;
	yaal::u64_t _num;
public:
	HDiscrete( yaal::i64_t from_, yaal::i64_t to_ );
	i64_t operator()( void );
	u64_t range( void ) const;
protected:
	virtual yaal::i64_t do_next_discrete( void ) override;
	virtual double long do_next_continuous( void ) override;
};

class HUniform : public HDistribution {
public:
	HUniform( double long lower_, double long upper_ );
	double long operator()( void );
protected:
	virtual yaal::i64_t do_next_discrete( void ) override;
	virtual double long do_next_continuous( void ) override;
};

class HTriangle : public HDistribution {
public:
	HTriangle( double long lower_, double long upper_, double long mode_ );
	double long operator()( void );
protected:
	virtual yaal::i64_t do_next_discrete( void ) override;
	virtual double long do_next_continuous( void ) override;
};

class HGaussian : public HDistribution {
public:
	HGaussian( double long mu_, double long sigma_ );
	double long operator()( void );
protected:
	virtual yaal::i64_t do_next_discrete( void ) override;
	virtual double long do_next_continuous( void ) override;
};

}

/*! \brief Helper namespace for HRandomNumberGenerator related utilities.
 */
namespace rng_helper {

/*! \brief Make randomizer object initialized with "random" seed.
 *
 * \param range - upper limit for generated numbers.
 */
distribution::HDiscrete make_random_number_generator( i64_t range = meta::max_signed<i64_t>::value );

}

}

}

#endif /* #ifndef YAAL_HCORE_RANDOM_HXX_INCLUDED */

