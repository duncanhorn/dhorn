/*
 * Duncan Horn
 *
 * window.h
 *
 * An easy way to implement a win32 window.
 *
 * Window messages are handled as follows:
 *
 *      -   Messages that arrive before WM_NCCREATE are processed by DefWindowProc. This behavior is mostly unavoidable
 *          as a majority of the class would have to be re-written for it to (safely) behave any different.
 *      -   Messages that arrive after WM_NCCREATE are handled by the window_procedure function. While this function is
 *          declared as virtual, it is not recommended that you override this function as its default behavior should
 *          be good enough and gives you a lot free.
 *      -   From here, window_procedure looks up the list of callback handlers for the input message. If none exist, it
 *          moves on to the next step. If handler(s) do exist, the window_procedure function iterates all handlers
 *          invoking their callback function. If the function returns true, the handler's invoke count is decremented
 *          (except when the count is infinite), and the handler is removed from the list if its invoke count reaches
 *          zero. The window_procedure function then looks at the handler's eat_message flag (note that this is done
 *          only when the handler returns true (handled)). If the flag is set to true, then the message routing stops
 *          there and (true, result) is returned to the window_procedure. If this flag is set to false, or if the
 *          handler does not handle the message, the process is repeated for the next handler in the list until the end
 *          of the list is reached, or until a handler handles a message and its eat_message flag is set to true. In
 *          the event that multiple handlers handle a request, the result of the LAST handler to say that it handled a
 *          message is reported back to the system.
 *              *   Note that a "transparent" handler can be implemented by handling a message each time, but always
 *                  returning false. It should be noted that if no other handler exists to process the message, then
 *                  DefWindowProc will get called, which may not be desirable in some situations.
 *      -   If no handler returns true, then the DefWindowProc is called and its result is reported to the system.
 */
#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

#include "../math.h"
#include "../scope_exit.h"
#include "windows.h"

namespace dhorn
{
    namespace win32
    {
        /*
         * Types
         */
#pragma region Types

        enum class window_class_style : unsigned
        {
            vertical_redraw         = CS_VREDRAW,
            horizontal_redraw       = CS_HREDRAW,
            double_clicks           = CS_DBLCLKS,
            own_device_context      = CS_OWNDC,
            class_device_context    = CS_CLASSDC,
            parent_device_context   = CS_PARENTDC,
            no_close                = CS_NOCLOSE,
            save_bits               = CS_SAVEBITS,
            byte_align_client       = CS_BYTEALIGNCLIENT,
            byte_align_window       = CS_BYTEALIGNWINDOW,
            global_class            = CS_GLOBALCLASS,
            drop_shadow             = CS_DROPSHADOW,
        };

        inline window_class_style operator|(window_class_style lhs, window_class_style rhs)
        {
            return static_cast<window_class_style>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
        }

        inline window_class_style operator&(window_class_style lhs, window_class_style rhs)
        {
            return static_cast<window_class_style>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
        }

        enum class system_color : uintptr_t
        {
            scroll_bar              = COLOR_SCROLLBAR,
            background              = COLOR_BACKGROUND,
            active_caption          = COLOR_ACTIVECAPTION,
            inactive_caption        = COLOR_INACTIVECAPTION,
            menu                    = COLOR_MENU,
            window                  = COLOR_WINDOW,
            window_frame            = COLOR_WINDOWFRAME,
            menu_text               = COLOR_MENUTEXT,
            window_text             = COLOR_WINDOWTEXT,
            caption_text            = COLOR_CAPTIONTEXT,
            active_border           = COLOR_ACTIVEBORDER,
            inactive_border         = COLOR_INACTIVEBORDER,
            app_workspace           = COLOR_APPWORKSPACE,
            highlight               = COLOR_HIGHLIGHT,
            highlight_text          = COLOR_HIGHLIGHTTEXT,
            button_face             = COLOR_BTNFACE,
            button_shadow           = COLOR_BTNSHADOW,
            gray_text               = COLOR_GRAYTEXT,
            button_text             = COLOR_BTNTEXT,
            inactive_caption_text   = COLOR_INACTIVECAPTIONTEXT,
            button_highlight        = COLOR_BTNHIGHLIGHT,
        };

