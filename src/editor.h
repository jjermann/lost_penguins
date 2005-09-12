#ifndef DEF_EDITOR_H
#define DEF_EDITOR_H 1

#define EDIT_RESET_ACTIONS  0x00000001
#define EDIT_MOUSE_MOTION   0x00000002
#define EDIT_BOX            0x00000004
#define EDIT_ACT_BOX        0x00000008
#define EDIT_SEL_ACT_BOX    0x00000010
#define EDIT_PLACE_OBJECT   0x00000020
#define EDIT_REMOVE_OBJECTS 0x00000040
#define EDIT_SELECT_START   0x00000080
#define EDIT_SELECT_END     0x00000100
#define EDIT_MOVE_START     0x00000200
#define EDIT_MOVE_END       0x00000400

/** \brief abstract Box base class

*/
class Box {
    friend class Editor;
    friend class GraphicsEngine;
    public:
        Box(Sint16,Sint16);
        virtual ~Box();
        /// Name of the box
        string title;
        /// Activate specified entry, if no entry is selected this is -1
        virtual void act(Sint8) = 0;
        virtual void input(Uint16) { }
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
        virtual void update();
        /// True if the entries should be draw centered
        bool centered;
};

/** \brief Main Box of the editor with all main options

*/
class EditBox : public Box {
    public:
        EditBox(Sint16,Sint16);
        virtual void act(Sint8);
};

/** \brief Box used to choose the object that is to be placed

*/
class PlaceBox : public Box {
    public:
        PlaceBox(Sint16,Sint16);
        virtual void act(Sint8);
};

/** \brief Abstract base class for text input boxes

*/
class TextInputBox : public Box {
    public:
        TextInputBox(Sint16,Sint16);
        virtual ~TextInputBox();
        virtual void act(Sint8);
        virtual void input(Uint16);
        virtual void evaluateEntry();
    protected:
        Sint8 currententry;
        std::vector<string> etitles;
        std::vector<string> einput;
        virtual void update();
};

/** \brief Configure object attributes

*/
class ObjectBox : public TextInputBox {
    public:
        ObjectBox(string,Sint16,Sint16,ParameterMap& parameters);
    protected:
        virtual void evaluateEntry();
        string objname;
};

/** \brief Saves the map to the specified file

*/
class SaveAsBox : public TextInputBox {
    public:
        SaveAsBox(Sint16,Sint16);
    protected:
        virtual void evaluateEntry();
};

/** \brief Opens the specified map

*/
class OpenMapBox : public TextInputBox {
    public:
        OpenMapBox(Sint16,Sint16);
    protected:
        virtual void evaluateEntry();
};

/** \brief Creates a new map with the specified attributes

*/
class NewMapBox : public TextInputBox {
    public:
        NewMapBox(Sint16,Sint16);
    protected:
        virtual void evaluateEntry();
};


/** \brief Handels editor specific tasks

*/
class Editor {
    public:
        Editor();
        ~Editor();
        void reinit();
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
        /// Name the saved map file
        string save_name;
        /// Object to place when clicking the left mouse button
        string place_name;
        /// Parameter list used to create the object
        ParameterMap place_parameters;
        /// Append a command to the buffered map file
        void appendtoBuf(string);
        string getBufLine(string match);
        /// Change the specified line in the buffered map file
        bool changeBuf(string,string);
        /* TODO: add header modifiers */
        /// Remove the first line in the buffered map file matching the specified string
        string removefromBuf(string);
        /// Save the map file buffer to the specified file if possible
        int saveBuf(string);
        /// Action type for the mouse buttons when clicking
        Uint32 action_mouse_pressed[6];
        /// Action type for the mouse buttons when releasing the button
        Uint32 action_mouse_released[6];
        std::set<string> selection;
        bool select_start;
        SDL_Rect select_rect;
        Sint16 select_start_x;
        Sint16 select_start_y;
        bool move_start;
        Sint16 move_start_x;
        Sint16 move_start_y;
        SDL_Surface* mask_surface;
        Object* move_object;
    private:
        Sint16 getBufLineNr(string);
        void updateSelection(Sint16 x, Sint16 y);
        bool updateMove(Sint16 x, Sint16 y);
};

#endif
