#ifndef CANARD_ASIO_OPERATION_HOLDER_HPP
#define CANARD_ASIO_OPERATION_HOLDER_HPP

#include <memory>
#include <utility>
#include <boost/asio/handler_alloc_hook.hpp>

namespace canard {
namespace detail {

    template <class Handler, class Operation>
    struct op_holder
    {
        Handler* h;
        void* ptr;
        Operation* op;

        explicit op_holder(Handler& handler)
            : h(std::addressof(handler))
            , ptr(allocate(h))
            , op(nullptr)
        {
        }

        op_holder(Handler& handler, Operation* const operation)
            : h(std::addressof(handler))
            , ptr(operation)
            , op(operation)
        {
        }

        ~op_holder()
        {
            reset();
        }

        template <class... Args>
        auto construct(Args&&... args)
            -> Operation*
        {
            return op = new(ptr) Operation{std::forward<Args>(args)...};
        }

        void handler(Handler& handler)
        {
            h = std::addressof(handler);
        }

        auto release()
            -> Operation*
        {
            ptr = nullptr;
            auto const operation = op;
            op = nullptr;
            return operation;
        }

        void reset()
        {
            if (op) {
                op->~Operation();
                op = nullptr;
            }
            if (ptr) {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(ptr, sizeof(Operation), h);
                ptr = nullptr;
            }
        }

    private:
        static auto allocate(Handler* const handler_ptr)
            -> void*
        {
            using boost::asio::asio_handler_allocate;
            return asio_handler_allocate(sizeof(Operation), handler_ptr);
        }
    };

} // namespace detail
} // namespace canard

#endif // CANARD_ASIO_OPERATION_HOLDER_HPP