        enum class window_style : uint32_t
        {
            border                  = WS_BORDER,
            caption                 = WS_CAPTION,
            child                   = WS_CHILD,
            child_window            = WS_CHILDWINDOW,
            clip_children           = WS_CLIPCHILDREN,
            clip_siblings           = WS_CLIPSIBLINGS,
            disabled                = WS_DISABLED,
            dialog_frame            = WS_DLGFRAME,
            group                   = WS_GROUP,
            horizontal_scroll       = WS_HSCROLL,
            iconic                  = WS_ICONIC,
            maximize                = WS_MAXIMIZE,
            maximize_box            = WS_MAXIMIZEBOX,
            minimize                = WS_MINIMIZE,
            minimize_box            = WS_MINIMIZEBOX,
            overlapped              = WS_OVERLAPPED,
            overlapped_window       = WS_OVERLAPPEDWINDOW,
            popup                   = WS_POPUP,
            popup_window            = WS_POPUPWINDOW,
            size_box                = WS_SIZEBOX,
            system_menu             = WS_SYSMENU,
            tab_stop                = WS_TABSTOP,
            thick_frame             = WS_THICKFRAME,
            tiled                   = WS_TILED,
            tiled_window            = WS_TILEDWINDOW,
            visible                 = WS_VISIBLE,
            vertical_scroll         = WS_VSCROLL,
        };

        inline window_style operator|(window_style lhs, window_style rhs)
        {
            return static_cast<window_style>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
        }

        inline window_style operator&(window_style lhs, window_style rhs)
        {
            return static_cast<window_style>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
        }

        enum class window_message : unsigned
        {
            // Clipboard
            ask_clipboard_format_name               = WM_ASKCBFORMATNAME,
            change_clipboard_chain                  = WM_CHANGECBCHAIN,
            clear                                   = WM_CLEAR,
            clipboard_update                        = WM_CLIPBOARDUPDATE,
            copy                                    = WM_COPY,
            cut                                     = WM_CUT,
            destroy_clipboard                       = WM_DESTROYCLIPBOARD,
            draw_clipboard                          = WM_DRAWCLIPBOARD,
            horizontal_scroll_clipboard             = WM_HSCROLLCLIPBOARD,
            paint_clipboard                         = WM_PAINTCLIPBOARD,
            paste                                   = WM_PASTE,
            render_all_formats                      = WM_RENDERALLFORMATS,
            render_format                           = WM_RENDERFORMAT,
            size_clipboard                          = WM_SIZECLIPBOARD,
            vertical_scroll_clipboard               = WM_VSCROLLCLIPBOARD,

            // Cursor
            set_cursor                              = WM_SETCURSOR,

            // Data Copy
            copy_data                               = WM_COPYDATA,

            // Desktop Window Manager (DWM)
            dwm_colorization_color_changed          = WM_DWMCOLORIZATIONCOLORCHANGED,
            dwm_composition_changed                 = WM_DWMCOMPOSITIONCHANGED,
            dwm_non_client_rendering_changed        = WM_DWMNCRENDERINGCHANGED,
            dwm_send_iconic_live_preview_bitmap     = WM_DWMSENDICONICLIVEPREVIEWBITMAP,
            dwm_send_iconic_thumbnail               = WM_DWMSENDICONICTHUMBNAIL,
            dwm_window_maximized_change             = WM_DWMWINDOWMAXIMIZEDCHANGE,

            // Device Management
            device_change                           = WM_DEVICECHANGE,

            // Dialog Box
            control_color_dialog                    = WM_CTLCOLORDLG,
            enter_idle                              = WM_ENTERIDLE,
            get_dialog_code                         = WM_GETDLGCODE,
            init_dialog                             = WM_INITDIALOG,
            next_dialog_control                     = WM_NEXTDLGCTL,

            // Hook
            cancel_journal                          = WM_CANCELJOURNAL,
            queue_sync                              = WM_QUEUESYNC,

            // Keyboard Accelerator
            change_ui_state                         = WM_CHANGEUISTATE,
            init_menu                               = WM_INITMENU,
            init_menu_popup                         = WM_INITMENUPOPUP,
            menu_character                          = WM_MENUCHAR,
            menu_select                             = WM_MENUSELECT,
            query_ui_state                          = WM_QUERYUISTATE,
            system_character                        = WM_SYSCHAR,
            system_command                          = WM_SYSCOMMAND,
            update_ui_state                         = WM_UPDATEUISTATE,

            // Keyboard Input
            activate                                = WM_ACTIVATE,
            application_command                     = WM_APPCOMMAND,
            character                               = WM_CHAR,
            dead_character                          = WM_DEADCHAR,
            get_hotkey                              = WM_GETHOTKEY,
            hotkey                                  = WM_HOTKEY,
            key_down                                = WM_KEYDOWN,
            key_up                                  = WM_KEYUP,
            kill_focus                              = WM_KILLFOCUS,
            set_focus                               = WM_SETFOCUS,
            set_hotkey                              = WM_SETHOTKEY,
            system_dead_character                   = WM_SYSDEADCHAR,
            system_key_up                           = WM_SYSKEYUP,
            unicode_character                       = WM_UNICHAR,

