/*
 * windows.h
 *
 * Duncan Horn
 *
 * Mostly just wraps Win32 functions and "C++-ifys" them, throwing exceptions on failure
 */
#pragma once

#ifndef WIN32
static_assert(false, "Cannot include dhorn/windows/windows.h on a non-Windows machine")
#endif  /* WIN32 */

#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>

#if !(defined WIN32_LEAN_AND_MEAN) && !(defined _DHORN_NO_WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif

#if !(defined NOMINMAX) && !(defined _DHORN_NO_NOMINMAX)
#define NOMINMAX 1
#endif

#include <Windows.h>

namespace dhorn
{
    namespace win32
    {
        /*
         * Type declarations
         */
#pragma region Type Declarations

        using tstring = std::basic_string<TCHAR>;

        using pid_t = uint32_t;
        using tid_t = uint32_t;

        using brush_handle = HBRUSH;
        using cursor_handle = HCURSOR;
        using icon_handle = HICON;
        using instance_handle = HINSTANCE;
        using menu_handle = HMENU;
        using window_handle = HWND;

#pragma endregion



        /*
         * Exception Types
         */
#pragma region Exceptions

        class windows_exception :
            public std::exception
        {
        public:
            virtual HRESULT get_hresult(void) const = 0;
        };

        class hresult_exception final :
            public windows_exception
        {
        public:
            hresult_exception(_In_ HRESULT hr) :
                _hr(hr)
            {
            }

            virtual const char *what(void)
            {
                return "hresult_exception";
            }

            virtual HRESULT get_hresult(void) const
            {
                return this->_hr;
            }

        private:

            HRESULT _hr;
        };

        class win32_exception final :
            public windows_exception
        {
        public:
            win32_exception(_In_ uint32_t status) :
                _status(status)
            {
            }

            virtual const char *what(void)
            {
                return "win32_exception";
            }

            uint32_t get_status(void) const
            {
                return this->_status;
            }

            virtual HRESULT get_hresult(void) const
            {
                return HRESULT_FROM_WIN32(this->_status);
            }

        private:
            uint32_t _status;
        };

#pragma endregion



        /*
         * Helpers
         */
        template <typename HandleType, typename HandleTraits>
        class basic_handle;

        namespace garbage
        {
            inline void throw_last_error(void)
            {
                auto error = GetLastError();
                if (error)
                {
                    // Many functions expect us to check GetLastError to distinguish between errors
                    // and other general cases. Just go ahead and always check if we suspect error.
                    throw win32_exception(error);
                }
            }

            template <typename Func>
            inline void make_boolean_call(Func &func)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw_last_error();
                }
            }

            template <typename Func, typename... Args>
            inline void make_boolean_call(Func &func, Args&&... args)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw_last_error();
                }
            }

            template <typename HandleTraits, typename Func>
            inline basic_handle<HANDLE, HandleTraits> make_handle_call(Func &func)
            {
                HANDLE result = func();
                if ((result == INVALID_HANDLE_VALUE) || !result)
                {
                    throw_last_error();
                }

                return result;
            }

            template <typename HandleTraits, typename Func, typename... Args>
            inline basic_handle<HANDLE, HandleTraits> make_handle_call(Func &func, Args&&... args)
            {
                HANDLE result = func(std::forward<Args>(args)...);
                if ((result == INVALID_HANDLE_VALUE) || !result)
                {
                    throw_last_error();
                }

                return result;
            }

            template <typename Func>
            inline void make_hresult_call(Func &func)
            {
                HRESULT hr = func();

                if (FAILED(hr))
                {
                    throw_last_error();
                }
            }

            template <typename Func, typename... Args>
            inline void make_hresult_call(Func &func, Args&&... args)
            {
                HRESULT hr = func(std::forward<Args>(args)...);

                if (FAILED(hr))
                {
                    throw_last_error();
                }
            }

            template <typename ResultType, ResultType Failure = ResultType{}, typename Func>
            inline ResultType make_call_fail_on_value(Func &func)
            {
                auto result = func();
                if (result == Failure)
                {
                    throw_last_error();
                }

                return result;
            }

            template <
                typename ResultType,
                ResultType Failure = ResultType{},
                typename Func,
                typename... Args>
            inline ResultType make_call_fail_on_value(Func &func, Args&&... args)
            {
                auto result = func(std::forward<Args>(args)...);
                if ((result == Failure) && (GetLastError() != 0))
                {
                    throw_last_error();
                }

                return result;
            }



            const TCHAR *null_if_empty(_In_ const tstring &str)
            {
                return ((&str == nullptr) || str.empty()) ? nullptr : str.c_str();
            }
        }



        /*
         * HANDLE Wrapper
         */
