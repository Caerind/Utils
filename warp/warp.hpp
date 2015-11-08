// Warp: a handy string interpolator (C++11)
// - rlyeh, zlib/libpng licensed.

/*

# Warp :recycle: <a href="https://travis-ci.org/r-lyeh/warp"><img src="https://api.travis-ci.org/r-lyeh/warp.svg?branch=master" align="right" /></a>
- Warp is a handy string interpolator (C++11).
- Warp is cross-platform, lightweight and header-only.
- Warp is zlib/libpng licensed.

## Quick tutorial

- Simple interpolation usage:
  - Use `$(symbol)` macro to create or update a symbol.
  - Use `$$(string)` macro to translate all symbols into a string.
  - `$symbols` are valid until they're destroyed (past end of scope).
```c++
{
    $(WORLD) = "world";
    assert( $$("Hello $WORLD") == "Hello world" );
}
assert( $$("Hello $WORLD") == "Hello $WORLD" );
```

- Valid $symbols are replaced, everything else is quoted.
```c++
$(PLAYER_1) = "Mark";
$(PLAYER_2) = "Karl";

assert( $$("$PLAYER_1 and $PLAYER_2 logged in") == "Mark and Karl logged in");
assert( $$("$PLAYER_3 logged out") == "$PLAYER_3 logged out");
```

- You can reassign and update symbols as many times as needed.
```c++
$(PLAYER_1) = "Mike";
$(PLAYER_2) = "John";
assert( $$("$PLAYER_1 and $PLAYER_2 logged in") == "Mike and John logged in");
```

- Composition and symbol chaining is supported through dynamic lookups.
```c++
$(HEY) = "Hello stranger";
$(GREETING) = "$HEY! How are you?";
assert( $$("$GREETING") == "Hello stranger! How are you?" );
```

- However, recursive symbols are quoted to avoid recursive locks.
```c++
$(LOOPBACK) = "$LOOPBACK is unsafe, hence quoted";
assert( $$("$LOOPBACK") == "$LOOPBACK" );
```

- Symbol hot-swapping is supported as well.
```c++
$(HEY) = "Hey $PLAYER_1 and $PLAYER_2";
assert( $$("$GREETING") == "Hey Mike and John! How are you?" );
```

- Symbols are stringgs, and can hold values of many different types.
```c++
$(name) = "John Doe";   // strings
$(flag) = true;         // booleans
$(letter) = 'a';        // characters
$(items) = 100;         // integers
$(price) = 99.95f;      // floats
$(pi) = 3.141592;       // doubles
```

- Cast symbols to other types by using these additional macros.
```c++
assert( $bool(flag) == true );
assert( $char(letter) == 'a' );
assert( $int(items) * 2 == 200 );
assert( $string(name) + $string(name) == "John DoeJohn Doe" );
assert( $float(price) * $double(pi) > 300 );
```

- Casting to custom types is supported as well, by using $cast(type,symbol)
```c++
typedef int my_custom_type;
my_custom_type currency = $cast(my_custom_type, price);
assert( currency == 99 );
```

*/

