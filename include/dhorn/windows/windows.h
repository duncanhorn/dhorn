/*
 * windows.h
 *
 * Duncan Horn
 *
 * Mostly just wraps Win32 functions and "C++-ifys" them, throwing exceptions on failure
 */

#ifndef WIN32
static_assert(false, "Cannot include dhorn\windows\windows.h on a non-Windows machine")
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
            template <typename Func>
            inline void make_boolean_call(Func &func)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw win32_exception(GetLastError());
                }
            }

            template <typename Func, typename... Args>
            inline void make_boolean_call(Func &func, Args&&... args)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw win32_exception(GetLastError());
                }
            }

            template <typename HandleTraits, typename Func>
            inline basic_handle<HANDLE, HandleTraits> make_handle_call(Func &func)
            {
                HANDLE result = func();
                if ((result == INVALID_HANDLE_VALUE) || !result)
                {
                    throw win32_exception(GetLastError());
                }

                return result;
            }

            template <typename HandleTraits, typename Func, typename... Args>
            inline basic_handle<HANDLE, HandleTraits> make_handle_call(Func &func, Args&&... args)
            {
                HANDLE result = func(std::forward<Args>(args)...);
                if ((result == INVALID_HANDLE_VALUE) || !result)
                {
                    throw win32_exception(GetLastError());
                }

                return result;
            }

            template <typename Func>
            inline void make_hresult_call(Func &func)
            {
                HRESULT hr = func();

                if (FAILED(hr))
                {
                    throw hresult_exception(hr);
                }
            }

            template <typename Func, typename... Args>
            inline void make_hresult_call(Func &func, Args&&... args)
            {
                HRESULT hr = func(std::forward<Args>(args)...);

                if (FAILED(hr))
                {
                    throw hresult_exception(hr);
                }
            }

            template <typename ResultType, typename Func>
            inline ResultType make_call_fail_on_null(Func &func)
            {
                auto result = func();
                if (!result)
                {
                    throw win32_exception(GetLastError());
                }

                return result;
            }

            template <typename ResultType, typename Func, typename... Args>
            inline ResultType make_call_fail_on_null(Func &func, Args&&... args)
            {
                auto result = func(std::forward<Args>(args)...);
                if (!result)
                {
                    throw win32_exception(GetLastError());
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
         * Process and Thread Functions
         */
#pragma region Process and Thread

        // Add as needed

#pragma endregion



        /*
         * File Management Functions
         */
#pragma region File Management

        template <
            typename ResultHandleTraits = unique_handle_traits,
            typename SourceHandleTraits = unique_handle_traits>
        basic_handle<HANDLE, ResultHandleTraits> create_file(
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

            return garbage::make_call_fail_on_null<HWND>(CreateWindowEx, 0L, clsName, wndName, style, x, y,
                width, height, parent, menu, instance, param);
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

            return garbage::make_call_fail_on_null<HWND>(CreateWindowEx, extendedStyle, clsName, wndName, style,
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

            return garbage::make_call_fail_on_null<HWND>(FindWindow, clsName, wndName);
        }

        inline window_handle find_window(
            _In_ window_handle parentWindow,
            _In_ window_handle childWindowAfter,
            _In_opt_ const tstring &className = TEXT(""),
            _In_opt_ const tstring &windowName = TEXT(""))
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_null<HWND>(FindWindowEx, parentWindow, childWindowAfter,
                clsName, wndName);
        }

        inline window_handle get_ancestor(_In_ window_handle window, _In_ unsigned int flags)
        {
            return garbage::make_call_fail_on_null<HWND>(GetAncestor, window, flags);
        }

        inline RECT get_client_rect(_In_ window_handle window)
        {
            RECT result;
            garbage::make_boolean_call(GetClientRect, window, &result);

            return result;
        }

        inline window_handle get_desktop_window(void)
        {
            return garbage::make_call_fail_on_null<HWND>(GetDesktopWindow);
        }

        inline window_handle get_foreground_window(void)
        {
            return garbage::make_call_fail_on_null<HWND>(GetForegroundWindow);
        }

        inline window_handle get_last_active_popup(_In_ window_handle owner)
        {
            return garbage::make_call_fail_on_null<HWND>(GetLastActivePopup, owner);
        }

        inline window_handle get_next_window(_In_ window_handle window, _In_ unsigned int dir)
        {
            // GetNextWindow is defined to GetWindow (and apparently GetNextWindow isn't available...)
            return garbage::make_call_fail_on_null<HWND>(GetWindow, window, dir);
        }

        inline window_handle get_parent(_In_ window_handle child)
        {
            return garbage::make_call_fail_on_null<HWND>(GetParent, child);
        }

        inline uint32_t get_process_default_layout(void)
        {
            DWORD result;
            garbage::make_boolean_call(GetProcessDefaultLayout, &result);

            return result;
        }

        inline window_handle get_shell_window(void)
        {
            return garbage::make_call_fail_on_null<HWND>(GetShellWindow);
        }

        inline uint32_t get_sys_color(_In_ int index)
        {
            return GetSysColor(index);
        }

        inline window_handle get_top_window(_In_opt_ window_handle parentWindow = nullptr)
        {
            return garbage::make_call_fail_on_null<HWND>(GetTopWindow, parentWindow);
        }

        inline window_handle get_window(_In_ window_handle window, _In_ unsigned int dir)
        {
            return garbage::make_call_fail_on_null<HWND>(GetWindow, window, dir);
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

        uint32_t get_window_thread_process_id(
            _In_ window_handle window,
            _In_opt_ uint32_t *processId = nullptr)
        {
            // uint32_t* is not implicitly castable to DWORD* even though they will be the same
            // size, so we must reinterpret_cast here
            return garbage::make_call_fail_on_null<uint32_t>(GetWindowThreadProcessId, window,
                reinterpret_cast<DWORD *>(processId));
        }

        std::pair<tid_t, pid_t> get_window_thread_process_id(_In_ window_handle window)
        {
            DWORD processId;
            DWORD threadId = GetWindowThreadProcessId(window, &processId);

            return std::make_pair(threadId, processId);
        }

#pragma endregion
    }
}
