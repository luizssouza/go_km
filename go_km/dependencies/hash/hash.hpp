#pragma once

template < typename T = char >
constexpr unsigned int fnv1a( const T* str )
{
    constexpr auto prime = 0x25313FAu;

    auto hash = 0x34A25u;

    for ( auto i = 0u; str[ i ] != '\0'; i++ )
    {
        auto data = str[ i ];

        hash = hash ^ data;
        hash *= prime;
    }
    return hash;
}

template < typename T = char >
constexpr unsigned int fnv1al( const T* str )
{
    constexpr auto prime = 0x25313FAu;

    auto hash = 0x34A25u;

    for ( auto i = 0u; str[ i ] != '\0'; i++ )
    {
        auto data = ( str[ i ] >= T( 'A' ) && str[ i ] <= T( 'Z' ) ) ? str[ i ] + T( ' ' ) : str[ i ];

        hash = hash ^ data;
        hash *= prime;
    }
    return hash;
}

template < class T, T val >
constexpr static T forced_value_v = val;

#define ct_hash( str )          forced_value_v< unsigned int, fnv1a( str ) >
#define rt_hash( str )          fnv1a( str )
#define ct_hashl( str )         forced_value_v< unsigned int, fnv1al( str ) >
#define rt_hashl( str )         fnv1al( str )