/*

Copyright (c) 2015 r-lyeh (https://github.com/r-lyeh)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

#pragma once

#define WARP_VERSION "0.0.0" // (2015/08/09) Initial version

// public api, define/update

#if __COUNTER__ != __COUNTER__
#define $(...)     warp::symbol warp$joint(warp_symbol_,__COUNTER__) = warp::symbol( "$" #__VA_ARGS__ )
#else
#define $(...)     warp::symbol warp$joint(warp_symbol_,__LINE__) = warp::symbol( "$" #__VA_ARGS__ )
#endif

// public api, translate

#define $$(...)    warp::symbol()(__VA_ARGS__)

// public api, cast and sugars

#define $cast(t,x) warp::as<t>( $$("$" #x) )
#define $string(x) $cast( std::string, x )
#define $bool(x)   $cast( bool, x )
#define $char(x)   $cast( char, x )
#define $int(x)    $cast( int, x )
#define $float(x)  $cast( float, x )
#define $double(x) $cast( double, x )


// private api following

#include <map>
#include <sstream>
#include <string>

namespace warp {

// string conversion, taken from https://github.com/r-lyeh/wire {
struct string : public std::string {
    string( const std::string &s = std::string() ) : std::string( s )
    {}

    string( const char &c, size_t n = 1 ) : std::string( n, c )
    {}

    string( const char *cstr ) : std::string( cstr ? cstr : "" )
    {}

    string( char * const &cstr ) : std::string( cstr ? cstr : "" )
    {}

    string( const bool &t ) : std::string( t ? "true" : "false" )
    {}

    template< typename T >
    string( const T &t ) : std::string( std::to_string(t) )
    {}

    template<size_t N>
    string( const char (&cstr)[N] ) : std::string( cstr )
    {}
};
// }

// conversion casts, taken from https://github.com/r-lyeh/wire {
template< typename T > inline T as( const string &self ) {
    T t;
    return std::istringstream(self) >> t ? t :
            (T)(self.size() && (self != "0") && (self != "false"));
}
template<> inline char as( const string &self ) {
    return self.size() == 1 ? (char)(self[0]) : (char)(as<int>(self));
}
template<> inline signed char as( const string &self ) {
    return self.size() == 1 ? (signed char)(self[0]) : (signed char)(as<int>(self));
}
template<> inline unsigned char as( const string &self ) {
    return self.size() == 1 ? (unsigned char)(self[0]) : (unsigned char)(as<int>(self));
}
template<> inline const char *as( const string &self ) {
    return self.c_str();
}
template<> inline std::string as( const string &self ) {
    return self;
}
// }

class symbol {

    // registry of all existing symbols {
    enum { ADD, DEL, GET };
    using map = std::map< std::string, string >;
    static map &registry( int mode, std::string *key = 0, string *val = 0 ) {
        static map all;
        switch( mode ) { default:
            case GET: return all;
            case ADD: return all[ *key ] = ( val ? *val : all[ *key ] ), all;
            case DEL: {
                auto find = all.find(*key);
                return find == all.end() ? all : (all.erase( find ), all);
            }
        }
    }
    map::const_iterator cbegin() const { return registry(GET).cbegin(); }
    map::const_iterator cend()   const { return registry(GET).cend();   }
    // }

    std::string key;
    bool skip = false;

public:

    symbol()
    {}

    template<typename T>
    symbol( const T &key ) : key(key)
    {}
    template<size_t N>
    symbol( const char (&k)[N] ) : key(k)
    {}

    ~symbol() {
        if( !skip ) {
            registry( DEL, &key );
        }
    }

    symbol( const symbol &other ) {
        *this = other;
    }
    symbol &operator=( const symbol &other ) {
        if( &other != this ) {
            key = other.key;
        }
        return *this;
    }

    template<typename T>
    symbol &operator=( const T &other ) {
        registry( ADD, &key )[ key ];
        val() = other;
        skip = true;
        return *this;
    }
    template<size_t N>
    symbol &operator=( const char (&other)[N] ) {
        registry( ADD, &key )[ key ];
        val() = other;
        skip = true;
        return *this;
    }

    const string &val() const { 
        return registry(GET)[key];
    }
    string &val() {
        return registry(GET)[key];
    }

    template<typename IT>
    std::string warp( std::string out, IT begin, const IT &end ) const {
        auto replace = []( std::string &self, const std::string &target, const std::string &replacement ) {
            size_t found = 0, findings = 0;
            while( ( found = self.find( target, found ) ) != std::string::npos ) {
                self.replace( found, target.length(), replacement );
                found += replacement.length();
                findings++;
            }
            return findings;
        };
        for( auto it = begin; it != end; ) {
            const std::string &key = it->first;
            const string &val = it->second;
            ++it;
            // replace if valid && not loopback
            bool is_valid = key[0] == '$' && !val.empty();
            bool is_not_loopback = (val.find(key) == std::string::npos);
            if( is_valid && is_not_loopback ) {
                //std::cout << ";" << key << "=" << val << std::endl;
                if( replace( out, key, val ) ) {
                    //std::cout << ";;" << out << std::endl;
                    // restart chain on replacements (so new symbols with higher priorities in list get evaluated again)
                    it = begin;
                }
            }
        }
        return out;
    }

    operator std::string() const {
        return warp( key, cbegin(), cend() );
    }
    std::string operator()( const std::string &key ) const {
        return warp( key, cbegin(), cend() );
    }

    template<typename ostream>
    inline friend ostream & operator<< ( ostream &os, const symbol &self ) {
        return os << (std::string)self, os;
    }

    std::string debug() const {
        std::stringstream cout;
        cout << "symbols {" << std::endl;
        for( auto it = cbegin(), end = cend(); it != end; ++it ) {
            cout << it->first << '=' << it->second << std::endl;
        }
        cout << "} ---" << std::endl;
        return cout.str();
    }
};

}

#define warp$joint$(a,b) a##b
#define warp$joint(a,b)  warp$joint$(a,b)

