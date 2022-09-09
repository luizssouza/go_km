#pragma once

class hash_number
{
    using u64 = unsigned long long;
public:

    static constexpr auto hash_key_1 = 3592423423231234ull;
    static constexpr auto hash_key_2 = 2134230912390ull;

    __forceinline constexpr hash_number( const u64 number ) :
        _number( ( number ^ hash_key_1 ) ^ hash_key_2 )
    {
    }

    __declspec( noinline ) volatile u64 number( )
    {
        volatile auto n1 = hash_key_1;
        volatile auto n2 = hash_key_2;
        return ( *( u64* )&_number ^ *( u64* )&n1 ) ^ *( u64* )&n2;
    }

private:
    volatile u64 _number;
};

#define hash_num( num )     hash_number{ num }.number()