#pragma region handle

        struct unique_handle_traits
        {
            static void close(_In_ HANDLE handle)
            {
                // NOTE: It is *okay* to call with INVALID_HANDLE_VALUE
                if (handle != INVALID_HANDLE_VALUE)
                {
                    garbage::make_boolean_call(CloseHandle, handle);
                }
            }
        };

        struct shared_handle_traits :
            public unique_handle_traits
        {
            static HANDLE copy(_In_ HANDLE handle)
            {
                // NOTE: It is *okay* to call with INVALID_HANDLE_VALUE
                if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
                {
                    // We only duplicate for current process with same access rights
                    HANDLE result;
                    HANDLE processHandle = GetCurrentProcess();

                    garbage::make_boolean_call(
                        DuplicateHandle,
                        processHandle,              // Source process
                        handle,                     // Handle to duplicate
                        processHandle,              // Destination process
                        &result,                    // Output handle
                        0,                          // Ingored (same access rights)
                        false,                      // Not inheritable
                        DUPLICATE_SAME_ACCESS);     // Same access rights as source handle

                    return result;
                }

                return handle;
            }
        };

        template <typename HandleType, typename Traits>
        class basic_handle
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            basic_handle(void) :
                _handle(INVALID_HANDLE_VALUE)
            {
            }

            basic_handle(_In_opt_ HandleType handle) :
                _handle(handle)
            {
            }

            basic_handle(_In_ const basic_handle &other)
            {
                this->_handle = Traits::copy(other._handle);
            }

            basic_handle(_Inout_ basic_handle &&other) :
                _handle(other._handle)
            {
                other._handle = INVALID_HANDLE_VALUE;
            }

            ~basic_handle(void)
            {
                this->Release();
            }



            /*
             * Operators
             */
            basic_handle &operator=(_In_ const basic_handle &other)
            {
                if (this != &other)
                {
                    this->Release();
                    this->_handle = Traits::copy(other._handle);
                }

                return *this;
            }

            basic_handle &operator=(_Inout_ basic_handle &&other)
            {
                this->Release();
                this->_handle = other._handle;
                other._handle = INVALID_HANDLE_VALUE;

                return *this;
            }

            basic_handle &operator=(_In_ HandleType basic_handle)
            {
                this->Release();
                this->_handle = basic_handle;

                return *this;
            }

            operator HandleType(void)
            {
                return this->_handle;
            }



            /*
             * Public functions
             */
            bool invalid(void) const
            {
                // Null handles have special meaning, but they are assumed to be invalid
                return this->_handle == INVALID_HANDLE_VALUE || this->_handle == nullptr;
            }

            void release(void)
            {
                this->Release();
            }

            void swap(_Inout_ basic_handle &other)
            {
                auto temp = other._handle;
                other._handle = this->_handle;
                this->_handle = temp;
            }



        private:

            void Release(void)
            {
                auto temp = this->_handle;

                if (!this->invalid())
                {
                    this->_handle = INVALID_HANDLE_VALUE;
                    garbage::make_boolean_call(CloseHandle, temp);
                }
            }



            HandleType _handle;
        };

        using unique_handle = basic_handle<HANDLE, unique_handle_traits>;
        using shared_handle = basic_handle<HANDLE, shared_handle_traits>;

#pragma endregion



        /*
         * Cursor Functions
         */
