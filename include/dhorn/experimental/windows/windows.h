/*
 * Duncan Horn
 *
 * windows.h
 *
 * Mostly just wraps Win32 functions and "C++-ifys" them, throwing exceptions on failure
 */
#pragma once

#if !defined(WIN32) && !defined(_WIN64)
static_assert(false, "Cannot include dhorn/windows/windows.h on a non-Windows machine")
#endif  /* WIN32 */

#ifndef NOMINMAX
static_assert(false, "dhorn/windows/windows.h requires that std::min/std::max be usable");
#endif

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "../../com/hresult_error.h"

namespace dhorn
{
    namespace experimental
    {
        namespace win32
        {
            /*
             * Type declarations
             */
#pragma region Type Declarations

            static_assert(sizeof(DWORD) == sizeof(std::uint32_t), "DWORD must be 32 bits");
            static_assert(sizeof(WPARAM) == sizeof(std::uintptr_t), "WPARAM must be the size of a pointer");
            static_assert(sizeof(LPARAM) == sizeof(std::intptr_t), "LPARAM must be the size of a pointer");

            using tstring = std::basic_string<TCHAR>;

            using pid_t = std::uint32_t;
            using tid_t = std::uint32_t;

            using handle_t = HANDLE;
            using bitmap_handle = HBITMAP;
            using brush_handle = HBRUSH;
            using cursor_handle = HCURSOR;
            using device_context_handle = HDC;
            using gdi_object_handle = HGDIOBJ;
            using icon_handle = HICON;
            using instance_handle = HINSTANCE;
            using menu_handle = HMENU;
            using module_handle = HMODULE;
            using window_handle = HWND;

            static const handle_t invalid_handle_value = INVALID_HANDLE_VALUE;

#pragma endregion



            /*
             * Helpers
             */
            namespace details
            {
                template <typename Func, typename... Args>
                inline void make_boolean_call(Func &func, Args&&... args)
                {
                    if (!func(std::forward<Args>(args)...))
                    {
                        throw std::system_error(::GetLastError(), std::system_category());
                    }
                }

                template <typename Func, typename... Args>
                inline void make_hresult_call(Func &func, Args&&... args)
                {
                    com::check_hresult(func(std::forward<Args>(args)...));
                }

                template <typename ResultType, ResultType Failure = ResultType{}, typename Func, typename... Args>
                inline ResultType make_call_fail_on_value(Func &func, Args&&... args)
                {
                    auto result = func(std::forward<Args>(args)...);
                    if (result == Failure)
                    {
                        auto err = ::GetLastError();
                        throw std::system_error(err, std::system_category());
                    }

                    return result;
                }



                inline const TCHAR *null_if_empty(const tstring &str)
                {
                    return ((&str == nullptr) || str.empty()) ? nullptr : str.c_str();
                }
            }



            /*
             * Cursor Functions
             */
#pragma region Cursor Functions

            inline cursor_handle load_cursor(instance_handle instance, LPCTSTR name)
            {
                return details::make_call_fail_on_value<cursor_handle>(LoadCursor, instance, name);
            }

            inline cursor_handle load_cursor(instance_handle instance, const tstring &name)
            {
                return load_cursor(instance, details::null_if_empty(name));
            }

            inline void destroy_cursor(cursor_handle cursor)
            {
                details::make_boolean_call(::DestroyCursor, cursor);
            }

#pragma endregion



            /*
             * Device Context Functions
             */
#pragma region Device Context Functions

            inline void cancel_dc(device_context_handle hdc)
            {
                details::make_boolean_call(::CancelDC, hdc);
            }

            inline device_context_handle create_compatible_dc(device_context_handle hdc)
            {
                return details::make_call_fail_on_value<device_context_handle>(::CreateCompatibleDC, hdc);
            }

            inline device_context_handle create_dc(
                const tstring &driver,
                const tstring &device,
                const tstring &output,
                const DEVMODE *initData)
            {
                return details::make_call_fail_on_value<device_context_handle>(
                    ::CreateDC,
                    details::null_if_empty(driver),
                    details::null_if_empty(device),
                    details::null_if_empty(output),
                    initData);
            }