            // Menu
            command                                 = WM_COMMAND,
            context_menu                            = WM_CONTEXTMENU,
            enter_menu_loop                         = WM_ENTERMENULOOP,
            exit_menu_loop                          = WM_EXITMENULOOP,
            get_title_bar_info_ex                   = WM_GETTITLEBARINFOEX,
            menu_command                            = WM_MENUCOMMAND,
            menu_drag                               = WM_MENUDRAG,
            menu_get_object                         = WM_MENUGETOBJECT,
            menu_right_button_up                    = WM_MENURBUTTONUP,
            next_menu                               = WM_NEXTMENU,
            uninitialize_menu_popup                 = WM_UNINITMENUPOPUP,

            // Mouse Input
            capture_changed                         = WM_CAPTURECHANGED,
            left_button_double_click                = WM_LBUTTONDBLCLK,
            left_button_down                        = WM_LBUTTONDOWN,
            left_button_up                          = WM_LBUTTONUP,
            middle_button_double_click              = WM_MBUTTONDBLCLK,
            middle_button_down                      = WM_MBUTTONDOWN,
            middle_button_up                        = WM_MBUTTONUP,
            mouse_activate                          = WM_MOUSEACTIVATE,
            mouse_hover                             = WM_MOUSEHOVER,
            mouse_horizontal_wheel                  = WM_MOUSEHWHEEL,
            mouse_leave                             = WM_MOUSELEAVE,
            mouse_move                              = WM_MOUSEMOVE,
            mouse_wheel                             = WM_MOUSEWHEEL,
            non_client_hit_test                     = WM_NCHITTEST,
            non_client_left_button_double_click     = WM_NCLBUTTONDBLCLK,
            non_client_left_button_down             = WM_NCLBUTTONDOWN,
            non_client_left_button_up               = WM_NCLBUTTONUP,
            non_client_middle_button_double_click   = WM_NCMBUTTONDBLCLK,
            non_client_middle_button_down           = WM_NCMBUTTONDOWN,
            non_client_middle_button_up             = WM_NCMBUTTONUP,
            non_client_mouse_hover                  = WM_NCMOUSEHOVER,
            non_client_mouse_leave                  = WM_NCMOUSELEAVE,
            non_client_mouse_move                   = WM_NCMOUSEMOVE,
            non_client_right_button_double_click    = WM_NCRBUTTONDBLCLK,
            non_client_right_button_down            = WM_NCRBUTTONDOWN,
            non_client_right_button_up              = WM_NCRBUTTONUP,
            non_client_close_button_double_click    = WM_NCXBUTTONDBLCLK,
            non_client_close_button_down            = WM_NCXBUTTONDOWN,
            non_client_close_button_up              = WM_NCXBUTTONUP,
            right_button_double_click               = WM_RBUTTONDBLCLK,
            right_button_down                       = WM_RBUTTONDOWN,
            right_button_up                         = WM_RBUTTONUP,
            close_button_double_click               = WM_XBUTTONDBLCLK,
            close_button_down                       = WM_XBUTTONDOWN,
            close_button_up                         = WM_XBUTTONUP,

            // Multiple Document Interface
            mdi_activate                            = WM_MDIACTIVATE,
            mdi_cascade                             = WM_MDICASCADE,
            mdi_create                              = WM_MDICREATE,
            mdi_destroy                             = WM_MDIDESTROY,
            mdi_get_active                          = WM_MDIGETACTIVE,
            mdi_icon_arrange                        = WM_MDIICONARRANGE,
            mdi_maximize                            = WM_MDIMAXIMIZE,
            mdi_next                                = WM_MDINEXT,
            mdi_refresh_menu                        = WM_MDIREFRESHMENU,
            mdi_restore                             = WM_MDIRESTORE,
            mdi_set_menu                            = WM_MDISETMENU,
            mdi_tile                                = WM_MDITILE,

            // Raw Input
            input                                   = WM_INPUT,
            input_device_change                     = WM_INPUT_DEVICE_CHANGE,

            // Scroll Bar
            control_color_scroll_bar                = WM_CTLCOLORSCROLLBAR,
            horizontal_scroll                       = WM_HSCROLL,
            vertical_scroll                         = WM_VSCROLL,

            // Timer
            timer                                   = WM_TIMER,