#pragma region Cursor Functions

        cursor_handle load_cursor(_In_opt_ instance_handle instance, _In_ LPCTSTR name)
        {
            return garbage::make_call_fail_on_value<cursor_handle>(LoadCursor, instance, name);
        }

        cursor_handle load_cursor(_In_opt_ instance_handle instance, _In_ const tstring &name)
        {
            return load_cursor(instance, garbage::null_if_empty(name));
        }

#pragma endregion



        /*
         * File Management Functions
         */
#pragma region File Management

        template <
            typename ResultHandleTraits = unique_handle_traits,
            typename SourceHandleTraits = unique_handle_traits>
        inline basic_handle<HANDLE, ResultHandleTraits> create_file(
            _In_ const tstring &fileName,
            _In_ uint32_t desiredAccess,
            _In_ uint32_t shareMode,
            _In_opt_ LPSECURITY_ATTRIBUTES securityAttributes,
            _In_ uint32_t creationDisposition,
            _In_ uint32_t flagsAndAttributes,
            _In_opt_ basic_handle<HANDLE, SourceHandleTraits> templateFile = unique_handle(nullptr))
        {
            return garbage::make_handle_call<ResultHandleTraits>(CreateFile, fileName.c_str(),
                desiredAccess, shareMode, securityAttributes, creationDisposition,
                flagsAndAttributes, templateFile);
        }

#pragma endregion



        /*
         * Message Functions
         */
#pragma region Message Functions

        inline intptr_t dispatch_message(_In_ const MSG &msg)
        {
            return DispatchMessage(&msg);
        }

        inline bool get_message(
            _Out_ MSG &msg,
            _In_opt_ window_handle window = 0,
            _In_opt_ unsigned filterMin = 0,
            _In_opt_ unsigned filterMax = 0)
        {
            return !!garbage::make_call_fail_on_value<int, -1>(
                GetMessage, &msg, window, filterMin, filterMax);
        }

        inline bool peek_message(
            _In_opt_ window_handle window,
            _In_ unsigned filterMin,
            _In_ unsigned filterMax,
            _In_ unsigned remove,
            _Out_ MSG *result)
        {
            return !!PeekMessage(result, window, filterMin, filterMax, remove);
        }

        inline void post_message(
            _In_opt_ window_handle window,
            _In_ unsigned msg,
            _In_ uintptr_t wparam,
            _In_ intptr_t lparam)
        {
            garbage::make_boolean_call(PostMessage, window, msg, wparam, lparam);
        }

        inline void post_quit_message(_In_ int exitCode)
        {
            PostQuitMessage(exitCode);
        }

        inline void translate_message(_In_ const MSG &msg)
        {
            garbage::make_boolean_call(TranslateMessage, &msg);
        }

#pragma endregion



        /*
         * Painting and Drawing Functions
         */
#pragma region Painting and Drawing Functions

        inline void update_window(_In_ window_handle window)
        {
            garbage::make_boolean_call(UpdateWindow, window);
        }

#pragma endregion



        /*
         * Process and Thread Functions
         */
#pragma region Process and Thread

        inline tid_t get_current_thread_id(void)
        {
            return GetCurrentThreadId();
        }

#pragma endregion



        /*
         * Window Class Functions
         */