            inline void delete_dc(device_context_handle hdc)
            {
                details::make_boolean_call(::DeleteDC, hdc);
            }

            inline void delete_object(gdi_object_handle obj)
            {
                details::make_boolean_call(::DeleteObject, obj);
            }

            inline gdi_object_handle get_current_object(device_context_handle hdc, unsigned objectType)
            {
                return details::make_call_fail_on_value<gdi_object_handle>(::GetCurrentObject, hdc, objectType);
            }

            inline device_context_handle get_dc(window_handle window = nullptr)
            {
                return details::make_call_fail_on_value<device_context_handle>(::GetDC, window);
            }

            inline COLORREF get_dc_brush_color(device_context_handle hdc)
            {
                return details::make_call_fail_on_value<COLORREF, CLR_INVALID>(::GetDCBrushColor, hdc);
            }

            inline void release_dc(device_context_handle hdc, window_handle window = nullptr)
            {
                return details::make_boolean_call(::ReleaseDC, window, hdc);
            }

#pragma endregion



            /*
             * Dynamic-Link Library Functions
             */
#pragma region Dynamic-Link Library Functions

            inline void free_library(module_handle handle)
            {
                details::make_boolean_call(::FreeLibrary, handle);
            }

#pragma endregion



            /*
             * File Management Functions
             */
#pragma region File Management

            inline handle_t create_file(
                const tstring &fileName,
                std::uint32_t desiredAccess,
                std::uint32_t shareMode,
                LPSECURITY_ATTRIBUTES securityAttributes,
                std::uint32_t creationDisposition,
                std::uint32_t flagsAndAttributes,
                handle_t templateFile = nullptr)
            {
                return details::make_call_fail_on_value<handle_t, invalid_handle_value>(
                    CreateFile,
                    details::null_if_empty(fileName),
                    desiredAccess,
                    shareMode,
                    securityAttributes,
                    creationDisposition,
                    flagsAndAttributes,
                    templateFile);
            }

#pragma endregion



            /*
             * Handle and Object Functions
             */
#pragma region Handle and Object Functions

            inline void close_handle(handle_t handle)
            {
                details::make_boolean_call(CloseHandle, handle);
            }

            inline handle_t duplicate_handle(
                handle_t sourceProcess,
                handle_t sourceHandle,
                handle_t targetProcess,
                std::uint32_t desiredAccess,
                bool inheritHandle,
                std::uint32_t options)
            {
                handle_t result;
                details::make_boolean_call(
                    DuplicateHandle,
                    sourceProcess,
                    sourceHandle,
                    targetProcess,
                    &result,
                    desiredAccess,
                    inheritHandle,
                    options);

                return result;
            }

            inline std::uint32_t get_handle_information(handle_t handle)
            {
                DWORD flags;
                details::make_boolean_call(GetHandleInformation, handle, &flags);

                return flags;
            }

            inline void set_handle_information(handle_t handle, std::uint32_t mask, std::uint32_t flags)
            {
                details::make_boolean_call(SetHandleInformation, handle, mask, flags);
            }

#pragma endregion



            /*
             * Icon Functions
             */
#pragma region Icon Functions

            inline void destroy_icon(icon_handle icon)
            {
                details::make_boolean_call(::DestroyIcon, icon);
            }

#pragma endregion



            /*
             * Menu Functions
             */
#pragma region Menu Functions

            inline void destroy_menu(menu_handle menu)
            {
                details::make_boolean_call(::DestroyMenu, menu);
            }

#pragma endregion



            /*
             * Message Functions
             */
#pragma region Message Functions

            inline std::intptr_t dispatch_message(const MSG &msg)
            {
                return DispatchMessage(&msg);
            }

            inline bool get_message(
                MSG &msg,
                window_handle window = nullptr,
                unsigned filterMin = 0,
                unsigned filterMax = 0)
            {
                return !!details::make_call_fail_on_value<int, -1>(GetMessage, &msg, window, filterMin, filterMax);
            }

