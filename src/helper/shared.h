//
// Created by alik on 5/19/18.
//

#pragma once
#ifndef FAKESERVERQUERIES_SHARED_H
#define FAKESERVERQUERIES_SHARED_H

#if __has_include(<boost/smart_ptr/local_shared_ptr.hpp>)
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>

namespace fsq
{
    template<typename T>
    using shared_ptr = boost::local_shared_ptr<T>;

    template<class T, class... Args>
    inline auto make_shared(Args&&... args)
    {
        return boost::make_local_shared<T>(std::forward<Args>(args)...);
    }
}
#else
#include <memory>

	namespace fsq
	{
		using std::shared_ptr;
		using std::make_shared;
	}
#endif

#define DEFINE_CLASS_SHARED(X) \
class X; \
using X##Ref = fsq::shared_ptr<X>

#endif //FAKESERVERQUERIES_SHARED_H