#pragma region Window Class Functions

        inline WNDCLASS get_class_info(
            _In_opt_ instance_handle instance,
            _In_ const tstring &className)
        {
            WNDCLASS result;
            garbage::make_boolean_call(GetClassInfo, instance, className.c_str(), &result);

            return result;
        }

        inline WNDCLASSEX get_class_info_ex(
            _In_opt_ instance_handle instance,
            _In_ const tstring &className)
        {
            WNDCLASSEX result;
            garbage::make_boolean_call(GetClassInfoEx, instance, className.c_str(), &result);

            return result;
        }

        inline uint32_t get_class_long(_In_ window_handle window, _In_ int index)
        {
            return garbage::make_call_fail_on_value<uint32_t>(GetClassLong, window, index);
        }

        inline uintptr_t get_class_long_ptr(_In_ window_handle window, _In_ int index)
        {
            return garbage::make_call_fail_on_value<uintptr_t>(GetClassLongPtr, window, index);
        }

        inline tstring get_class_name(_In_ window_handle window)
        {
            // maximum class name is 256 characters
            TCHAR buffer[257];
            garbage::make_call_fail_on_value<int>(GetClassName, window, buffer, 257);

            return buffer;
        }

        inline uint16_t get_class_word(_In_ window_handle window, _In_ int index)
        {
            return garbage::make_call_fail_on_value<uint16_t>(GetClassWord, window, index);
        }

        inline long get_window_long(_In_ window_handle window, _In_ int index)
        {
            return garbage::make_call_fail_on_value<long>(GetWindowLong, window, index);
        }

        inline uintptr_t get_window_long_ptr(_In_ window_handle window, _In_ int index)
        {
            return garbage::make_call_fail_on_value<uintptr_t>(GetWindowLongPtr, window, index);
        }

        inline ATOM register_class(_In_ const WNDCLASS &wndClass)
        {
            return garbage::make_call_fail_on_value<ATOM>(RegisterClass, &wndClass);
        }

        inline ATOM register_class(_In_ const WNDCLASSEX &wndClass)
        {
            return garbage::make_call_fail_on_value<ATOM>(RegisterClassEx, &wndClass);
        }

        inline uint32_t set_class_long(_In_ window_handle window, _In_ int index, _In_ long value)
        {
            return garbage::make_call_fail_on_value<uint32_t>(SetClassLong, window, index, value);
        }

        inline uintptr_t set_class_long_ptr(
            _In_ window_handle window,
            _In_ int index,
            _In_ uintptr_t value)
        {
            return garbage::make_call_fail_on_value<uintptr_t>(
                SetClassLongPtr, window, index, value);
        }

        inline uint16_t set_class_word(
            _In_ window_handle window,
            _In_ int index,
            _In_ uint16_t value)
        {
            return garbage::make_call_fail_on_value<uint16_t>(SetClassWord, window, index, value);
        }

        inline long set_window_long(_In_ window_handle window, _In_ int index, _In_ long value)
        {
            return garbage::make_call_fail_on_value<long>(SetWindowLong, window, index, value);
        }

        inline uintptr_t set_window_long_ptr(
            _In_ window_handle window,
            _In_ int index,
            _In_ uintptr_t value)
        {
            return garbage::make_call_fail_on_value<uintptr_t>(
                SetWindowLongPtr, window, index, value);
        }

        inline void unregister_class(
            _In_ const tstring &className,
            _In_opt_ instance_handle instance = nullptr)
        {
            garbage::make_boolean_call(UnregisterClass, className.c_str(), instance);
        }

#pragma endregion



        /*
         * Window Functions
         */
