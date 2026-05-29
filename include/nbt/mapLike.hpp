#pragma once
#include <concepts>
#include <ranges>
#include <string>

namespace NBT::MapLike {
    using std::convertible_to, std::string, std::ranges::range;

    namespace detail {
        template <typename T>
        concept Shape = range<T> && range<const T> && requires(T t, const T constT, const string& key) {
            typename T::value_type;

            t.find(key);
            t.end();
            { t.find(key) == t.end() } -> convertible_to<bool>;

            constT.find(key);
            constT.end();
            { constT.find(key) == constT.end() } -> convertible_to<bool>;
        };
    }

    template <typename P>
    concept MapLike = requires { typename P::template map<string, int>; } && detail::Shape<typename P::template map<string, int>>;
}