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
#include <functional>
#include <memory>
#include <vector>

#include "windows_exception.h"

namespace dhorn
{
    namespace win32
    {
        /*
         * Type declarations
         */
#pragma region Type Declarations

        static_assert(sizeof(DWORD) == sizeof(uint32_t), "DWORD must be 32 bits");
        static_assert(sizeof(WPARAM) == sizeof(uintptr_t), "WPARAM must be the size of a pointer");
        static_assert(sizeof(LPARAM) == sizeof(intptr_t), "LPARAM must be the size of a pointer");

        using tstring = std::basic_string<TCHAR>;

        using pid_t = uint32_t;
        using tid_t = uint32_t;

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
        namespace garbage
        {
            template <typename Func, typename... Args>
            inline void make_boolean_call(Func &func, Args&&... args)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw_last_error();
                }
            }

            template <typename Func, typename... Args>
            inline void make_hresult_call(Func &func, Args&&... args)
            {
                throw_if_failed(func(std::forward<Args>(args)...));
            }

            template <typename ResultType, ResultType Failure = ResultType{}, typename Func, typename... Args>
            inline ResultType make_call_fail_on_value(Func &func, Args&&... args)
            {
                auto result = func(std::forward<Args>(args)...);
                if (result == Failure)
                {
                    throw_last_error();
                }

                return result;
            }



            inline const TCHAR *null_if_empty(_In_ const tstring &str)
            {
                return ((&str == nullptr) || str.empty()) ? nullptr : str.c_str();
            }
        }



        /*
         * Cursor Functions
         */
#pragma region Cursor Functions

        inline cursor_handle load_cursor(_In_opt_ instance_handle instance, _In_ LPCTSTR name)
        {
            return garbage::make_call_fail_on_value<cursor_handle>(LoadCursor, instance, name);
        }

        inline cursor_handle load_cursor(_In_opt_ instance_handle instance, _In_ const tstring &name)
        {
            return load_cursor(instance, garbage::null_if_empty(name));
        }

        inline void destroy_cursor(_In_ cursor_handle cursor)
        {
            garbage::make_boolean_call(::DestroyCursor, cursor);
        }

#pragma endregion



        /*
         * Device Context Functions
         */
#pragma region Device Context Functions

        inline void cancel_dc(_In_ device_context_handle hdc)
        {
            garbage::make_boolean_call(::CancelDC, hdc);
        }

        inline device_context_handle create_compatible_dc(_In_ device_context_handle hdc)
        {
            return garbage::make_call_fail_on_value<device_context_handle>(::CreateCompatibleDC, hdc);
        }

        inline device_context_handle create_dc(
            _In_opt_ const tstring &driver,
            _In_ const tstring &device,
            _In_opt_ const tstring &output,
            _In_ const DEVMODE *initData)
        {
            return garbage::make_call_fail_on_value<device_context_handle>(
                ::CreateDC,
                garbage::null_if_empty(driver),
                garbage::null_if_empty(device),
                garbage::null_if_empty(output),
                initData);
        }

        inline void delete_dc(_In_ device_context_handle hdc)
        {
            garbage::make_boolean_call(::DeleteDC, hdc);
        }

        inline void delete_object(_In_ gdi_object_handle obj)
        {
            garbage::make_boolean_call(::DeleteObject, obj);
        }

        inline gdi_object_handle get_current_object(_In_ device_context_handle hdc, _In_ unsigned objectType)
        {
            return garbage::make_call_fail_on_value<gdi_object_handle>(::GetCurrentObject, hdc, objectType);
        }

        inline device_context_handle get_dc(_In_ window_handle window = nullptr)
        {
            return garbage::make_call_fail_on_value<device_context_handle>(::GetDC, window);
        }

        inline COLORREF get_dc_brush_color(_In_ device_context_handle hdc)
        {
            return garbage::make_call_fail_on_value<COLORREF, CLR_INVALID>(::GetDCBrushColor, hdc);
        }

        inline void release_dc(_In_ device_context_handle hdc, _In_opt_ window_handle window = nullptr)
        {
            return garbage::make_boolean_call(::ReleaseDC, window, hdc);
        }

#pragma endregion



        /*
         * Dynamic-Link Library Functions
         */
#pragma region Dynamic-Link Library Functions

        inline void free_library(_In_ module_handle handle)
        {
            garbage::make_boolean_call(::FreeLibrary, handle);
        }

#pragma endregion



        /*
         * File Management Functions
         */