#pragma region Window Functions

        inline RECT adjust_window_rect(
            _In_ const RECT &inputRect,
            _In_ uint32_t style,
            _In_ bool hasMenu)
        {
            RECT result = inputRect;
            garbage::make_boolean_call(AdjustWindowRect, &result, style, hasMenu);

            return result;
        }

        inline RECT adjust_window_rect(
            _In_ const RECT &inputRect,
            _In_ uint32_t style,
            _In_ bool hasMenu,
            _In_ uint32_t extendedStyle)
        {
            RECT result = inputRect;
            garbage::make_boolean_call(AdjustWindowRectEx, &result, style, hasMenu, extendedStyle);

            return result;
        }

        inline void allow_set_foreground_window(_In_ pid_t processId)
        {
            garbage::make_boolean_call(AllowSetForegroundWindow, processId);
        }

        inline void animate_window(
            _In_ window_handle window,
            _In_ uint32_t duration,
            _In_ uint32_t flags)
        {
            garbage::make_boolean_call(AnimateWindow, window, duration, flags);
        }

        inline bool any_popup(void)
        {
            return !!AnyPopup();
        }

        inline void bring_window_to_top(_In_ window_handle window)
        {
            garbage::make_boolean_call(BringWindowToTop, window);
        }

        inline RECT calculate_popup_window_position(
            _In_ const POINT &anchorPoint,
            _In_ const SIZE &windowSize,
            _In_ unsigned int flags)
        {
            RECT result;
            garbage::make_boolean_call(CalculatePopupWindowPosition, &anchorPoint, &windowSize,
                flags, nullptr, &result);

            return result;
        }

        inline RECT calculate_popup_window_position(
            _In_ const POINT &anchorPoint,
            _In_ const SIZE &windowSize,
            _In_ unsigned int flags,
            _In_ const RECT &exclude)
        {
            RECT result;

            // The exclude rectangle is marked as _In_, but is non-const. Thus, the const_cast
            garbage::make_boolean_call(CalculatePopupWindowPosition, &anchorPoint, &windowSize,
                flags, const_cast<RECT *>(&exclude), &result);

            return result;
        }

        inline void close_window(_In_ window_handle window)
        {
            garbage::make_boolean_call(CloseWindow, window);
        }

        inline window_handle create_window(
            _In_opt_ const tstring &className,
            _In_opt_ const tstring &windowName,
            _In_ uint32_t style,
            _In_ int x, _In_ int y, _In_ int width, _In_ int height,
            _In_opt_ window_handle parent = nullptr,
            _In_opt_ menu_handle menu = nullptr,
            _In_opt_ instance_handle instance = nullptr,
            _In_opt_ void *param = nullptr)
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(CreateWindowEx, 0L, clsName, wndName,
                style, x, y, width, height, parent, menu, instance, param);
        }

        inline window_handle create_window(
            _In_ uint32_t extendedStyle,
            _In_opt_ const tstring &className,
            _In_opt_ const tstring &windowName,
            _In_ uint32_t style,
            _In_ int x, _In_ int y, _In_ int width, _In_ int height,
            _In_opt_ window_handle parent = nullptr,
            _In_opt_ menu_handle menu = nullptr,
            _In_opt_ instance_handle instance = nullptr,
            _In_opt_ void *param = nullptr)
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(CreateWindowEx, extendedStyle, clsName, wndName, style,
                x, y, width, height, parent, menu, instance, param);
        }

        inline void destroy_window(_In_ window_handle window)
        {
            garbage::make_boolean_call(DestroyWindow, window);
        }

        inline void enum_child_windows(
            _In_ window_handle parentWindow,
            _In_ WNDENUMPROC func,
            _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumChildWindows, parentWindow, func, param);
        }

        inline void enum_thread_windows(
            _In_ tid_t threadId,
            _In_ WNDENUMPROC func,
            _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumThreadWindows, threadId, func, param);
        }

        inline void enum_windows(_In_ WNDENUMPROC func, _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumWindows, func, param);
        }

        namespace garbage
        {
            BOOL CALLBACK enum_windows_proc(_In_ window_handle childWindow, _In_ LPARAM param)
            {
                auto func = reinterpret_cast<std::function<bool(window_handle)> *>(param);
                return (*func)(childWindow);
            }
        }

        inline void enum_cild_windows(
            _In_ window_handle parentWindow,
            _In_ std::function<bool(window_handle)> func)
        {
            enum_child_windows(parentWindow, garbage::enum_windows_proc,
                reinterpret_cast<LPARAM>(&func));
        }

        inline void enum_thread_windows(
            _In_ tid_t threadId,
            _In_ std::function<bool(window_handle)> func)
        {
            enum_thread_windows(threadId, garbage::enum_windows_proc,
                reinterpret_cast<LPARAM>(&func));
        }

        inline void enum_windows(_In_ std::function<bool(window_handle)> func)
        {
            enum_windows(garbage::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
        }

        inline window_handle find_window(
            _In_opt_ const tstring &className = TEXT(""),
            _In_opt_ const tstring &windowName = TEXT(""))
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(FindWindow, clsName, wndName);
        }

        inline window_handle find_window(
            _In_ window_handle parentWindow,
            _In_ window_handle childWindowAfter,
            _In_opt_ const tstring &className = TEXT(""),
            _In_opt_ const tstring &windowName = TEXT(""))
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(FindWindowEx, parentWindow, childWindowAfter,
                clsName, wndName);
        }

        inline window_handle get_ancestor(_In_ window_handle window, _In_ unsigned int flags)
        {
            return garbage::make_call_fail_on_value<HWND>(GetAncestor, window, flags);
        }

        inline RECT get_client_rect(_In_ window_handle window)
        {
            RECT result;
            garbage::make_boolean_call(GetClientRect, window, &result);

            return result;
        }

        inline window_handle get_desktop_window(void)
        {
            return garbage::make_call_fail_on_value<HWND>(GetDesktopWindow);
        }

        inline window_handle get_foreground_window(void)
        {
            return garbage::make_call_fail_on_value<HWND>(GetForegroundWindow);
        }

        inline window_handle get_last_active_popup(_In_ window_handle owner)
        {
            return garbage::make_call_fail_on_value<HWND>(GetLastActivePopup, owner);
        }

        inline window_handle get_next_window(_In_ window_handle window, _In_ unsigned int dir)
        {
            // GetNextWindow is defined to GetWindow (and apparently GetNextWindow isn't available...)
            return garbage::make_call_fail_on_value<HWND>(GetWindow, window, dir);
        }

        inline window_handle get_parent(_In_ window_handle child)
        {
            return garbage::make_call_fail_on_value<HWND>(GetParent, child);
        }

        inline uint32_t get_process_default_layout(void)
        {
            DWORD result;
            garbage::make_boolean_call(GetProcessDefaultLayout, &result);

            return result;
        }

        inline window_handle get_shell_window(void)
        {
            return garbage::make_call_fail_on_value<HWND>(GetShellWindow);
        }

        inline uint32_t get_sys_color(_In_ int index)
        {
            return GetSysColor(index);
        }

        inline window_handle get_top_window(_In_opt_ window_handle parentWindow = nullptr)
        {
            return garbage::make_call_fail_on_value<HWND>(GetTopWindow, parentWindow);
        }

        inline window_handle get_window(_In_ window_handle window, _In_ unsigned int dir)
        {
            return garbage::make_call_fail_on_value<HWND>(GetWindow, window, dir);
        }

        inline RECT get_window_rect(_In_ window_handle window)
        {
            RECT result;
            garbage::make_boolean_call(GetWindowRect, window, &result);

            return result;
        }

        inline void get_window_text(_In_ window_handle window, _In_ TCHAR *text, _In_ int len)
        {
            GetWindowText(window, text, len);
        }

        template <size_t size>
        inline void get_window_text(_In_ window_handle window, _In_ TCHAR text[size])
        {
            GetWindowText(window, text, size);
        }

        template <int max_length = UINT_MAX>
        inline tstring get_window_text(_In_ window_handle window)
        {
            // Get the estimated size
            size_t size = std::min(GetWindowTextLength(window), max_length) + 1;
            std::unique_ptr<TCHAR> str(new TCHAR[size]);
            GetWindowText(window, str.get(), size);

            tstring result = str.get();
            return result;
        }

        inline uint32_t get_window_thread_process_id(
            _In_ window_handle window,
            _In_opt_ uint32_t *processId = nullptr)
        {
            // uint32_t* is not implicitly castable to DWORD* even though they will be the same
            // size, so we must reinterpret_cast here
            return garbage::make_call_fail_on_value<uint32_t, 0>(GetWindowThreadProcessId, window,
                reinterpret_cast<DWORD *>(processId));
        }

        inline std::pair<tid_t, pid_t> get_window_thread_process_id(_In_ window_handle window)
        {
            DWORD processId;
            DWORD threadId = GetWindowThreadProcessId(window, &processId);

            return std::make_pair(threadId, processId);
        }

        inline void show_window(_In_ window_handle window, _In_ int cmdShow)
        {
            garbage::make_boolean_call(ShowWindow, window, cmdShow);
        }

#pragma endregion
    }
}
