#ifndef DEF_EDITOR_H
#define DEF_EDITOR_H 1

#define EDIT_RESET_ACTIONS  0x00000001
#define EDIT_BOX            0x00000004
#define EDIT_ACT_BOX        0x00000008
#define EDIT_PLACE_OBJECT   0x00000010

/** \brief abstract selection box base class

*/
class Box {
    friend class Editor;
    friend class GraphicsEngine;
    public:
        Box(Sint16,Sint16);
        virtual ~Box();
        /// Name of the menu
        string title;
        /// Main menu function, depends on currententry (changed by input), overload this.
        virtual void act(Sint8) = 0;
        Uint8 getSize() {
            return entries.size();
        }
        const SDL_Rect& getArea();
        Sint8 getCurrentEntry(Sint16,Sint16);
    protected:
        /// Menu surface to be drawn
        SDL_Surface* surface;
        /// List of all Box entry names
        std::vector<string> entries;
        /// Font for non highlighted menu entries
        Font* font;
        /// Font for the menu title
        Font* font_title;
        /// Font for the selected menu entry
        Font* font_high;
        /// Area occupied by the selection box
        SDL_Rect area;
        /// Draw the box surface
        void update();
};

class EditBox : public Box {
    public:
        EditBox(Sint16,Sint16);
        virtual void act(Sint8);
};

/** \brief Handels editor specific tasks

*/
class Editor {
    friend class EditBox;
    public:
        Editor();
        ~Editor();
        void run_action(Uint32 action, Uint16 x=0, Uint16 y=0);
        Uint32 getActionMPressed(Uint8 button) {
            if (button>=1 && button <6) return action_mouse_pressed[button];
            else return NOTHING;
        }
        Uint32 getActionMReleased(Uint8 button) {
            if (button>=1 && button <6) return action_mouse_released[button];
            else return NOTHING;
        }
        /// Set the current box
        Box* setBox(Box* newbox);
        /// Close the current box
        void closeBox(); 
        bool hasBox() {
            if (box && box->surface) return true;
            else return false;
        }
        Box* box;
    protected:
        /// Name the saved map file
        string save_name;
        /// Object to place when clicking the left mouse button
        string place_name;
        /// Image name of the placed object
        string place_image;
        /// Action type for the mouse buttons when clicking
        Uint32 action_mouse_pressed[6];
        /// Action type for the mouse buttons when releasing the button
        Uint32 action_mouse_released[6];
        /// Append a command to the buffered map file
        void appendtoBuf(string);
        /* TODO: add header modifiers */
        /// Remove the first line in the buffered map file matching the specified string
        string removefromBuf(string);
        /// Save the map file buffer to the specified file if possible
        int saveBuf(string);
};

#endif