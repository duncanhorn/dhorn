/*
 * Duncan Horn 
 *
 * scanner.h
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../../unicode/iterator.h"

namespace dhorn::experimental::json
{
    /*
     * scan_position
     *
     * Represents the line, character, and absolute position of the input
     */
    struct scan_position
    {
        std::size_t line = 1;
        std::size_t column = 1;
        std::size_t absolute = 0;
    };



    /*
     * bad_json
     *
     * Exception that is thrown when invalid json is encountered while scanning input
     */
    class bad_json :
        public std::runtime_error
    {
    public:
        bad_json(const scan_position& pos, const char* message) :
            std::runtime_error(make_error_text(pos, message)),
            _pos(pos),
            _message(message)
        {
        }

        // Accessors
        const scan_position& position() const noexcept
        {
            return this->_pos;
        }

        const std::string& message() const noexcept
        {
            return this->_message;
        }

    private:

        static std::string make_error_text(const scan_position& pos, const char* message)
        {
            char buffer[512];
            std::snprintf(buffer, std::size(buffer), "bad_json: %s (line %zu, column %zu, position %zu)",
                message, pos.line, pos.column, pos.absolute);
            return buffer;
        }

        scan_position _pos;
        std::string _message;
    };



    namespace details
    {
        // Since the standard versions are non-constexpr/non-character-independent, provide our own
        inline constexpr bool in_range(char32_t ch, char32_t low, char32_t high) noexcept
        {
            assert(low <= high);
            return (ch >= low) && (ch <= high);
        }

        inline constexpr bool is_digit(char32_t ch) noexcept
        {
            return in_range(ch, '0', '9');
        }

        inline constexpr bool is_hex_digit(char32_t ch) noexcept
        {
            return is_digit(ch) || in_range(ch, 'A', 'F') || in_range(ch, 'a', 'f');
        }

        inline constexpr bool is_control(char32_t ch) noexcept
        {
            return ch < 32;
        }

        inline constexpr bool is_whitespace(char32_t ch) noexcept
        {
            return (ch == '\t') || in_range(ch, 10, 13) || (ch == ' ');
        }

        inline constexpr bool is_upper(char32_t ch) noexcept
        {
            return in_range(ch, 'A', 'Z');
        }

        inline constexpr bool is_lower(char32_t ch) noexcept
        {
            return in_range(ch, 'a', 'z');
        }

        inline constexpr bool is_alphabetical(char32_t ch) noexcept
        {
            return is_upper(ch) || is_lower(ch);
        }

        inline constexpr bool is_alphanumeric(char32_t ch) noexcept
        {
            return is_alphabetical(ch) || is_digit(ch);
        }

        inline constexpr bool is_separator(char32_t ch)
        {
            // Tokens can end with whitespace, ',', ':', '{', '}', '[', ']'
            switch (ch)
            {
            case U',':
            case U':':
            case U'{':
            case U'}':
            case U'[':
            case U']':
                return true;

            default:
                return is_whitespace(ch);
            }
        }

        inline constexpr char32_t hex_to_digit(char32_t ch) noexcept
        {
            assert(is_hex_digit(ch));
            return is_digit(ch) ? (ch - U'0') : is_upper(ch) ? (ch - U'A' + 10) : (ch - U'a' + 10);
        }



        /*
         * scan_state_tag
         *
         * Indicates the active value in a union of all *_scan_state types
         */
        enum class scan_state_tag : std::uint8_t
        {
            global,
            null,
            true_boolean,
            false_boolean,
            number,
            string,
            array,
            object,
        };



        /*
         * global_scan_state
         */
        struct global_scan_state
        {
            bool seen_value = false;

            bool on_valid_token()
            {
                return !std::exchange(seen_value, true);
            }
        };



        /*
         * token_scan_state
         *
         * Tracks state when parsing a well-known string in JSON input ("null", "true", or "false")
         */
        template <typename Derived>
        struct token_scan_state
        {
            std::size_t index = 0;

            constexpr bool consume(char32_t ch) noexcept
            {
                return !valid_terminal_state() && (Derived::expected_string[index++] == ch);
            }

            constexpr std::size_t size() noexcept
            {
                // NOTE: Terminating null, hence the minus one
                return std::size(Derived::expected_string) - 1;
            }

            constexpr bool valid_terminal_state() noexcept
            {
                return index >= size();
            }
        };

        /*
         * null_scan_state
         */
        struct null_scan_state :
            token_scan_state<null_scan_state>
        {
            static constexpr char32_t expected_string[] = U"null";
        };

        /*
         * true_scan_State
         */
        struct true_scan_state :
            token_scan_state<true_scan_state>
        {
            static constexpr char32_t expected_string[] = U"true";
        };

        /*
         * false_scan_state
         */
        struct false_scan_state :
            token_scan_state<false_scan_state>
        {
            static constexpr char32_t expected_string[] = U"false";
        };



        /*
         * number_scan_state
         */
        struct number_scan_state
        {
            static constexpr std::size_t npos = static_cast<std::size_t>(-1);

            std::size_t current_pos = 0;

            // NOTE: integral_start is always 0
            std::size_t integral_end = npos; // fractional, exponent, or end-of-string

            std::size_t fractional_start = npos;
            std::size_t fractional_end = npos; // exponent or end-of-string

            std::size_t exponent_start = npos;
            // NOTE: exponent end is always npos

            // NOTE: 'state' logically represents the edge coming out of the last character encountered
            enum class state
            {
                init,               // No characters encountered
                integral_sign,      // Number began with a leading '-'
                integral_zero,      // First digit encountered was a '0'
                integral,           // Reading digits in the integral part that did _not_ start with a '0'
                fractional_symbol,  // Encountered a '.' after the integral part
                fractional,         // Reading digits after the decimal point
                exponent_symbol,    // Encountered an 'e' or 'E' after the integral or fractional part
                exponent_sign,      // Encountered either a '+' or '-' after the exponent sign
                exponent,           // Reading digits after the exponent symbol/sign
            };

            state current_state = state::init;

            constexpr bool consume(char32_t ch) noexcept
            {
                if (ch == '-')
                {
                    switch (current_state)
                    {
                    case state::init:
                        current_state = state::integral_sign;
                        break;

                    case state::exponent_symbol:
                        current_state = state::exponent_sign;
                        exponent_start = current_pos; // Negative sign is part of the exponent
                        break;

                    default:
                        return false;
                    }
                }
                else if (ch == '+')
                {
                    switch (current_state)
                    {
                    case state::exponent_symbol:
                        current_state = state::exponent_sign;
                        exponent_start = current_pos + 1; // Positive sign is _not_ part of the exponent
                        break;

                    default:
                        return false;
                    }
                }
                else if (ch == '0')
                {
                    switch (current_state)
                    {
                    case state::init:
                    case state::integral_sign:
                        current_state = state::integral_zero;
                        break;

                    case state::fractional_symbol:
                        current_state = state::fractional;
                        fractional_start = current_pos;
                        break;

                    case state::exponent_symbol:
                        current_state = state::exponent;
                        exponent_start = current_pos;
                        break;

                    case state::exponent_sign:
                        current_state = state::exponent;
                        // NOTE: exponent_start should be set when we encounter the sign
                        assert(exponent_start != npos);
                        break;

                    case state::integral:
                    case state::fractional:
                    case state::exponent:
                        break;

                    default:
                            return false;
                    }
                }
                else if ((ch >= '1') && (ch <= '9'))
                {
                    switch (current_state)
                    {
                    case state::fractional_symbol:
                        current_state = state::fractional;
                        fractional_start = current_pos;
                        break;

                    case state::exponent_symbol:
                        current_state = state::exponent;
                        exponent_start = current_pos;
                        break;

                    case state::exponent_sign:
                        current_state = state::exponent;
                        // NOTE: exponent_start should be set when we encounter the sign
                        assert(exponent_start != npos);
                        break;

                    case state::init:
                    case state::integral_sign:
                    case state::integral:
                    case state::fractional:
                    case state::exponent:
                        break;

                    default:
                        return false;
                    }
                }
                else if (ch == '.')
                {
                    switch (current_state)
                    {
                    case state::integral_zero:
                    case state::integral:
                        current_state = state::fractional_symbol;
                        integral_end = current_pos;
                        break;

                    default:
                        return false;
                    }
                }
                else if ((ch == 'e') || (ch == 'E'))
                {
                    switch (current_state)
                    {
                    case state::integral_zero:
                    case state::integral:
                        current_state = state::exponent_symbol;
                        integral_end = current_pos;
                        break;

                    case state::fractional:
                        current_state = state::exponent_symbol;
                        fractional_end = current_pos;
                        break;

                    default:
                        return false;
                    }
                }
                else
                {
                    return false;
                }

                ++current_pos;
                return true;
            }

            constexpr bool valid_terminal_state() noexcept
            {
                switch (current_state)
                {
                case state::integral_zero:
                case state::integral:
                case state::fractional:
                case state::exponent:
                    return true;

                default:
                    return false;
                }
            }

            constexpr bool has_integral() noexcept
            {
                // Must at least have a leading '0'
                return true;
            }

            constexpr bool has_fractional() noexcept
            {
                return fractional_start != npos;
            }

            constexpr bool has_exponent() noexcept
            {
                return exponent_start != npos;
            }
        };



        /*
         * string_scan_state
         */
        struct string_scan_state
        {
            enum class state
            {
                normal,
                escaped,
                hex_0,
                hex_1,
                hex_2,
                hex_3,
            };

            state current_state = state::normal;
            char32_t hex_char;

            template <typename OutputItr>
            constexpr bool consume(char32_t ch, OutputItr itr)
            {
                switch (current_state)
                {
                case state::normal:
                    assert(ch != '"');
                    if (ch == '\\')
                    {
                        current_state = state::escaped;
                    }
                    else if (is_control(ch))
                    {
                        return false;
                    }
                    else
                    {
                        *itr++ = ch;
                    }
                    break;

                case state::escaped:
                    switch (ch)
                    {
                    case '"':
                    case '\\':
                    case '/':
                        *itr++ = ch;
                        break;

                    case 'b':
                        *itr++ = '\b';
                        break;

                    case 'f':
                        *itr++ = '\f';
                        break;

                    case 'n':
                        *itr++ = '\n';
                        break;

                    case 'r':
                        *itr++ = '\r';
                        break;

                    case 't':
                        *itr++ = '\t';
                        break;

                    case 'u':
                        current_state = state::hex_0;
                        break;

                    default:
                        return false;
                    }
                    break;

                case state::hex_0:
                    if (!details::is_hex_digit(ch))
                    {
                        return false;
                    }
                    hex_char = hex_to_digit(ch) << 24;
                    current_state = state::hex_1;
                    break;

                case state::hex_1:
                    if (!details::is_hex_digit(ch))
                    {
                        return false;
                    }
                    hex_char |= hex_to_digit(ch) << 16;
                    current_state = state::hex_2;
                    break;

                case state::hex_2:
                    if (!details::is_hex_digit(ch))
                    {
                        return false;
                    }
                    hex_char |= hex_to_digit(ch) << 8;
                    current_state = state::hex_3;
                    break;

                case state::hex_3:
                    if (!details::is_hex_digit(ch))
                    {
                        return false;
                    }
                    hex_char |= hex_to_digit(ch);
                    *itr++ = hex_char;
                    current_state = state::normal;
                    break;

                default:
                    assert(false);
                }

                return true;
            }

            constexpr bool valid_terminal_state() noexcept
            {
                return current_state == state::normal;
            }
        };



        /*
         * array_scan_state
         */
        struct array_scan_state
        {
            constexpr bool consume(char32_t ch) noexcept
            {
                ch;
                return false;
            }
        };



        /*
         * object_scan_state
         */
        struct object_scan_state
        {
            constexpr bool consume(char32_t ch) noexcept
            {
                ch;
                return false;
            }
        };



        /*
         * scan_state
         */
        struct scan_state
        {
            scan_state_tag active_value;
            union
            {
                global_scan_state global_state;
                null_scan_state null_state;
                true_scan_state true_state;
                false_scan_state false_state;
                number_scan_state number_state;
                string_scan_state string_state;
                array_scan_state array_state;
                object_scan_state object_state;
            };
        };



        /*
         * scan_token
         */
        enum class scan_token
        {
            none,
            null_begin,
            true_begin,
            false_begin,
            number_begin,
            string_begin,
            array_begin,
            array_end,
            object_begin,
            object_end,
        };
    }



    /*
     * basic_scanner
     */
    template <
        typename Handler,
        typename CharTy,
        typename CharTraits = std::char_traits<CharTy>,
        typename Alloc = std::allocator<CharTy>>
    class basic_scanner
    {
        template <typename Ty>
        using rebind_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Ty>;

    public:
        /*
         * Public Types
         */
        using string_type = std::basic_string<CharTy, CharTraits, Alloc>;



        /*
         * Constructor(s)/Destructor
         */
        basic_scanner(Handler& handler) :
            _handler(handler)
        {
        }

        explicit basic_scanner(Handler& handler, const Alloc& alloc) noexcept :
            _handler(handler),
            _stringBuffer(alloc),
            _stateStack(alloc)
        {
        }



        /*
         * 
         */
        template <typename Itr>
        void operator()(Itr front, Itr back)
        {
            continue_scan(front, back);
        }



    private:

        [[noreturn]]
        void invalid_character(char32_t ch)
        {
            char str[unicode::encoding_traits<unicode::encoding::utf_8>::max_code_point_size + 1];
            auto itr = unicode::make_output_iterator<unicode::encoding::utf_8>(str);
            *itr++ = ch;
            *itr++ = '\0';
            assert(itr.base() <= std::end(str));

            char buffer[26];
            std::snprintf(buffer, std::size(buffer), "invalid character '%s'", str);
            throw bad_json(this->_pos, buffer);
        }

        template <typename Itr>
        Itr advance(Itr& itr) noexcept
        {
            assert(*itr != '\n'); // Only when we're handling whitespace
            ++this->_pos.absolute;
            ++this->_pos.column;
            return ++itr;
        }

        template <typename Itr>
        void continue_scan(Itr front, Itr back)
        {
            auto begin = unicode::iterator(front);
            auto end = unicode::iterator(back);
            while (begin != end)
            {
                assert(!this->_stateStack.empty());
                switch (this->_stateStack.back().active_value)
                {
                case details::scan_state_tag::global:
                    begin = continue_global_scan(begin, end); // TODO: generalize and inline here?
                    break;

                case details::scan_state_tag::null:
                    begin = continue_null_scan(begin, end);
                    break;

                case details::scan_state_tag::true_boolean:
                    begin = continue_true_scan(begin, end);
                    break;

                case details::scan_state_tag::false_boolean:
                    begin = continue_false_scan(begin, end);
                    break;

                case details::scan_state_tag::number:
                    begin = continue_number_scan(begin, end);
                    break;

                case details::scan_state_tag::string:
                    begin = continue_string_scan(begin, end);
                    break;

                case details::scan_state_tag::object:
                    // TODO
                    break;

                case details::scan_state_tag::array:
                    // TODO
                    break;

                default:
                    assert(false); // Cannot happen
                }
            }
        }

        template <typename Itr, typename State>
        bool continue_token_scan(Itr& begin, Itr end, State& state)
        {
            assert(begin != end);
            assert(!state.valid_terminal_state());

            // NOTE: Itr is already a UTF-aware iterator type
            do
            {
                const char32_t ch = *begin;
                if (!state.consume(ch))
                {
                    invalid_character(ch);
                }
                begin = advance(begin);

                if (state.valid_terminal_state())
                {
                    // TODO: Need to handle the case where the next character is not valid!
                    this->_stateStack.pop_back();
                    return true;
                }
            } while (begin != end);

            return false;
        }

        template <typename Itr>
        Itr continue_null_scan(Itr begin, Itr end)
        {
            auto& state = this->_stateStack.back().null_state;
            if (continue_token_scan(begin, end, state) && !this->_handler.on_null())
            {
                throw bad_json(this->_pos, "on_null returned false");
            }

            return begin;
        }

        template <typename Itr>
        Itr continue_true_scan(Itr begin, Itr end)
        {
            auto& state = this->_stateStack.back().true_state;
            if (continue_token_scan(begin, end, state) && !this->_handler.on_boolean(true))
            {
                throw bad_json(this->_pos, "on_boolean returned false");
            }

            return begin;
        }

        template <typename Itr>
        Itr continue_false_scan(Itr begin, Itr end)
        {
            auto& state = this->_stateStack.back().false_state;
            if (continue_token_scan(begin, end, state) && !this->_handler.on_boolean(false))
            {
                throw bad_json(this->_pos, "on_boolean returned false");
            }

            return begin;
        }

        template <typename Itr>
        Itr continue_number_scan(Itr begin, Itr end)
        {
            // NOTE: Itr is already a UTF-aware iterator type
            end;// TODO
            return begin;
        }

        template <typename Itr>
        Itr continue_string_scan(Itr begin, Itr end)
        {
            assert(begin != end);

            auto& state = this->_stateStack.back().string_state;

            // NOTE: Itr is already a UTF-aware iterator type
            do
            {
                const char32_t ch = *begin;

                // NOTE: We expect the first '"' to be skipped so that any subsequent '"' (that's not escaped) will
                // indicate the end of the string
                if (ch == U'"')
                {
                    if (!state.valid_terminal_state())
                    {
                        invalid_character(ch);
                    }
                    else if (!this->_handler.on_string(this->_stringBuffer))
                    {
                        throw bad_json(this->_pos, "on_string returned false");
                    }
                    else
                    {
                        begin = advance(begin); // "consume" the final quotation mark
                        this->_stateStack.pop_back();
                        break;
                    }
                }

                // TODO: Is construction of the output iterator inefficient, or is it a no-op? Should we cache it?
                if (!state.consume(
                    ch,
                    make_output_iterator<unicode::character_encoding_v<CharTy>>(std::back_inserter(this->_stringBuffer))))
                {
                    invalid_character(ch);
                }

                begin = advance(begin);
            } while (begin != end);

            return begin;
        }

        template <typename Itr>
        details::scan_token next_token(Itr& begin, Itr end)
        {
            assert(begin != end);

            char32_t ch = *begin;
            do
            {
                if (ch == '{')
                {
                    return details::scan_token::object_begin;
                }
                else if (ch == '}')
                {
                    return details::scan_token::object_end;
                }
                else if (ch == '[')
                {
                    return details::scan_token::array_begin;
                }
                else if (ch == ']')
                {
                    return details::scan_token::array_end;
                }
                else if (ch == '"')
                {
                    return details::scan_token::string_begin;
                }
                else if ((ch == '-') || details::is_digit(ch))
                {
                    return details::scan_token::number_begin;
                }
                else if (ch == 't')
                {
                    return details::scan_token::true_begin;
                }
                else if (ch == 'f')
                {
                    return details::scan_token::false_begin;
                }
                else if (ch == 'n')
                {
                    return details::scan_token::null_begin;
                }
                else if (details::is_whitespace(ch))
                {
                    // NOTE: We will likely encounter two forms of input: "minified" JSON and "beautified" JSON. With
                    // "minified" JSON, we can expect minimal whitespace, if any. With "beautified" JSON, we can expect
                    // a lot more whitespace, and most whitespace will occur in "chunks" (multiple tabs, spaces, etc.).
                    // Since the purpose of "minified" JSON is speed, we try and optimize for speed with such input,
                    // hence why this check is last. However, if we ever do encounter whitespace, we want to make
                    // skipping over it relatively fast, hence the loop here (as opposed to just letting the loop
                    // continue executing, entering this branch for each subsequent whitespace character).
                    do
                    {
                        // NOTE: Explicitly _not_ using advance here since we may need to advance the line
                        ++this->_pos.absolute;
                        if (ch == '\n')
                        {
                            this->_pos.column = 1;
                            ++this->_pos.line;
                        }
                        else
                        {
                            ++this->_pos.column;
                        }

                        if (++begin == end)
                        {
                            break;
                        }
                        else
                        {
                            ch = *begin;
                        }
                    } while (details::is_whitespace(ch));
                }
                else
                {
                    invalid_character(ch);
                }
            } while (begin != end);

            assert(begin == end);
            return details::scan_token::none;
        }

        template <typename Itr>
        Itr continue_global_scan(Itr begin, Itr end)
        {
            auto& state = this->_stateStack.back().global_state;

            if (auto token = next_token(begin, end); token != details::scan_token::none)
            {
                if (!state.on_valid_token())
                {
                    // Only one value allowed in global scope
                    invalid_character(*begin);
                }

                details::scan_state nextState{};

                // TODO: single root!
                switch (token)
                {
                case details::scan_token::null_begin:
                    nextState.active_value = details::scan_state_tag::null;
                    nextState.null_state = {};
                    break;

                case details::scan_token::true_begin:
                    nextState.active_value = details::scan_state_tag::true_boolean;
                    nextState.true_state = {};
                    break;

                case details::scan_token::false_begin:
                    nextState.active_value = details::scan_state_tag::false_boolean;
                    nextState.false_state = {};
                    break;

                case details::scan_token::number_begin:
                    nextState.active_value = details::scan_state_tag::number;
                    nextState.number_state = {};
                    break;

                case details::scan_token::string_begin:
                    nextState.active_value = details::scan_state_tag::string;
                    nextState.string_state = {};
                    break;

                case details::scan_token::array_begin:
                    nextState.active_value = details::scan_state_tag::array;
                    nextState.array_state = {};
                    break;

                case details::scan_token::object_begin:
                    nextState.active_value = details::scan_state_tag::object;
                    nextState.object_state = {};
                    break;

                default:
                    // Object/array end are not valid here!
                    invalid_character(*begin);
                }

                this->_stateStack.push_back(nextState);
            }
            else
            {
                assert(begin == end);
            }

            return begin;
        }



        scan_position _pos;
        string_type _stringBuffer;
        Handler& _handler; // TODO: compressed_pair

        std::vector<details::scan_state, rebind_allocator<details::scan_state>> _stateStack =
        {
            { details::scan_state_tag::global, details::global_scan_state{} }
        };

















#if 0
        template <typename Itr>
        Itr scan_value(Itr begin, Itr end)
        {
            auto advance = [&](char32_t ch) constexpr
            {
                ++this->_pos.absolute;
                if (ch == '\n')
                {
                    this->_pos.column = 1;
                    ++this->_pos.line;
                }
                else
                {
                    ++this->_pos.column;
                }
            };

            for (; begin != end; )
            {
                const char32_t ch = *begin;

                if (ch == '{')
                {
                    if (!this->_handler.on_object_begin())
                    {
                        throw bad_json(this->_pos, "on_object_begin returned false");
                    }

                    advance(ch);
                }
                else if (ch == '}')
                {
                    if (!this->_handler.on_object_end())
                    {
                        throw bad_json(this->_pos, "on_object_end returned false");
                    }

                    advance(ch);
                }
                else if (ch == '[')
                {
                    if (!this->_handler.on_array_begin())
                    {
                        throw bad_json(this->_pos, "on_array_begin returned false");
                    }

                    advance(ch);
                }
                else if (ch == ']')
                {
                    if (!this->_handler.on_array_end())
                    {
                        throw bad_json(this->_pos, "on_array_end returned false");
                    }

                    advance(ch);
                }
                else if (ch == '"')
                {
                    // TODO: string
                }
                else if ((ch == '-') || details::is_digit(ch))
                {
                    // TODO: number
                }
                else if (ch == 't')
                {
                    // TODO: true
                }
                else if (ch == 'f')
                {
                    // TODO: false
                }
                else if (ch == 'n')
                {
                    // TODO: null
                }
                else if (details::is_whitespace(ch))
                {
                    // NOTE: We will likely encounter two forms of input: "minified" JSON and "beautified" JSON. With
                    // "minified" JSON, we can expect minimal whitespace, if any. With "beautified" JSON, we can expect
                    // a lot more whitespace, and most whitespace will occur in "chunks" (multiple tabs, spaces, etc.).
                    // Since the purpose of "minified" JSON is speed, we try and optimize for speed with such input,
                    // hence why this check is last. However, if we ever do encounter whitespace, we want to make
                    // skipping over it relatively fast, hence the loop inside skip_whitespace (as opposed to just
                    // letting the loop continue executing, entering this branch for each subsequent whitespace
                    // character).
                    begin = skip_whitespace(begin, end);
                }
                else
                {
                    char chStr[utf_traits<char>::max_code_point_size + 1];
                    auto itr = utf8_output_iterator(chStr);
                    *itr++ = ch;
                    *itr++ = '\0';
                    assert(itr.base() <= std::end(chStr));

                    char buffer[512];
                    std::snprintf(buffer, std::size(buffer), "invalid character '%s'", chStr);
                    throw bad_json(this->_pos, buffer);
                }
            }

            return begin;
        }

        template <typename Itr>
        constexpr Itr skip_whitespace(Itr begin, Itr end) noexcept
        {
            // NOTE: This function is optimized to assume that the first character has already been validated to be
            // whitespace.
            assert((begin != end) && details::is_whitespace(*begin));

            char32_t ch = *begin;
            do
            {
                ++this->_pos.absolute;
                if (ch == '\n')
                {
                    this->_pos.column = 1;
                    ++this->_pos.line;
                }
                else
                {
                    ++this->_pos.column;
                }

                if (++begin == end)
                {
                    break;
                }
                else
                {
                    ch = *begin;
                }
            } while (details::is_whitespace(ch));

            return begin;
        }
#endif
    };

    template <typename Handler> using scanner = basic_scanner<Handler, char>;
    template <typename Handler> using wscanner = basic_scanner<Handler, wchar_t>;
    template <typename Handler> using u16scanner = basic_scanner<Handler, char16_t>;
    template <typename Handler> using u32scanner = basic_scanner<Handler, char32_t>;
}