#pragma region File Management

        inline handle_t create_file(
            _In_ const tstring &fileName,
            _In_ uint32_t desiredAccess,
            _In_ uint32_t shareMode,
            _In_opt_ LPSECURITY_ATTRIBUTES securityAttributes,
            _In_ uint32_t creationDisposition,
            _In_ uint32_t flagsAndAttributes,
            _In_opt_ handle_t templateFile = nullptr)
        {
            return garbage::make_call_fail_on_value<handle_t, invalid_handle_value>(
                CreateFile,
                garbage::null_if_empty(fileName),
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

        inline void close_handle(_In_ handle_t handle)
        {
            garbage::make_boolean_call(CloseHandle, handle);
        }

        inline handle_t duplicate_handle(
            _In_ handle_t sourceProcess,
            _In_ handle_t sourceHandle,
            _In_ handle_t targetProcess,
            _In_ uint32_t desiredAccess,
            _In_ bool inheritHandle,
            _In_ uint32_t options)
        {
            handle_t result;
            garbage::make_boolean_call(
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

        inline uint32_t get_handle_information(_In_ handle_t handle)
        {
            DWORD flags;
            garbage::make_boolean_call(GetHandleInformation, handle, &flags);

            return flags;
        }

        inline void set_handle_information(_In_ handle_t handle, _In_ uint32_t mask, _In_ uint32_t flags)
        {
            garbage::make_boolean_call(SetHandleInformation, handle, mask, flags);
        }

#pragma endregion



        /*
         * Icon Functions
         */
#pragma region Icon Functions

        inline void destroy_icon(_In_ icon_handle icon)
        {
            garbage::make_boolean_call(::DestroyIcon, icon);
        }

#pragma endregion



        /*
         * Menu Functions
         */
#pragma region Menu Functions

        inline void destroy_menu(_In_ menu_handle menu)
        {
            garbage::make_boolean_call(::DestroyMenu, menu);
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
            _In_opt_ window_handle window = nullptr,
            _In_opt_ unsigned filterMin = 0,
            _In_opt_ unsigned filterMax = 0)
        {
            return !!garbage::make_call_fail_on_value<int, -1>(GetMessage, &msg, window, filterMin, filterMax);
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

        inline bool translate_message(_In_ const MSG &msg)
        {
            // TranslateMessage does not fail
            return !!::TranslateMessage(&msg);
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

        inline std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> get_logical_processor_information(void)
        {
            using SLPI = SYSTEM_LOGICAL_PROCESSOR_INFORMATION;

            DWORD length = 0;
            std::vector<SLPI> result;

            // First attempt. This should fail
            throw_hr_if_true(!!::GetLogicalProcessorInformation(result.data(), &length), E_UNEXPECTED);
            expect_error(ERROR_INSUFFICIENT_BUFFER);

            // Now that we know the expected length, we can allocate what we need
            result.reserve(length / sizeof(SLPI));
            throw_hr_if_false(result.size() * sizeof(SLPI) == length, E_UNEXPECTED);
            garbage::make_boolean_call(GetLogicalProcessorInformation, result.data(), &length);
            throw_hr_if_false(result.size() * sizeof(SLPI) == length, E_UNEXPECTED);

            return result;
        }

        inline uint32_t get_maximum_processor_count(_In_ uint16_t groupNumber = ALL_PROCESSOR_GROUPS)
        {
            return garbage::make_call_fail_on_value<uint32_t>(GetMaximumProcessorCount, groupNumber);
        }

#pragma endregion



        /*
         * Window Class Functions
         */
#pragma region Window Class Functions

        inline WNDCLASS get_class_info(_In_opt_ instance_handle instance, _In_ const tstring &className)
        {
            WNDCLASS result;
            garbage::make_boolean_call(GetClassInfo, instance, garbage::null_if_empty(className), &result);

            return result;
        }

        inline WNDCLASSEX get_class_info_ex(_In_opt_ instance_handle instance, _In_ const tstring &className)
        {
            WNDCLASSEX result;
            garbage::make_boolean_call(GetClassInfoEx, instance, garbage::null_if_empty(className), &result);

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
            // maximum class name is 256 characters (as per MSDN)
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

        inline uintptr_t set_class_long_ptr(_In_ window_handle window, _In_ int index, _In_ uintptr_t value)
        {
            return garbage::make_call_fail_on_value<uintptr_t>(SetClassLongPtr, window, index, value);
        }

        inline uint16_t set_class_word(_In_ window_handle window, _In_ int index, _In_ uint16_t value)
        {
            return garbage::make_call_fail_on_value<uint16_t>(SetClassWord, window, index, value);
        }

        inline long set_window_long(_In_ window_handle window, _In_ int index, _In_ long value)
        {
            return garbage::make_call_fail_on_value<long>(SetWindowLong, window, index, value);
        }

        inline uintptr_t set_window_long_ptr(_In_ window_handle window, _In_ int index, _In_ uintptr_t value)
        {
            ::SetLastError(0);
            return garbage::make_call_fail_on_value<uintptr_t>(SetWindowLongPtr, window, index, value);
        }

        inline void unregister_class(_In_ const tstring &className, _In_opt_ instance_handle instance = nullptr)
        {
            garbage::make_boolean_call(UnregisterClass, garbage::null_if_empty(className), instance);
        }

#pragma endregion



        /*
         * Window Functions
         */
#pragma region Window Functions

        inline RECT adjust_window_rect(_In_ const RECT &inputRect, _In_ uint32_t style, _In_ bool hasMenu)
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

        inline void animate_window(_In_ window_handle window, _In_ uint32_t duration, _In_ uint32_t flags)
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
            garbage::make_boolean_call(
                CalculatePopupWindowPosition,
                &anchorPoint,
                &windowSize,
                flags,
                nullptr,
                &result);

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
            garbage::make_boolean_call(
                CalculatePopupWindowPosition,
                &anchorPoint,
                &windowSize,
                flags,
                const_cast<RECT *>(&exclude),
                &result);

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
            _In_ int x = CW_USEDEFAULT,
            _In_ int y = CW_USEDEFAULT,
            _In_ int width = CW_USEDEFAULT,
            _In_ int height = CW_USEDEFAULT,
            _In_opt_ window_handle parent = nullptr,
            _In_opt_ menu_handle menu = nullptr,
            _In_opt_ instance_handle instance = nullptr,
            _In_opt_ void *param = nullptr)
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(
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
            _In_ uint32_t extendedStyle,
            _In_opt_ const tstring &className,
            _In_opt_ const tstring &windowName,
            _In_ uint32_t style,
            _In_ int x = CW_USEDEFAULT,
            _In_ int y = CW_USEDEFAULT,
            _In_ int width = CW_USEDEFAULT,
            _In_ int height = CW_USEDEFAULT,
            _In_opt_ window_handle parent = nullptr,
            _In_opt_ menu_handle menu = nullptr,
            _In_opt_ instance_handle instance = nullptr,
            _In_opt_ void *param = nullptr)
        {
            auto clsName = garbage::null_if_empty(className);
            auto wndName = garbage::null_if_empty(windowName);

            return garbage::make_call_fail_on_value<HWND>(
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

        inline void destroy_window(_In_ window_handle window)
        {
            garbage::make_boolean_call(DestroyWindow, window);
        }

        inline void enum_child_windows(_In_ window_handle parentWindow, _In_ WNDENUMPROC func, _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumChildWindows, parentWindow, func, param);
        }

        inline void enum_thread_windows(_In_ tid_t threadId, _In_ WNDENUMPROC func, _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumThreadWindows, threadId, func, param);
        }

        inline void enum_windows(_In_ WNDENUMPROC func, _In_ intptr_t param)
        {
            garbage::make_boolean_call(EnumWindows, func, param);
        }

        namespace garbage
        {
            inline BOOL CALLBACK enum_windows_proc(_In_ window_handle childWindow, _In_ LPARAM param)
            {
                auto func = reinterpret_cast<std::function<bool(window_handle)> *>(param);
                return (*func)(childWindow);
            }
        }

        inline void enum_cild_windows(_In_ window_handle parentWindow, _In_ std::function<bool(window_handle)> func)
        {
            enum_child_windows(parentWindow, garbage::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
        }

        inline void enum_thread_windows(_In_ tid_t threadId, _In_ std::function<bool(window_handle)> func)
        {
            enum_thread_windows(threadId, garbage::enum_windows_proc, reinterpret_cast<LPARAM>(&func));
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

            return garbage::make_call_fail_on_value<HWND>(
                FindWindowEx,
                parentWindow,
                childWindowAfter,
                clsName,
                wndName);
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
            // GetNextWindow is defined to GetWindow (and apparently GetNextWindow gives errors...)
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

        inline uint32_t get_window_thread_process_id(_In_ window_handle window, _In_opt_ uint32_t *processId = nullptr)
        {
            // uint32_t* is not implicitly castable to DWORD* even though they will be the same
            // size, so we must reinterpret_cast here
            return garbage::make_call_fail_on_value<uint32_t, 0>(
                GetWindowThreadProcessId,
                window,
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



        /*
         * Window Procedure Functions
         */
#pragma region Window Procedure Functions

        inline intptr_t default_window_procedure(
            _In_ window_handle window,
            _In_ unsigned message,
            _In_ uintptr_t wparam,
            _In_ intptr_t lparam)
        {
            return DefWindowProc(window, message, wparam, lparam);
        }

#pragma endregion
    }
}