            // Window
            activate_application                    = WM_ACTIVATEAPP,
            cancel_mode                             = WM_CANCELMODE,
            child_activate                          = WM_CHILDACTIVATE,
            close                                   = WM_CLOSE,
            compacting                              = WM_COMPACTING,
            create                                  = WM_CREATE,
            destroy                                 = WM_DESTROY,
            dpi_changed                             = WM_DPICHANGED,
            enable                                  = WM_ENABLE,
            enter_size_move                         = WM_ENTERSIZEMOVE,
            erase_background                        = WM_ERASEBKGND,
            exit_size_move                          = WM_EXITSIZEMOVE,
            get_font                                = WM_GETFONT,
            get_hmenu                               = MN_GETHMENU,
            get_icon                                = WM_GETICON,
            get_min_max_info                        = WM_GETMINMAXINFO,
            get_text                                = WM_GETTEXT,
            get_text_length                         = WM_GETTEXTLENGTH,
            input_language_change                   = WM_INPUTLANGCHANGE,
            input_language_change_request           = WM_INPUTLANGCHANGEREQUEST,
            move                                    = WM_MOVE,
            moving                                  = WM_MOVING,
            non_client_activate                     = WM_NCACTIVATE,
            non_client_calc_size                    = WM_NCCALCSIZE,
            non_client_create                       = WM_NCCREATE,
            non_client_destroy                      = WM_NCDESTROY,
            null                                    = WM_NULL,
            paint                                   = WM_PAINT,
            query_drag_icon                         = WM_QUERYDRAGICON,
            query_open                              = WM_QUERYOPEN,
            quit                                    = WM_QUIT,
            set_font                                = WM_SETFONT,
            set_icon                                = WM_SETICON,
            set_text                                = WM_SETTEXT,
            show_window                             = WM_SHOWWINDOW,
            size                                    = WM_SIZE,
            sizing                                  = WM_SIZING,
            style_changed                           = WM_STYLECHANGED,
            style_changing                          = WM_STYLECHANGING,
            theme_changed                           = WM_THEMECHANGED,
            user_changed                            = WM_USERCHANGED,
            window_position_changed                 = WM_WINDOWPOSCHANGED,
            window_position_changing                = WM_WINDOWPOSCHANGING,

            // Custom defined
            deferred_invoke                         = (WM_USER | 0x0127),
        };