            inline bool peek_message(
                window_handle window,
                unsigned filterMin,
                unsigned filterMax,
                unsigned remove,
                MSG *result)
            {
                return !!PeekMessage(result, window, filterMin, filterMax, remove);
            }

            inline void post_message(window_handle window, unsigned msg, std::uintptr_t wparam, std::intptr_t lparam)
            {
                details::make_boolean_call(PostMessage, window, msg, wparam, lparam);
            }

            inline void post_quit_message(int exitCode)
            {
                PostQuitMessage(exitCode);
            }

            inline bool translate_message(const MSG &msg)
            {
                // TranslateMessage does not fail
                return !!::TranslateMessage(&msg);
            }

#pragma endregion



            /*
             * Painting and Drawing Functions
             */
#pragma region Painting and Drawing Functions

            inline device_context_handle begin_paint(window_handle window, PAINTSTRUCT *ps)
            {
                return details::make_call_fail_on_value<HDC>(::BeginPaint, window, ps);
            }

            inline void end_paint(window_handle window, const PAINTSTRUCT &ps)
            {
                details::make_boolean_call(::EndPaint, window, &ps);
            }

            inline void invalidate_rect(window_handle window, RECT *area, bool eraseBackground)
            {
                details::make_boolean_call(::InvalidateRect, window, area, eraseBackground);
            }

