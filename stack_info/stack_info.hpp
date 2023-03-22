/**
 * @file stack_info.hpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Defines the `stack_info` class and associated subtypes.
 *
 */

#include <memory>

namespace stack_info {
    /**
     * @brief Records information about the stack.
     *
     */
    class stack_info {
    private:
        /**
         * @brief An integer stack.
         *
         */
        class int_stack {
        private:
            /**
             * @brief A data node.
             *
             */
            struct node {
                /**
                 * @brief A pointer to the next node, or `nullptr`.
                 *
                 */
                std::unique_ptr<node> last;

                /**
                 * @brief The value of this node.
                 *
                 */
                const unsigned int value;
            };

            /**
             * @brief A pointer to the top of the stack, or nullptr.
             *
             */
            std::unique_ptr<node> last;

            /**
             * @brief The total of the elements in the stack.
             *
             */
            unsigned int total;

        public:
            /**
             * @brief Class constructor.
             *
             */
            int_stack();

            /**
             * @brief Reports the total of the elements in the stack.
             *
             * @return The total of the elements in the stack.
             */
            [[nodiscard]] unsigned int get_total() const;

            /**
             * @brief Pops the top element from the stack.
             *
             * @return The value of the top element on the stack.
             */
            unsigned int pop();

            /**
             * @brief Pushes a value onto the stack.
             *
             * @param value The value to push onto the stack.
             */
            void push(unsigned int value);
        };

        /**
         * @brief The default number of bytes to push onto the stack.
         *
         */
        static constexpr unsigned int default_num_bytes = 8;

        /**
         * @brief The stack must be aligned to this many bytes to call a function.
         *
         */
        static constexpr unsigned int stack_alignment = 16;

        /**
         * @brief The stack of integers.
         *
         */
        int_stack stack;

    public:
        /**
         * @brief Reports whether the stack needs to be 16-byte aligned.
         *
         * @return True if the stack needs to be 16-byte aligned.
         */
        [[nodiscard]] bool needs_alignment() const;

        /**
         * @brief Pops the top value from the stack.
         *
         * @return The top value on the stack.
         */
        unsigned int pop();

        /**
         * @brief Pushes the given number of bytes onto the stack.
         *
         * @param num_bytes The number of bytes to push onto the stack. Defaults to 8.
         */
        void push(unsigned int num_bytes = default_num_bytes);
    };
}  //  namespace stack_info