        enum class virtual_key : uintptr_t
        {
            left_button             = VK_LBUTTON,
            right_button            = VK_RBUTTON,
            cancel                  = VK_CANCEL,
            middle_button           = VK_MBUTTON,
            close_button_1          = VK_XBUTTON1,
            close_button_2          = VK_XBUTTON2,
            backspace               = VK_BACK,
            tab                     = VK_TAB,
            clear                   = VK_CLEAR,
            carriage_return         = VK_RETURN,
            shift                   = VK_SHIFT,
            control                 = VK_CONTROL,
            alt                     = VK_MENU,
            pause                   = VK_PAUSE,
            caps_lock               = VK_CAPITAL,
            kana_mode               = VK_KANA,
            hangeul_mode            = VK_HANGEUL,
            hangul_mode             = VK_HANGUL,
            junja_mode              = VK_JUNJA,
            final_mode              = VK_FINAL,
            hanja_mode              = VK_HANJA,
            kanji_mode              = VK_KANJI,
            escape                  = VK_ESCAPE,
            convert                 = VK_CONVERT,
            non_convert             = VK_NONCONVERT,
            accept                  = VK_ACCEPT,
            mode_change_request     = VK_MODECHANGE,
            space                   = VK_SPACE,
            page_up                 = VK_PRIOR,
            page_down               = VK_NEXT,
            end                     = VK_END,
            home                    = VK_HOME,
            left                    = VK_LEFT,
            up                      = VK_UP,
            right                   = VK_RIGHT,
            down                    = VK_DOWN,
            select                  = VK_SELECT,
            print                   = VK_PRINT,
            execute                 = VK_EXECUTE,
            print_screen            = VK_SNAPSHOT,
            insert                  = VK_INSERT,
            del                     = VK_DELETE,
            help                    = VK_HELP,
            left_windows            = VK_LWIN,
            right_windows           = VK_RWIN,
            applications            = VK_APPS,
            computer_sleep          = VK_SLEEP,
            num_pad_0               = VK_NUMPAD0,
            num_pad_1               = VK_NUMPAD1,
            num_pad_2               = VK_NUMPAD2,
            num_pad_3               = VK_NUMPAD3,
            num_pad_4               = VK_NUMPAD4,
            num_pad_5               = VK_NUMPAD5,
            num_pad_6               = VK_NUMPAD6,
            num_pad_7               = VK_NUMPAD7,
            num_pad_8               = VK_NUMPAD8,
            num_pad_9               = VK_NUMPAD9,
            multiply                = VK_MULTIPLY,
            add                     = VK_ADD,
            separator               = VK_SEPARATOR,
            subtract                = VK_SUBTRACT,
            decimal                 = VK_DECIMAL,
            divide                  = VK_DIVIDE,
            f1                      = VK_F1,
            f2                      = VK_F2,
            f3                      = VK_F3,
            f4                      = VK_F4,
            f5                      = VK_F5,
            f6                      = VK_F6,
            f7                      = VK_F7,
            f8                      = VK_F8,
            f9                      = VK_F9,
            f10                     = VK_F10,
            f11                     = VK_F11,
            f12                     = VK_F12,
            f13                     = VK_F13,
            f14                     = VK_F14,
            f15                     = VK_F15,
            f16                     = VK_F16,
            f17                     = VK_F17,
            f18                     = VK_F18,
            f19                     = VK_F19,
            f20                     = VK_F20,
            f21                     = VK_F21,
            f22                     = VK_F22,
            f23                     = VK_F23,
            f24                     = VK_F24,
            num_lock                = VK_NUMLOCK,
            scroll_lock             = VK_SCROLL,
            left_shift              = VK_LSHIFT,
            right_shift             = VK_RSHIFT,
            left_control            = VK_LCONTROL,
            right_control           = VK_RCONTROL,
            left_alt                = VK_LMENU,
            right_alt               = VK_RMENU,
            browser_back            = VK_BROWSER_BACK,
            browser_forward         = VK_BROWSER_FORWARD,
            browser_refresh         = VK_BROWSER_REFRESH,
            browser_stop            = VK_BROWSER_STOP,
            browser_search          = VK_BROWSER_SEARCH,
            browser_favorites       = VK_BROWSER_FAVORITES,
            browser_home            = VK_BROWSER_HOME,
            volume_mute             = VK_VOLUME_MUTE,
            volume_down             = VK_VOLUME_DOWN,
            volume_up               = VK_VOLUME_UP,
            media_next_track        = VK_MEDIA_NEXT_TRACK,
            media_previous_track    = VK_MEDIA_PREV_TRACK,
            media_stop              = VK_MEDIA_STOP,
            media_play_pause        = VK_MEDIA_PLAY_PAUSE,
            launch_mail             = VK_LAUNCH_MAIL,
            launch_media_select     = VK_LAUNCH_MEDIA_SELECT,
            launch_app_1            = VK_LAUNCH_APP1,
            launch_app_2            = VK_LAUNCH_APP2,
            oem_1                   = VK_OEM_1,
            oem_plus                = VK_OEM_PLUS,
            oem_comma               = VK_OEM_COMMA,
            oem_minus               = VK_OEM_MINUS,
            oem_period              = VK_OEM_PERIOD,
            oem_2                   = VK_OEM_2,
            oem_3                   = VK_OEM_3,
            oem_4                   = VK_OEM_4,
            oem_5                   = VK_OEM_5,
            oem_6                   = VK_OEM_6,
            oem_7                   = VK_OEM_7,
            oem_8                   = VK_OEM_8,
            oem_102                 = VK_OEM_102,
            process                 = VK_PROCESSKEY,
            packet                  = VK_PACKET,
            attention               = VK_ATTN,
            cr_select               = VK_CRSEL,
            ex_select               = VK_EXSEL,
            erase_eof               = VK_EREOF,
            play                    = VK_PLAY,
            zoom                    = VK_ZOOM,
            pa_1                    = VK_PA1,
            oem_clear               = VK_OEM_CLEAR,

            // Alphabetic characters
            a                       = 'A',
            b                       = 'B',
            c                       = 'C',
            d                       = 'D',
            e                       = 'E',
            f                       = 'F',
            g                       = 'G',
            h                       = 'H',
            i                       = 'I',
            j                       = 'J',
            k                       = 'K',
            l                       = 'L',
            m                       = 'M',
            n                       = 'N',
            o                       = 'O',
            p                       = 'P',
            q                       = 'Q',
            r                       = 'R',
            s                       = 'S',
            t                       = 'T',
            u                       = 'U',
            v                       = 'V',
            w                       = 'W',
            x                       = 'X',
            y                       = 'Y',
            z                       = 'Z',
        };

#pragma endregion



        /*
         * window_class
         */
        struct window_class
        {
            // Members
            window_class_style  style;
            int                 class_extra;
            instance_handle     instance;
            icon_handle         icon;
            cursor_handle       cursor;
            brush_handle        background;
            tstring             menu_name;
            tstring             class_name;
            icon_handle         small_icon;

            // Default construct all members (to null/nullptr/etc.)
            window_class(const tstring &className) :
                style{},
                class_extra{},
                instance{},
                icon{},
                cursor{},
                background{},
                class_name(className),
                small_icon{}
            {
            }

            inline void use_defaults(void)
            {
                this->style = window_class_style::horizontal_redraw | window_class_style::vertical_redraw;
                this->cursor = load_cursor(nullptr, IDC_ARROW);
                this->background = reinterpret_cast<brush_handle>(system_color::background) + 1;
            }