            inline void update_window(window_handle window)
            {
                details::make_boolean_call(UpdateWindow, window);
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

            inline std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> get_logical_processor_information(void)
            {
                using SLPI = SYSTEM_LOGICAL_PROCESSOR_INFORMATION;

                DWORD length = 0;
                std::vector<SLPI> result;

                // First attempt. This should fail
                if (!::GetLogicalProcessorInformation(result.data(), &length))
                {
                    com::throw_hresult(E_UNEXPECTED);
                }

                auto error = ::GetLastError();
                if (error && (error != ERROR_INSUFFICIENT_BUFFER))
                {
                    throw std::system_error(error, std::system_category());
                }

                // Now that we know the expected length, we can allocate what we need
                result.reserve(length / sizeof(SLPI));
                if (result.size() * sizeof(SLPI) != length)
                {
                    com::throw_hresult(E_UNEXPECTED);
                }

                details::make_boolean_call(GetLogicalProcessorInformation, result.data(), &length);
                if (result.size() * sizeof(SLPI) != length)
                {
                    com::throw_hresult(E_UNEXPECTED);
                }

                return result;
            }

            inline std::uint32_t get_maximum_processor_count(std::uint16_t groupNumber = ALL_PROCESSOR_GROUPS)
            {
                return details::make_call_fail_on_value<std::uint32_t>(GetMaximumProcessorCount, groupNumber);
            }

#pragma endregion



            /*
             * Window Class Functions
             */
#pragma region Window Class Functions

            inline WNDCLASS get_class_info(instance_handle instance, const tstring &className)
            {
                WNDCLASS result;
                details::make_boolean_call(GetClassInfo, instance, details::null_if_empty(className), &result);

                return result;
            }

            inline WNDCLASSEX get_class_info_ex(instance_handle instance, const tstring &className)
            {
                WNDCLASSEX result;
                details::make_boolean_call(GetClassInfoEx, instance, details::null_if_empty(className), &result);

                return result;
            }

            inline std::uint32_t get_class_long(window_handle window, int index)
            {
                return details::make_call_fail_on_value<std::uint32_t>(GetClassLong, window, index);
            }

            inline std::uintptr_t get_class_long_ptr(window_handle window, int index)
            {
                return details::make_call_fail_on_value<std::uintptr_t>(GetClassLongPtr, window, index);
            }

            inline tstring get_class_name(window_handle window)
            {
                // maximum class name is 256 characters (as per MSDN)
                TCHAR buffer[257];
                auto pos = details::make_call_fail_on_value<int>(GetClassName, window, buffer, 257);
                buffer[pos] = '\0';

                return buffer;
            }

            inline std::uint16_t get_class_word(window_handle window, int index)
            {
                return details::make_call_fail_on_value<std::uint16_t>(GetClassWord, window, index);
            }

            inline long get_window_long(window_handle window, int index)
            {
                return details::make_call_fail_on_value<long>(GetWindowLong, window, index);
            }

            inline std::uintptr_t get_window_long_ptr(window_handle window, int index)
            {
                return details::make_call_fail_on_value<std::uintptr_t>(GetWindowLongPtr, window, index);
            }

            inline ATOM register_class(const WNDCLASS &wndClass)
            {
                return details::make_call_fail_on_value<ATOM>(RegisterClass, &wndClass);
            }

            inline ATOM register_class(const WNDCLASSEX &wndClass)
            {
                return details::make_call_fail_on_value<ATOM>(RegisterClassEx, &wndClass);
            }

            inline std::uint32_t set_class_long(window_handle window, int index, long value)
            {
                return details::make_call_fail_on_value<std::uint32_t>(SetClassLong, window, index, value);
            }

            inline std::uintptr_t set_class_long_ptr(window_handle window, int index, std::uintptr_t value)
            {
                return details::make_call_fail_on_value<std::uintptr_t>(SetClassLongPtr, window, index, value);
            }

            inline std::uint16_t set_class_word(window_handle window, int index, std::uint16_t value)
            {
                return details::make_call_fail_on_value<std::uint16_t>(SetClassWord, window, index, value);
            }

            inline long set_window_long(window_handle window, int index, long value)
            {
                return details::make_call_fail_on_value<long>(SetWindowLong, window, index, value);
            }

            inline std::uintptr_t set_window_long_ptr(window_handle window, int index, std::uintptr_t value)
            {
                ::SetLastError(0);
                auto result = ::SetWindowLongPtr(window, index, value);
                if (result == 0)
                {
                    if (auto err = ::GetLastError(); err != 0)
                    {
                        throw std::system_error(err, std::system_category());
                    }
                }

                return result;
            }

            inline void unregister_class(const tstring &className, instance_handle instance = nullptr)
            {
                details::make_boolean_call(UnregisterClass, details::null_if_empty(className), instance);
            }

#pragma endregion



            /*
             * Window Functions
             */
#pragma region Window Functions

            inline RECT adjust_window_rect(const RECT &inputRect, std::uint32_t style, bool hasMenu)
            {
                RECT result = inputRect;
                details::make_boolean_call(AdjustWindowRect, &result, style, hasMenu);

                return result;
            }

            inline RECT adjust_window_rect(const RECT &inputRect, std::uint32_t style, bool hasMenu, std::uint32_t extendedStyle)
            {
                RECT result = inputRect;
                details::make_boolean_call(AdjustWindowRectEx, &result, style, hasMenu, extendedStyle);

                return result;
            }

            inline void allow_set_foreground_window(pid_t processId)
            {
                details::make_boolean_call(AllowSetForegroundWindow, processId);
            }

            inline void animate_window(window_handle window, std::uint32_t duration, std::uint32_t flags)
            {
                details::make_boolean_call(AnimateWindow, window, duration, flags);
            }

            inline bool any_popup(void)
            {
                return !!AnyPopup();
            }

            inline void bring_window_to_top(window_handle window)
            {
                details::make_boolean_call(BringWindowToTop, window);
            }

            inline RECT calculate_popup_window_position(
                const POINT &anchorPoint,
                const SIZE &windowSize,
                unsigned int flags)
            {
                RECT result;
                details::make_boolean_call(
                    CalculatePopupWindowPosition,
                    &anchorPoint,
                    &windowSize,
                    flags,
                    nullptr,
                    &result);

                return result;
            }

            inline RECT calculate_popup_window_position(
                const POINT &anchorPoint,
                const SIZE &windowSize,
                unsigned int flags,
                const RECT &exclude)
            {
                RECT result;

                // The exclude rectangle is marked as _In_, but is non-const. Thus, the const_cast
                details::make_boolean_call(
                    CalculatePopupWindowPosition,
                    &anchorPoint,
                    &windowSize,
                    flags,
                    const_cast<RECT *>(&exclude),
                    &result);

                return result;
            }

            inline void close_window(window_handle window)
            {
                details::make_boolean_call(CloseWindow, window);
            }

            inline window_handle create_window(
                const tstring &className,
                const tstring &windowName,
                std::uint32_t style,
                int x = CW_USEDEFAULT,
                int y = CW_USEDEFAULT,
                int width = CW_USEDEFAULT,
                int height = CW_USEDEFAULT,
                window_handle parent = nullptr,
                menu_handle menu = nullptr,
                instance_handle instance = nullptr,
                void *param = nullptr)
            {
                auto clsName = details::null_if_empty(className);
                auto wndName = details::null_if_empty(windowName);

                return details::make_call_fail_on_value<HWND>(
                    CreateWindowEx,
                    0L,
                    clsName,
                    wndName,
                    style,
                    x,
                    y,
                    width,
                    height,
                    parent,
                    menu,
                    instance,
                    param);
            }

            inline window_handle create_window(
                std::uint32_t extendedStyle,
                const tstring &className,
                const tstring &windowName,
                std::uint32_t style,
                int x = CW_USEDEFAULT,
                int y = CW_USEDEFAULT,
                int width = CW_USEDEFAULT,
                int height = CW_USEDEFAULT,
                window_handle parent = nullptr,
                menu_handle menu = nullptr,
                instance_handle instance = nullptr,
                void *param = nullptr)
            {
                auto clsName = details::null_if_empty(className);
                auto wndName = details::null_if_empty(windowName);

                return details::make_call_fail_on_value<HWND>(
                    CreateWindowEx,
                    extendedStyle,
                    clsName,
                    wndName,
                    style,
                    x, y,
                    width, height,
                    parent,
                    menu,
                    instance,
                    param);
            }

            inline void destroy_window(window_handle window)
            {
                details::make_boolean_call(DestroyWindow, window);
            }

            inline void enum_child_windows(window_handle parentWindow, WNDENUMPROC func, std::intptr_t param)
            {
                details::make_boolean_call(EnumChildWindows, parentWindow, func, param);
            }

            inline void enum_thread_windows(tid_t threadId, WNDENUMPROC func, std::intptr_t param)
            {
                details::make_boolean_call(EnumThreadWindows, threadId, func, param);
            }

            inline void enum_windows(WNDENUMPROC func, std::intptr_t param)
            {
                details::make_boolean_call(EnumWindows, func, param);
            }

            namespace details
            {
                inline BOOL CALLBACK enum_windows_proc(window_handle childWindow, LPARAM param)
                {
                    auto func = reinterpret_cast<std::function<bool(window_handle)> *>(param);
                    return (*func)(childWindow);
                }
            }

            inline void enum_cild_windows(window_handle parentWindow, std::function<bool(window_handle)> func)
            {
                enum_child_windows(parentWindow, details::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
            }

            inline void enum_thread_windows(tid_t threadId, std::function<bool(window_handle)> func)
            {
                enum_thread_windows(threadId, details::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
            }

            inline void enum_windows(std::function<bool(window_handle)> func)
            {
                enum_windows(details::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
            }

            inline window_handle find_window(const tstring &className = TEXT(""), const tstring &windowName = TEXT(""))
            {
                auto clsName = details::null_if_empty(className);
                auto wndName = details::null_if_empty(windowName);

                return details::make_call_fail_on_value<HWND>(FindWindow, clsName, wndName);
            }

            inline window_handle find_window(
                window_handle parentWindow,
                window_handle childWindowAfter,
                const tstring &className = TEXT(""),
                const tstring &windowName = TEXT(""))
            {
                auto clsName = details::null_if_empty(className);
                auto wndName = details::null_if_empty(windowName);

                return details::make_call_fail_on_value<HWND>(
                    FindWindowEx,
                    parentWindow,
                    childWindowAfter,
                    clsName,
                    wndName);
            }

            inline window_handle get_ancestor(window_handle window, unsigned int flags)
            {
                return details::make_call_fail_on_value<HWND>(GetAncestor, window, flags);
            }

            inline RECT get_client_rect(window_handle window)
            {
                RECT result;
                details::make_boolean_call(GetClientRect, window, &result);

                return result;
            }

            inline window_handle get_desktop_window(void)
            {
                return details::make_call_fail_on_value<HWND>(GetDesktopWindow);
            }

            inline window_handle get_foreground_window(void)
            {
                return details::make_call_fail_on_value<HWND>(GetForegroundWindow);
            }

            inline window_handle get_last_active_popup(window_handle owner)
            {
                return details::make_call_fail_on_value<HWND>(GetLastActivePopup, owner);
            }

            inline window_handle get_next_window(window_handle window, unsigned int dir)
            {
                // GetNextWindow is defined to GetWindow (and apparently GetNextWindow gives errors...)
                return details::make_call_fail_on_value<HWND>(GetWindow, window, dir);
            }

            inline window_handle get_parent(window_handle child)
            {
                return details::make_call_fail_on_value<HWND>(GetParent, child);
            }

            inline std::uint32_t get_process_default_layout(void)
            {
                DWORD result;
                details::make_boolean_call(GetProcessDefaultLayout, &result);

                return result;
            }

            inline window_handle get_shell_window(void)
            {
                return details::make_call_fail_on_value<HWND>(GetShellWindow);
            }

            inline std::uint32_t get_sys_color(int index)
            {
                return GetSysColor(index);
            }

            inline window_handle get_top_window(window_handle parentWindow = nullptr)
            {
                return details::make_call_fail_on_value<HWND>(GetTopWindow, parentWindow);
            }

            inline window_handle get_window(window_handle window, unsigned int dir)
            {
                return details::make_call_fail_on_value<HWND>(GetWindow, window, dir);
            }

            inline RECT get_window_rect(window_handle window)
            {
                RECT result;
                details::make_boolean_call(GetWindowRect, window, &result);

                return result;
            }

            inline void get_window_text(window_handle window, TCHAR *text, int len)
            {
                GetWindowText(window, text, len);
            }

            template <std::size_t size>
            inline void get_window_text(window_handle window, TCHAR text[size])
            {
                GetWindowText(window, text, size);
            }

            template <int max_length = UINT_MAX>
            inline tstring get_window_text(window_handle window)
            {
                // Get the estimated size
                std::size_t size = std::min(GetWindowTextLength(window), max_length) + 1;
                std::unique_ptr<TCHAR> str(new TCHAR[size]);
                GetWindowText(window, str.get(), size);

                tstring result = str.get();
                return result;
            }

            inline std::uint32_t get_window_thread_process_id(window_handle window, std::uint32_t *processId = nullptr)
            {
                // std::uint32_t* is not implicitly castable to DWORD* even though they will be the same
                // size, so we must reinterpret_cast here
                return details::make_call_fail_on_value<std::uint32_t, 0>(
                    GetWindowThreadProcessId,
                    window,
                    reinterpret_cast<DWORD *>(processId));
            }

            inline std::pair<tid_t, pid_t> get_window_thread_process_id(window_handle window)
            {
                DWORD processId;
                DWORD threadId = GetWindowThreadProcessId(window, &processId);

                return std::make_pair(threadId, processId);
            }

            inline void show_window(window_handle window, int cmdShow)
            {
                ::ShowWindow(window, cmdShow);
            }

#pragma endregion



            /*
             * Window Procedure Functions
             */
#pragma region Window Procedure Functions

            inline std::intptr_t default_window_procedure(
                window_handle window,
                unsigned message,
                std::uintptr_t wparam,
                std::intptr_t lparam)
            {
                return DefWindowProc(window, message, wparam, lparam);
            }

#pragma endregion
        }
    }
}
