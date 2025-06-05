#pragma once

#define TR_EXPAND_MACRO(x) x
#define TR_STRINGIFY_MACRO(x) #x


#define BIT(x) (1 << x)

#define TR_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