            // Construct a WNDCLASSEX structure
            inline WNDCLASSEX get(void) const
            {
                WNDCLASSEX result
                {
                    sizeof(result),
                    static_cast<unsigned>(style),
                    nullptr,
                    class_extra,
                    0,
                    instance,
                    icon,
                    cursor,
                    background,
                    garbage::null_if_empty(menu_name),
                    garbage::null_if_empty(class_name),
                    small_icon
                };
                return result;
            }
        };



        /*
         * window_options
         */
        struct window_options
        {
            window_options(const tstring &windowName) :
                window_name(windowName),
                style(window_style::overlapped_window),
                x(CW_USEDEFAULT),
                y(CW_USEDEFAULT),
                width(CW_USEDEFAULT),
                height(CW_USEDEFAULT),
                parent(nullptr),
                menu(nullptr)
            {
            }

            tstring window_name;
            window_style style;
            int x;
            int y;
            int width;
            int height;
            window_handle parent;
            menu_handle menu;
        };



        /*
         * paint_struct
         */
        struct paint_struct final
        {
            device_context_handle device_context;
            PAINTSTRUCT ps;

            /*
             * Constructor(s)/Destructor
             */
            paint_struct(void) :
                device_context(nullptr),
                ps{},
                _window(nullptr)
            {
            }

            paint_struct(window_handle window) :
                _window(window)
            {
                this->device_context = begin_paint(window, &this->ps);
            }

            ~paint_struct(void)
            {
                if (this->_window)
                {
                    end_paint(this->_window, ps);
                }
            }

            // Cannot copy
            paint_struct(const paint_struct &) = delete;
            paint_struct &operator=(const paint_struct &) = delete;

            // Can move
            paint_struct(paint_struct &&other) :
                paint_struct()
            {
                this->swap(other);
            }

            paint_struct &operator=(paint_struct &&rhs)
            {
                assert(this != &rhs);
                this->swap(rhs);

                return *this;
            }



            void swap(paint_struct &other)
            {
                std::swap(this->_window, other._window);
                std::swap(this->device_context, other.device_context);
                std::swap(this->ps, other.ps);
            }



        private:

            window_handle _window;
        };



        /*
         * callback_handler
         */
        class window;
        struct callback_handler
        {
            /*
             * Types and Constants
             */
            using result_type = std::pair<bool, intptr_t>;
            using callback_type = std::function<result_type(window *, uintptr_t, intptr_t)>;
            static const size_t repeat_infinite = static_cast<size_t>(-1);



            /*
             * Construction
             */
            callback_handler(window_message message, callback_type callback) :
                callback_handler(message, repeat_infinite, false, std::move(callback))
            {
            }

            callback_handler(window_message message, size_t repeatCount, bool eatMessage, callback_type callback) :
                message(message),
                repeat_count(repeatCount),
                eat_message(eatMessage),
                callback(std::move(callback))
            {
            }



            /*
             * Members
             */
            window_message message;
            size_t repeat_count;
            bool eat_message;
            callback_type callback;
        };



        /*
         * window
         */
        class window
        {
        public:
            /*
             * Public Types
             */
            using message_result_type = callback_handler::result_type;
            using message_callback_type = callback_handler::callback_type;
            using deferred_callback_type = std::function<void(void)>;



        private:

            /*
             * Private Types
             */
            using CallbackEntryType = std::pair<size_t, callback_handler>;

            struct deferred_invoke_handler
            {
                deferred_invoke_handler(deferred_callback_type func) :
                    _func(std::move(func))
                {
                }

                void invoke()
                {
                    this->_func();
                }



            private:

                deferred_callback_type _func;
            };



        public:
            /*
             * Constructor(s)/Destructor
             */
            window() :
                _running(false),
                _nextCallbackId(0),
                _window(nullptr),
                _threadId(0)
            {
                // Initialize the default callback handlers. All of them repeat infinitely and never eat messages

                // Deferred Callback Handler
                this->add_callback_handler(
                    window_message::deferred_invoke,
                    [](window *sender, uintptr_t wparam, intptr_t /*lparam*/) -> message_result_type
                {
                    return sender->OnDeferredCallback(wparam);
                });

                // WM_DESTROY
                this->add_callback_handler(
                    window_message::destroy,
                    [](window *sender, uintptr_t /*wparam*/, intptr_t /*lparam*/) -> message_result_type
                {
                    return std::make_pair(sender->on_destroy(), 0);
                });
            }

            virtual ~window(void)
            {
            }



            uintptr_t run(const window_class &windowClass, const window_options &options, int cmdShow)
            {
                // Can only call run once
                this->EnsureWindowUninitialized();
                auto cleanup = make_scope_exit([&]() { this->_running = false; });
                this->_running = true;

                // Calling thread becomes the "owner"/ui thread
                this->_threadId = get_current_thread_id();

                // Register the class
                auto wndclass = windowClass.get();
                wndclass.lpfnWndProc = uninitialized_window_procedure;
                wndclass.cbWndExtra = sizeof(this);
                register_class(wndclass);

                // Create the window
                this->_window = create_window(
                    garbage::null_if_empty(windowClass.class_name),     // Class Name
                    garbage::null_if_empty(options.window_name),        // Window Name
                    static_cast<uint32_t>(options.style),               // Window Style
                    options.x,                                          // X position
                    options.y,                                          // Y position
                    options.width,                                      // Width
                    options.height,                                     // Height
                    options.parent,                                     // Parent Window
                    options.menu,                                       // Menu Handle
                    windowClass.instance,                               // Instance Handle
                    this);                                              // lpCreateParams

                this->initialize();
                if (this->_initializeCallback)
                {
                    this->_initializeCallback();
                }

                // Make the window visible
                show_window(this->_window, cmdShow);
                update_window(this->_window);

                return this->message_pump();
            }



            /*
             * Public Functions
             */
#pragma region Public Functions

            tid_t thread_id(void) const
            {
                return this->_threadId;
            }

            bool is_ui_thread(void) const
            {
                return (this->_threadId == get_current_thread_id());
            }

            window_handle handle(void) const
            {
                return this->_window;
            }

            bool running(void) const
            {
                return this->_running;
            }

            rect<size_t> window_rect(void) const
            {
                auto result = get_window_rect(this->_window);
                return rect<size_t>(result.left, result.top, result.right - result.left, result.bottom - result.top);
            }

            rect<size_t> client_rect(void) const
            {
                auto result = get_client_rect(this->_window);
                return rect<size_t>(result.left, result.top, result.right - result.left, result.bottom - result.top);
            }

            paint_struct begin_paint(void)
            {
                EnsureWindowInitialized();
                return paint_struct(this->_window);
            }

            void invalidate(bool eraseBackground = true, rect<size_t> *area = nullptr)
            {
                EnsureWindowInitialized();
                if (area)
                {
                    RECT rc;
                    rc.left = area->x;
                    rc.right = rc.left + area->width;
                    rc.top = area->y;
                    rc.bottom = rc.top + area->height;
                    invalidate_rect(this->_window, &rc, eraseBackground);
                }
                else
                {
                    invalidate_rect(this->_window, nullptr, eraseBackground);
                }
            }

#pragma endregion



            /*
             * Callbacks and Posting to UI Thread
             */
#pragma region Callbacks

            void post_async(const deferred_callback_type &func)
            {
                auto handler = std::make_unique<deferred_invoke_handler>(func);
                this->Post(std::move(handler));
            }

            void post_and_wait(const deferred_callback_type &func)
            {
                std::mutex lock;
                std::unique_lock<std::mutex> guard(lock);
                std::condition_variable cond;
                bool completed = false;

                this->post_async([&]()
                {
                    func();

                    // Signal that we're done
                    std::unique_lock<std::mutex> guard2(lock);
                    completed = true;
                    cond.notify_one();
                });

                // Wait for the callback to set completed to true
                cond.wait(guard, [&]() -> bool { return completed; });
            }

            size_t add_callback_handler(const callback_handler &handler)
            {
                size_t callbackId = ++this->_nextCallbackId;

                // If run() has not been called, then we cannot post to the UI thread. It is then assumed that
                // add_callback_handler is being called by the thread creating the window. If this is not true, then
                // there could be race conditions for writing to the map/vectors
                if (this->_threadId)
                {
                    // Already running; post
                    this->post_async([this, handler, callbackId]() mutable
                    {
                        this->AddCallbackHandler(callback_handler(handler), callbackId);
                    });
                }
                else
                {
                    // Not running; add now
                    this->AddCallbackHandler(callback_handler(handler), callbackId);
                }

                return callbackId;
            }

            size_t add_callback_handler(window_message message, const message_callback_type &func)
            {
                return this->add_callback_handler(callback_handler(message, func));
            }

            void on_initialized(const deferred_callback_type &callback)
            {
                // There can only be one initialize callback
                this->_initializeCallback = callback;
            }

#pragma endregion



        protected:
            /*
             * Message Pump
             */
            virtual uintptr_t message_pump()
            {
                // Default is to use GetMessage. Derived classes can override this functionality if they wish to use
                // other means (e.g. PeekMessage)
                MSG msg;
                while (get_message(msg))
                {
                    translate_message(msg);
                    dispatch_message(msg);
                }

                return msg.wParam;
            }



            /*
             * Custom setup code that runs after the window is created, but before invoking the initialization callback
             */
            virtual void initialize(void)
            {
            }



            /*
             * Override-able window message functions
             */
#pragma region Window Messages

            virtual bool on_destroy(void)
            {
                post_quit_message(0);
                return true;
            }

#pragma endregion



            virtual intptr_t window_procedure(window_message message, uintptr_t wparam, intptr_t lparam)
            {
                message_result_type result(false, 0);

                // Check to see if any handlers exist for the message
                auto list_itr = this->_callbackHandlers.find(message);
                if (list_itr != std::end(this->_callbackHandlers))
                {
                    auto &list = list_itr->second;

                    // Save the return value so we can save time in the call to list.erase in the event that an early
                    // handler eats the message
                    auto itr = std::find_if(std::rbegin(list), std::rend(list), [&](CallbackEntryType &entry)
                    {
                        auto &handler = entry.second;
                        auto tempResult = handler.callback(this, wparam, lparam);

                        if (tempResult.first)
                        {
                            // Handled
                            result = tempResult;

                            // We use a repeat_count of zero to indicate that the hanler should
                            // only get run once regardless of if it handles the message or not
                            if (handler.repeat_count != callback_handler::repeat_infinite && handler.repeat_count != 0)
                            {
                                --handler.repeat_count;
                            }
                            if (handler.eat_message)
                            {
                                return true;
                            }
                        }

                        // Handler did not eat message; continue searching
                        return false;
                    });

                    // Need to remove all handlers with a repeat_count of zero
                    list.erase(std::remove_if(itr.base(), std::end(list), [](CallbackEntryType &entry) -> bool
                    {
                        return entry.second.repeat_count == 0;
                    }), std::end(list));
                }

                if (!result.first)
                {
                    return default_window_procedure(this->_window, static_cast<unsigned>(message), wparam, lparam);
                }

                return result.second;
            }



        private:

            /*
             * Ensuring valid state
             */
            void EnsureWindowInitialized(void)
            {
                if (!this->_window || !this->_running)
                {
                    throw hresult_exception(E_HANDLE);
                }
            }

            void EnsureWindowUninitialized(void)
            {
                if (this->_window)
                {
                    throw hresult_exception(E_ILLEGAL_METHOD_CALL);
                }
            }



            /*
             * Private/Non-Override-able Message Handling
             */
            void AddCallbackHandler(callback_handler &&handler, size_t callbackId)
            {
                this->_callbackHandlers[handler.message].emplace_back(callbackId, std::move(handler));
            }

            message_result_type OnDeferredCallback(uintptr_t wparam)
            {
                std::unique_ptr<deferred_invoke_handler> handler(reinterpret_cast<deferred_invoke_handler *>(wparam));

                handler->invoke();

                // Always handle, and the result value is not important
                return std::make_pair(true, 0);
            }

            void Post(std::unique_ptr<deferred_invoke_handler> handler)
            {
                // Cannot post before the window is created
                this->EnsureWindowInitialized();

                post_message(
                    this->_window,
                    static_cast<unsigned int>(window_message::deferred_invoke),
                    reinterpret_cast<uintptr_t>(handler.get()),
                    0);

                // unique_ptr is used in the case of an exception. Since none happened, detach
                handler.release();
            }



            /*
             * Window Procedure
             */
            static LRESULT CALLBACK uninitialized_window_procedure(
                window_handle window,
                unsigned int message,
                WPARAM wparam,
                LPARAM lparam)
            {
                auto msg = static_cast<window_message>(message);
                if (msg == window_message::create)
                {
                    CREATESTRUCT *vars = reinterpret_cast<CREATESTRUCT *>(lparam);
                    set_window_long_ptr(window, GWL_USERDATA, reinterpret_cast<uintptr_t>(vars->lpCreateParams));
                    set_window_long_ptr(window, GWL_WNDPROC, reinterpret_cast<uintptr_t>(initialized_window_procedure));

                    return initialized_window_procedure(window, message, wparam, lparam);
                }
                else
                {
                    return default_window_procedure(window, message, wparam, lparam);
                }
            }

            static LRESULT CALLBACK initialized_window_procedure(
                window_handle windowHandle,
                unsigned int message,
                WPARAM wparam,
                LPARAM lparam)
            {
                window *ptr = reinterpret_cast<window *>(get_window_long_ptr(windowHandle, GWL_USERDATA));
                assert(ptr);

                return ptr->window_procedure(static_cast<window_message>(message), wparam, lparam);
            }



            bool _running;
            deferred_callback_type _initializeCallback;
            std::map<window_message, std::vector<CallbackEntryType>> _callbackHandlers;
            std::atomic_size_t _nextCallbackId;

            // Only valid after window creation...
            window_handle _window;
            tid_t _threadId;
        };
    }
}
